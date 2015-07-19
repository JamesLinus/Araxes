// BlacklightEVO kernel/include/printf.h -- kvsprintf and friends
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__PRINTF_H
#define __KERNEL__INCLUDE__PRINTF_H

#include <stdarg.h>
#include <terminal.h>

int kvsprintf(char* str, const char* fmt, va_list va);
int kprintf(const char *fmt, ...);
int ksprintf(char* s, const char *fmt, ...);
void crash(char* file, int line, const char* fmt, ...);
int debug_printf(const char* fmt, ...);

#endif	// __KERNEL__INCLUDE__PRINTF_H



