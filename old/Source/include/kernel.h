// BlacklightEVO include\kernel.h -- some extra kernel related shit
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL_H
#define __KERNEL_H

#define KERNEL_VERSION_MAJOR 0
#define KERNEL_VERSION_MINOR 0
#define KERNEL_VERSION_WEEK 0x1331				// Upper byte is year, lower byte is week
#define KERNEL_VERSION_NAME "EVOlution"

#define dd(x) \
	__asm _emit (x)       & 0xFF \
	__asm _emit (x) >> 8  & 0xFF \
	__asm _emit (x) >> 16 & 0xFF \
	__asm _emit (x) >> 24 & 0xFF

#define KERNEL_STACK 0x00200000

extern char kernel_version_string[128];

EXPORT void kmain(unsigned long magic, unsigned long multiboot);

#endif	// __KERNEL_H