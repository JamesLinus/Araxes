// BlacklightEVO kernel/include/vga.h -- header for the QNS VGA terminal
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__VGA_H
#define __KERNEL__INCLUDE__VGA_H

#include <terminal.h>

void vga_terminal_initialize(struct terminal_info* term, int width, int height, unsigned char* textbuffer);
void vga_terminal_putchar(struct terminal_info* term, char c);
void vga_terminal_writestring(struct terminal_info* term, const char* data);
void vga_update_cursor(struct terminal_info* term);

#endif	// __KERNEL__INCLUDE__VGA_H
