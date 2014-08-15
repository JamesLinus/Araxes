// BlacklightEVO video\vga_text.h -- VGA text mode specific stuff
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __VIDEO__VGA_TEXT_H
#define __VIDEO__VGA_TEXT_H

void vga_update_cursor(void);
void vga_clear_screen(void);
void vga_scroll(void);
void vga_putchar(char c);

#endif	// __VIDEO__VGA_TEXT_H