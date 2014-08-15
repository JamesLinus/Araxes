// BlacklightEVO kernel/include/terminal.h -- terminal handling and switching
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__TERMINAL_H
#define __KERNEL__INCLUDE__TERMINAL_H

#define CONSOLE_VGA 0x00
#define CONSOLE_BOCHS 0x01
#define CONSOLE_VBOX 0x02
#define CONSOLE_VBE 0x03

#define CONSOLE_KEYBOARD 0x00

#define PALETTE_EGA 0x00
#define PALETTE_XTERM 0x01
#define PALETTE_TRUECOLOUR 0x02

struct terminal_info {
	int row, column;
	int width, height;
	unsigned char color;
	int mode;
	int palette;
	unsigned int fg, bg;
	int input;
	
	unsigned char* textbuffer, framebuffer;
	void (*initialize)(struct terminal_info* term, int width, int height);
	void (*putchar)(struct terminal_info* term, char c);
	void (*writestring)(struct terminal_info* term, const char* data);
};

extern struct terminal_info default_terminal;
extern struct terminal_info* current_terminal;
#define console_print(s) current_terminal->writestring(current_terminal, s)

#endif	// __KERNEL__INCLUDE__TERMINAL_H
