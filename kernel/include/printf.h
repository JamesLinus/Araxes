// BlacklightEVO kernel/include/printf.h -- kvsprintf and friends
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__PRINTF_H
#define __KERNEL__INCLUDE__PRINTF_H

#include <stdarg.h>

int kvsprintf(char* str, char* fmt, va_list va);
int kprintf(char *fmt, ...);
int ksprintf(char* s,char *fmt, ...);

#endif	// __KERNEL__INCLUDE__PRINTF_H



