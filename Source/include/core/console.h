// BlacklightEVO core\console.h -- Generic console driver
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__CONSOLE_H
#define __CORE__CONSOLE_H

#include "global.h"
#include <stdarg.h>
#include "hardware\uart.h"

#define CONSOLE_VGA 0x00
#define CONSOLE_BOCHS 0x01
#define CONSOLE_VBOX 0x02
#define CONSOLE_VBE 0x03

#define CONSOLE_KEYBOARD 0x00

#define PALETTE_EGA 0x00
#define PALETTE_XTERM 0x01
#define PALETTE_TRUECOLOUR 0x02

#define CONSOLE_IS_FRAMEBUFFER (console_mode == CONSOLE_BOCHS || console_mode == CONSOLE_VBOX || console_mode == CONSOLE_VBE)
#define CONSOLE_MODE_STRING (console_mode == CONSOLE_VGA ? "VGA text mode" : console_mode == CONSOLE_VBOX ? "VirtualBox" : console_mode == CONSOLE_BOCHS ? "BGA" : console_mode == CONSOLE_VBE ? "VBE" : "unknown (wtf?)")

extern int console_cursor_x, console_cursor_y, console_width, console_height;
extern unsigned char console_attrib;
extern int console_mode, console_palette;
extern unsigned int console_fg, console_bg;
extern int console_input;

void console_putchar(char c);
void console_print(char* s);
void console_clear(void);

int printf(const char* fmt, ...);
int vsprintf(char* buf, const char* fmt, va_list arg);
int debug_printf(unsigned short port, const char* fmt, ...);

void panic(char* file, int line, const char* s, ...);

#endif	// __CORE__CONSOLE_H