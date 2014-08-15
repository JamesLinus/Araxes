// BlacklightEVO core\console.cpp -- Generic console driver
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\console.h"
#include "core\mm.h"
#include "video\fb_text.h"
#include "video\vga_text.h"
#include "video\vt100.h"
#include "hardware\keyboard.h"

int console_cursor_x = 0, console_cursor_y = 0;
int console_width = 80, console_height = 25;
unsigned char console_attrib = 0x07;
int console_mode = CONSOLE_VGA;
int console_palette = PALETTE_EGA;
unsigned int console_fg = 0x07, console_bg = 0x00;
int console_input = CONSOLE_KEYBOARD;

void console_putchar(char c) {
	if (c == 0x08 && console_cursor_x) {
		console_cursor_x--;
		if (console_mode == CONSOLE_VGA)
			vga_putchar(' ');
		if (CONSOLE_IS_FRAMEBUFFER)
			fb_putchar(' ');
	}

	else if (c == '\t') {
		for (int i = 0; i < ((console_cursor_x % 8) ? console_cursor_x % 8 : 8); i++)
			console_putchar(' ');					// Hell yeah recursiveness.
	}

	else if (c == '\n') {
		console_cursor_x = 0;
		console_cursor_y++;
	}

	else if (c == '\r')
		console_cursor_x = 0;

	else if(c >= ' ') {
		if (console_mode == CONSOLE_VGA)
			vga_putchar(c);
		if (CONSOLE_IS_FRAMEBUFFER)
			fb_putchar(c);
		console_cursor_x++;
	}

	if (console_cursor_x >= console_width) {
		console_cursor_x = 0;
		console_cursor_y++;
	}

	if (console_cursor_y >= console_height) {
		if (console_mode == CONSOLE_VGA)
			vga_scroll();
		if (CONSOLE_IS_FRAMEBUFFER)
			fb_scroll();
		console_cursor_y = console_height - 1;
	}

	if (console_mode == CONSOLE_VGA)
		vga_update_cursor();
	if (CONSOLE_IS_FRAMEBUFFER)
		fb_update_cursor(false);
}

void console_print(char* s) {
	while (*s)
		console_putchar(*s++);
}

void console_clear(void) {
	console_cursor_x = console_cursor_y = 0;

	if (console_mode == CONSOLE_VGA) {
		vga_clear_screen();
		vga_update_cursor();
	}
	if (CONSOLE_IS_FRAMEBUFFER) {
		fb_clear_screen();
		fb_update_cursor(true);
	}
}
