// BlacklightEVO kernel/vga.c -- quick and dirty VGA terminal
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <vga.h>

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;
 
size_t vga_terminal_row;
size_t vga_terminal_column;
uint8_t vga_terminal_color;
uint16_t* vga_terminal_buffer;

#define vga_collapse(c,k) ((uint16_t) c | ((uint16_t) k) << 8)
 
void vga_terminal_initialize() {
	vga_terminal_row = 0;
	vga_terminal_column = 0;
	vga_terminal_color = 0x07;
	vga_terminal_buffer = (uint16_t*) 0xB8000;
	for ( size_t y = 0; y < VGA_HEIGHT; y++ ) {
		for ( size_t x = 0; x < VGA_WIDTH; x++ ) {
			const size_t index = y * VGA_WIDTH + x;
			vga_terminal_buffer[index] = vga_collapse(' ', vga_terminal_color);
		}
	}
}
 
inline void vga_terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	vga_terminal_buffer[index] = vga_collapse(c, color);
}

void vga_update_cursor(int x, int y) {
	unsigned short location = y * VGA_WIDTH + x;
	outb(0x3D4, 14);
	outb(0x3D5, location >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, location);
}
 
void vga_terminal_putchar(char c) {
	if (c == '\n') {
		vga_terminal_column = 0;
		vga_terminal_row++;
	} else if (c == '\r') {
		vga_terminal_column = 0;
	} else {
		vga_terminal_putentryat(c, vga_terminal_color, vga_terminal_column, vga_terminal_row);
		if (++vga_terminal_column == VGA_WIDTH)
			vga_terminal_column = 0;
	}
	
	if (vga_terminal_row == VGA_HEIGHT) {
		vga_terminal_row--;
		memcpy(vga_terminal_buffer, vga_terminal_buffer + VGA_WIDTH * 1, VGA_WIDTH * (VGA_HEIGHT - 1) * 2);
		for ( size_t x = 0; x < VGA_WIDTH; x++ ) {
			const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
			vga_terminal_buffer[index] = vga_collapse(' ', vga_terminal_color);
		}
	}
	
	vga_update_cursor(vga_terminal_column, vga_terminal_row);
}
 
void vga_terminal_writestring(const char* data) {
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		vga_terminal_putchar(data[i]);
}
