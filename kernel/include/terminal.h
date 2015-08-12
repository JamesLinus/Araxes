// BlacklightEVO kernel/include/terminal.h -- terminal handling and switching
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__TERMINAL_H
#define __KERNEL__INCLUDE__TERMINAL_H

#include <global.h>

#define CONSOLE_VGA 0x00
#define CONSOLE_BOCHS 0x01
#define CONSOLE_VBOX 0x02
#define CONSOLE_VBE 0x03

#define CONSOLE_KEYBOARD 0x00

#define PALETTE_EGA 0x00
#define PALETTE_XTERM 0x01
#define PALETTE_TRUECOLOUR 0x02

#define VT100_SGR_NORMAL	"\x1B[0m"
#define VT100_SGR_BOLD		"\x1B[1m"
#define VT100_SGR_DULL		"\x1B[2m"
#define VT100_SGR_UNDERLINE	"\x1B[4m"
#define VT100_SGR_BLINKING	"\x1B[5m"
#define VT100_SGR_REVERSE	"\x1B[7m"
#define VT100_SGR_INVISIBLE	"\x1B[8m"

#define VT100_SGR_FG_BLACK		"\x1B[30m"
#define VT100_SGR_FG_RED		"\x1B[31m"
#define VT100_SGR_FG_GREEN		"\x1B[32m"
#define VT100_SGR_FG_YELLOW		"\x1B[33m"
#define VT100_SGR_FG_BLUE		"\x1B[34m"
#define VT100_SGR_FG_MAGENTA	"\x1B[35m"
#define VT100_SGR_FG_CYAN		"\x1B[36m"
#define VT100_SGR_FG_WHITE		"\x1B[37m"
#define VT100_SGR_FG_XTERM(x)	"\x1B[38;5;" STR(x) "m"
#define VT100_SGR_FG_RGB(r,g,b)	"\x1B[38;2;" STR(r) ";" STR(g) ";" STR(b) "m"
#define VT100_SGR_FG_DEFAULT	"\x1B[39m"

#define VT100_SGR_BG_BLACK		"\x1B[40m"
#define VT100_SGR_BG_RED		"\x1B[41m"
#define VT100_SGR_BG_GREEN		"\x1B[42m"
#define VT100_SGR_BG_YELLOW		"\x1B[43m"
#define VT100_SGR_BG_BLUE		"\x1B[44m"
#define VT100_SGR_BG_MAGENTA	"\x1B[45m"
#define VT100_SGR_BG_CYAN		"\x1B[46m"
#define VT100_SGR_BG_WHITE		"\x1B[47m"
#define VT100_SGR_BG_XTERM(x)	"\x1B[48;5;" STR(x) "m"
#define VT100_SGR_BG_RGB(r,g,b)	"\x1B[48;2;" STR(r) ";" STR(g) ";" STR(b) "m"
#define VT100_SGR_BG_DEFAULT	"\x1B[49m"

#define LOG_TIME "[%10llu] "
#define LOG_INFO VT100_SGR_NORMAL VT100_SGR_FG_DEFAULT VT100_SGR_BG_DEFAULT LOG_TIME "[INFO] "
#define LOG_WARNING VT100_SGR_NORMAL VT100_SGR_FG_DEFAULT VT100_SGR_BG_DEFAULT VT100_SGR_BOLD LOG_TIME "[WARNING] "
#define LOG_ERROR VT100_SGR_NORMAL VT100_SGR_FG_DEFAULT VT100_SGR_BG_DEFAULT VT100_SGR_BOLD VT100_SGR_REVERSE LOG_TIME "[ERROR] "
#define LOG_FATAL VT100_SGR_NORMAL VT100_SGR_FG_DEFAULT VT100_SGR_BG_DEFAULT VT100_SGR_BG_RED VT100_SGR_BOLD VT100_SGR_FG_WHITE LOG_TIME "[FATAL] "	// aaaaaaaaaaaaaa

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
