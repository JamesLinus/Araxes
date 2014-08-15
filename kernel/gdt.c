// BlacklightEVO kernel/gdt.c -- x86 GDT/TSS
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>

#define GDT_ENTRIES 128

#if (GDT_ENTRIES % 2 != 0) || (GDT_ENTRIES < 6)
	#error "GDT_ENTRIES is invalid!"
#endif

typedef struct {
	unsigned short limit_0_15;
	unsigned short base_0_15;
	unsigned char base_16_23;
	unsigned char access;
	unsigned char flags;
	unsigned char base_24_31;
} __attribute__((packed)) gdt_entry;

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) gdtr_entry;

gdt_entry gdt[GDT_ENTRIES];
gdtr_entry gdtr;

bool gdt_used[GDT_ENTRIES / 2] = {false};
unsigned short gdt_kernel_cs;

extern void gdt_reload(void);	// from kernel/entry.asm

void gdt_initialize(void) {
	gdtr.base = (unsigned int)gdt;
	gdtr.limit = GDT_ENTRIES * 8;
	
	gdt_add_selector(0x00, 0, 0, 0, 0);			// Selector 0x00 is always unused according to Intel
	gdt_add_selector(0x08, 0, 0, 0, 0);			// For us, 0x08 is the same.
	gdt_used[0] = true;

	gdt_kernel_cs = gdt_add_task(0, 0xFFFFF, true);		// Should, under most circumstances, be 0x10

	gdt_reload();	// PRAY IT DON'T GO BOOM
}

void gdt_add_selector(int offset, unsigned int base, unsigned int limit, unsigned char access, unsigned char flags) {
	gdt[(offset/0x08)].limit_0_15 = (unsigned short)(limit);
	gdt[(offset/0x08)].base_0_15 = (unsigned short)(base & 0xFFFF);
	gdt[(offset/0x08)].base_16_23 = (unsigned char)((base >> 16) & 0xFF);
	gdt[(offset/0x08)].access = access;
	gdt[(offset/0x08)].flags =  (((limit >> 16) & 0x0F) | 0xF0);
	gdt[(offset/0x08)].base_24_31 = (unsigned char)((base >> 24) & 0xFF);
}

unsigned short gdt_add_task(unsigned int base, unsigned int limit, bool kernel_mode) {
	unsigned short ret = 0;
	for (int i = 0; i < GDT_ENTRIES / 2; i++) {
		if (!gdt_used[i]) {
			gdt_used[i] = true;
			if (kernel_mode) {
				gdt_add_selector(i * 0x10, base, limit & 0xFFFFF, 0x9A, 0xCF);
				gdt_add_selector(i * 0x10 + 0x08, base, limit & 0xFFFFF, 0x92, 0xCF);
			} else {
				gdt_add_selector(i * 0x10, base, limit & 0xFFFFF, 0xFA, 0xCF);
				gdt_add_selector(i * 0x10 + 0x08, base, limit & 0xFFFFF, 0xF2, 0xCF);
			}
			ret = i * 0x10;
			break;
		}
	}
	
	return ret;
}
