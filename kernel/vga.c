// BlacklightEVO kernel/vga.c -- quick and dirty VGA terminal
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <terminal.h>
#include <vga.h>

#define vga_collapse(c,k) ((uint16_t) c | ((uint16_t) k) << 8)
 
void vga_terminal_initialize(struct terminal_info* term, int width, int height) {
	term->row = 0;
	term->column = 0;
	term->color = 0x07;
	term->width = width;
	term->height = height;
	
	term->initialize = vga_terminal_initialize;
	term->putchar = vga_terminal_putchar;
	term->writestring = vga_terminal_writestring;
	
	for (int y = 0; y < term->height; y++) {
		for (int x = 0; x < term->width; x++) {
			int index = y * term->width + x;
			((unsigned short*)(term->textbuffer))[index] = vga_collapse(' ', term->color);
		}
	}
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
	if (c == '\n') {
		term->column = 0;
		term->row++;
	} else if (c == '\r') {
		term->column = 0;
	} else {
		vga_terminal_putentryat(term, c, term->color, term->column, term->row);
		if (++(term->column) == term->width) {
			term->column = 0;
			term->row++;
		}
	}
	
	if (term->row == term->height) {
		term->row--;
		memcpy((unsigned short*)(term->textbuffer), (unsigned short*)(term->textbuffer) + term->width * 1, term->width * (term->height - 1) * 2);
		for (int x = 0; x < term->width; x++) {
			const size_t index = (term->height - 1) * term->width + x;
			((unsigned short*)(term->textbuffer))[index] = vga_collapse(' ', term->color);
		}
	}
	
	vga_update_cursor(term);
}
 
void vga_terminal_writestring(struct terminal_info* term, const char* data) {
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		vga_terminal_putchar(term, data[i]);
}
