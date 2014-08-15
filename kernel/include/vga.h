// BlacklightEVO kernel/include/vga.h -- header for the QNS VGA terminal
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__VGA_H
#define __KERNEL__INCLUDE__VGA_H

void vga_terminal_initialize();
void vga_terminal_putchar(char c);
void vga_terminal_writestring(const char* data);

#endif	// __KERNEL__INCLUDE__VGA_H
