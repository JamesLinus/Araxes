// BlacklightEVO video\vga_text.cpp -- VGA text mode specific stuff
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "video\vga_text.h"
#include "core\console.h"

void* vga_text_fb = (void*)0x000B8000;

void vga_update_cursor(void) {
	unsigned short location = console_cursor_y * 80 + console_cursor_x;

	outb(0x3D4, 14);
	outb(0x3D5, location >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, location);
}

void vga_clear_screen(void) {
	unsigned short word = ((console_attrib << 8) | ' ');
	for (int i = 0; i < 80*25; i++)
		((unsigned short*)vga_text_fb)[i] = word;
}

void vga_scroll(void) {
	memcpy(vga_text_fb, (void*)((unsigned int)vga_text_fb+80*2),80*24*2);

	unsigned short word = ((console_attrib << 8) | ' ');
	for (int i = 80*24; i < 80*25; i++)
		((unsigned short*)vga_text_fb)[i] = word;
}

void vga_putchar(char c) {
	((unsigned short*)vga_text_fb)[console_cursor_y*80 + console_cursor_x] = c | (console_attrib << 8);
}
