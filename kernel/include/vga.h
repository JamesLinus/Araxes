// BlacklightEVO kernel/include/vga.h -- header for the QNS VGA terminal
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__VGA_H
#define __KERNEL__INCLUDE__VGA_H

#include <terminal.h>

void vga_terminal_initialize(struct terminal_info* term, int width, int height, unsigned char* textbuffer);
void vga_terminal_putchar(struct terminal_info* term, char c);
void vga_terminal_write(struct terminal_info* term, const char* data, size_t length);
void vga_terminal_writestring(struct terminal_info* term, const char* data);
void vga_update_cursor(struct terminal_info* term);

#endif	// __KERNEL__INCLUDE__VGA_H
