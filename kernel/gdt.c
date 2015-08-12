// BlacklightEVO kernel/gdt.c -- x86 GDT/TSS/paging
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// For some reason some of the paging gunk got shoved in here too.
// That should be fixed at some point (either all paging moved here, all moved
// to mm.c, or all moved to a new file).

#include <global.h>
#include <gdt.h>
#include <mm.h>

gdt_entry gdt[GDT_ENTRIES];
gdtr_entry gdtr;

bool gdt_used[GDT_ENTRIES / 2] = {false};
unsigned short gdt_kernel_cs;
unsigned short gdt_user_cs;
unsigned short gdt_user_tss = 0x33;		// We're safe to use 0x33 here since gdt_add_selector handles the offset.

tss_entry tss;

page_directory* paging_kernel_directory;
page_directory* paging_current_directory;

unsigned int* paging_frames;
unsigned int paging_nframes;


void gdt_initialize(void) {
	gdtr.base = (unsigned int)gdt;
	gdtr.limit = GDT_ENTRIES * 8;
	
	gdt_add_selector(0x00, 0, 0, 0, 0);			// Selector 0x00 is always unused according to Intel
	gdt_add_selector(0x08, 0, 0, 0, 0);			// For us, 0x08 is the same.
	gdt_used[0] = true;

	gdt_kernel_cs = gdt_add_task(0, 0xFFFFF, true);		// Should, under most circumstances, be 0x10
	gdt_user_cs = gdt_add_task(0, 0xFFFFF, false);		// Should, under most circumstances, be 0x20
	
	gdt_add_selector(gdt_user_tss, (unsigned int)&tss, sizeof(tss)-1, 0xE9, 0x40);
	memset(&tss, 0, sizeof(tss));
	
	tss.ss0  = gdt_kernel_cs+0x08;		// Kernel SS
	tss.esp0 = 0;	// Kernel ESP
	tss.cs = gdt_kernel_cs | 0x3;
	tss.ss = tss.ds = tss.es = tss.fs = tss.gs = (gdt_kernel_cs+0x08) | 0x3;
	gdt_used[3] = true;		// Lock this down.

	gdt_reload();	// PRAY IT DON'T GO BOOM
	
	asm volatile ("ltr %%ax" : : "a"(gdt_user_tss));	// It likes to go boom here too.
}

void gdt_add_selector(int offset, unsigned int base, unsigned int limit, unsigned char access, unsigned char flags) {
	gdt[(offset/0x08)].limit_0_15 = (unsigned short)(limit);
	gdt[(offset/0x08)].base_0_15 = (unsigned short)(base & 0xFFFF);
	gdt[(offset/0x08)].base_16_23 = (unsigned char)((base >> 16) & 0xFF);
	gdt[(offset/0x08)].access = access;
	gdt[(offset/0x08)].flags = flags;
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

void* tss_get_esp0(void) {
	return (void*)tss.esp0;
}

void tss_set_esp0(void* new_esp0) {
	//assert((stack_pointer & 0xF) == 0);
	tss.esp0 = (unsigned int) new_esp0;
}

void paging_set_directory(page_directory* directory) {
	paging_current_directory = directory;
	asm volatile ("mov %0, %%cr3\n"
				  "mov %%cr0, %%eax\n"
				  "orl $0x80000000, %%eax\n"
				  "mov %%eax, %%cr0\n"
				  :: "r"(directory->phys_addr)
				  : "%eax");
}
