// BlacklightEVO kernel/vga.c -- quick and dirty VGA terminal
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <terminal.h>
#include <vga.h>

#define vga_collapse(c,k) ((uint16_t) c | ((uint16_t) k) << 8)
 
void vga_terminal_initialize(struct terminal_info* term, int width, int height, unsigned char* textbuffer) {
	term->row = 0;
	term->column = 0;
	term->default_color = 0x07;
	term->color = 0x07;
	term->width = width;
	term->height = height;
	term->status = TERMINAL_STATUS_FREE;
	term->textbuffer = textbuffer;
	
	term->initialize = vga_terminal_initialize;
	term->putchar = vga_terminal_putchar;
	term->write = vga_terminal_write;
	term->writestring = vga_terminal_writestring;
	term->update_cursor = vga_update_cursor;
	
	for (int y = 0; y < term->height; y++) {
		for (int x = 0; x < term->width; x++) {
			int index = y * term->width + x;
			((unsigned short*)(term->textbuffer))[index] = vga_collapse(' ', term->color);
		}
	}
	debug_printf(LOG_INFO "Initializing VGA terminal at 0x%8X (%p, %p, %p).\n", term->textbuffer, term->initialize, term->putchar, term->writestring);
}
 
static inline void vga_terminal_putentryat(struct terminal_info* term, char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * term->width + x;
	((unsigned short*)(term->textbuffer))[index] = vga_collapse(c, color);
}

void vga_update_cursor(struct terminal_info* term) {
	unsigned short location = term->row * term->width + term->column;
	outb(0x3D4, 14);
	outb(0x3D5, location >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, location);
}
 
void vga_terminal_putchar(struct terminal_info* term, char c) {
	bool cursor_dirty = false;
	
	if (term->status == TERMINAL_STATUS_FREE) {
		if (c == '\n') {				// Line feed (implies carriage return)
			term->column = 0;
			term->row++;
			cursor_dirty = true;
		} else if (c == '\r') {				// Carriage return
			term->column = 0;
			cursor_dirty = true;
		} else {
			vga_terminal_putentryat(term, c, term->color, term->column, term->row);
			cursor_dirty = true;
			if (++(term->column) == term->width) {
				term->column = 0;
				term->row++;
			}
		}
	}
	
	if (term->row == term->height) {
		cursor_dirty = true;
		term->row--;
		memcpy((unsigned short*)(term->textbuffer), (unsigned short*)(term->textbuffer) + term->width * 1, term->width * (term->height - 1) * 2);
		for (int x = 0; x < term->width; x++) {
			const size_t index = (term->height - 1) * term->width + x;
			((unsigned short*)(term->textbuffer))[index] = vga_collapse(' ', term->color);
		}
	}
	
	if (cursor_dirty)
		term->update_cursor(term);		// This can be slow on some VGA hardware, so do it only if necessary.
}

void vga_terminal_writestring(struct terminal_info* term, const char* data) {
	vga_terminal_write(term, data, strlen(data));
}
 
void vga_terminal_write(struct terminal_info* term, const char* data, size_t length) {
	size_t datalen = length;
	size_t orig_i;
	int n_i = 0;
	int n[10] = {0};
	
	for ( size_t i = 0; i < datalen; i++ ) {
		if (data[i] == '\x1B' || data[i] == '\x9B' && term->status != TERMINAL_STATUS_ANSI) {		// ANSI escape character
			n_i = 0;
			for (int j = 0; j < 10; j++)
				n[j] = 0;
			orig_i = i;

			if (data[i] == '\x9B')		// C1 control 9B is a single-byte CSI
				term->status = TERMINAL_STATUS_ANSI;

			//debug_printf(LOG_INFO "Possible ANSI string encountered.\n");
			while (i++ < datalen) {
				if (data[i] == '[') {
					term->status = TERMINAL_STATUS_ANSI;
					//_debug_printf(LOG_INFO "Beginning ANSI processing");
					continue;
				}
				
				if (term->status == TERMINAL_STATUS_ANSI) {
					//_debug_printf(".");
					if (data[i] == ' ')
						continue;		// XXX: Is this a valid way of handling whitespace?
					else if (data[i] == ';') {
						if (n_i < 10)
							n_i++;
					} else if (data[i] >= '0' && data[i] <= '9') {
						n[n_i] *= 10;
						n[n_i] += (data[i]-'0');
					} else if (data[i] > 126)
						continue;		// XXX: Is this a valid way of handling invalid characters?
					else {
						switch (data[i]) {
							case 'm':
								n_i++;
								for (int j = 0; j < n_i; j++) {
									switch (n[j]) {
										case 0:		// Reset attributes to default
											term->color = term->default_color;
											break;
										case 1:		// Bold or increase intensity
											term->color |= 0x08;
											break;
										case 21:	// Bold off
										case 22:	// Normal intensity
											term->color &= 0xF7;
											break;
										case 30:
											term->color &= 0xF8;
											break;
										case 31:
											term->color &= 0xF8;
											term->color += 4;
											break;
										case 32:
											term->color &= 0xF8;
											term->color += 2;
											break;
										case 33:
											term->color &= 0xF8;
											term->color += 6;
											break;
										case 34:
											term->color &= 0xF8;
											term->color += 1;
											break;
										case 35:
											term->color &= 0xF8;
											term->color += 5;
											break;
										case 36:
											term->color &= 0xF8;
											term->color += 3;
											break;
										case 37:
											term->color &= 0xF8;
											term->color += 7;
											break;
										case 39:
											term->color &= 0xF8;
											term->color += term->default_color & 0x07;
											break;
										case 40:
											term->color &= 0x8F;
											break;
										case 41:
											term->color &= 0x8F;
											term->color += 0x40;
											break;
										case 42:
											term->color &= 0x8F;
											term->color += 0x20;
											break;
										case 43:
											term->color &= 0x8F;
											term->color += 0x60;
											break;
										case 44:
											term->color &= 0x8F;
											term->color += 0x10;
											break;
										case 45:
											term->color &= 0x8F;
											term->color += 0x50;
											break;
										case 46:
											term->color &= 0x8F;
											term->color += 0x30;
											break;
										case 47:
											term->color &= 0x8F;
											term->color += 0x70;
											break;
										case 49:
											term->color &= 0x8F;
											term->color += term->default_color & 0x70;
											break;
									}
								}
								term->status = TERMINAL_STATUS_FREE;
								break;
							default:
								i = orig_i;
								term->status = TERMINAL_STATUS_FREE;
								vga_terminal_putchar(term, data[i]);
								i++;
								vga_terminal_putchar(term, data[i]);
								break;
						}
					}
				} else {
					i--;
					break;
				}
			};
		} else {
			vga_terminal_putchar(term, data[i]);
		}
	}
}
