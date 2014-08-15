// BlacklightEVO core\gdt.h -- Global Descriptor Table
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__GDT_H
#define __CORE__GDT_H

#define GDT_ENTRIES 128

#if (GDT_ENTRIES % 2 != 0) || (GDT_ENTRIES < 4)
	#error "GDT_ENTRIES is invalid!"
#endif

typedef struct {
	unsigned short limit_0_15;
	unsigned short base_0_15;
	unsigned char base_16_23;
	unsigned char access;
	unsigned char flags;
	unsigned char base_24_31;
} gdt_entry;

typedef struct {
    unsigned short limit;
    unsigned int base;
} gdtr_entry;

extern unsigned short gdt_kernel_cs;

void gdt_initialize(void);
void gdt_add_selector(int offset, unsigned int base, unsigned int limit, unsigned char access, unsigned char flags);
unsigned short gdt_add_task(unsigned int base, unsigned int limit, bool kernel_mode);

#endif	// __CORE__GDT_H