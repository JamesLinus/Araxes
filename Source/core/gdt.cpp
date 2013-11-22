// BlacklightEVO core\gdt.cpp -- Global Descriptor Table
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\gdt.h"

gdt_entry gdt[GDT_ENTRIES];
gdtr_entry gdtr;

bool gdt_used[GDT_ENTRIES / 2] = {false};
unsigned short gdt_kernel_cs;

__declspec(naked) void gdt_initialize(void) {
	gdtr.base = (unsigned int)gdt;
	gdtr.limit = GDT_ENTRIES * 8;
	
	gdt_add_selector(0x00, 0, 0, 0, 0);			// Selector 0x00 is always unused according to Intel
	gdt_add_selector(0x08, 0, 0, 0, 0);			// For us, 0x08 is the same.
	gdt_used[0] = true;

	gdt_kernel_cs = gdt_add_task(0, 0xFFFFF, true);		// Should, under most circumstances, be 0x10

	__asm {
			lgdt [gdtr]

			mov ax, 0x10
			push ax
			push flush_cs
			retf

		flush_cs:
			add ax, 0x08
			mov ds, ax
			mov es, ax
			mov fs, ax
			mov gs, ax
			mov ss, ax

			sub esp, 2
			ret
	}
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