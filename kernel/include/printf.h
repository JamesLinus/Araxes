// BlacklightEVO kernel/include/printf.h -- kvsprintf and friends
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__PRINTF_H
#define __KERNEL__INCLUDE__PRINTF_H

#include <stdarg.h>
#include <terminal.h>
#include <global.h>

#define debug_printf(fmt, ...) _debug_printf(fmt, cpu_rdtsc()/1000000, ##__VA_ARGS__)

int kvsnprintf(char* str, size_t size, const char* fmt, va_list va);
int kprintf(const char *fmt, ...)
	__attribute__ ((format (printf, 1, 2)));
int ksnprintf(char* s, size_t size, const char *fmt, ...)
	__attribute__ ((format (printf, 3, 4)));
void crash(char* file, int line, const char* fmt, ...)
	__attribute__ ((format (printf, 3, 4)));
int _debug_printf(const char* fmt, ...);

#endif	// __KERNEL__INCLUDE__PRINTF_H



