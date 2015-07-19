// BlacklightEVO kernel/gdt.c -- x86 GDT/TSS/paging
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// For some reason all the paging gunk got shoved in here too.

#include <global.h>
#include <gdt.h>
#include <mm.h>

gdt_entry gdt[GDT_ENTRIES];
gdtr_entry gdtr;

bool gdt_used[GDT_ENTRIES / 2] = {false};
unsigned short gdt_kernel_cs;

page_directory* paging_kernel_directory;
page_directory* paging_current_directory;

//page_table paging_kernel_tables[1024];

unsigned int* paging_frames;
unsigned int paging_nframes;


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
	//gdt[(offset/0x08)].flags =  (((limit >> 16) & 0x0F) | 0xF0);
	gdt[(offset/0x08)].flags =  flags;
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


// PAGING GUNK
// TODO - Rewrite all of this.
// XXX - The above todo was written when this code was implemented.

/*

static void paging_set_frame(unsigned int frame_addr) {
	unsigned int frame = frame_addr/0x1000;
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	paging_frames[idx] |= (0x1 << off);
}

static void paging_clear_frame(unsigned int frame_addr) {
	unsigned int frame = frame_addr/0x1000;
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	paging_frames[idx] &= ~(0x1 << off);
}

static unsigned int paging_test_frame(unsigned int frame_addr) {
	unsigned int frame = frame_addr/0x1000;
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	return (paging_frames[idx] & (0x1 << off));
}

static unsigned int paging_first_frame(void) {
	unsigned int i, j;
	for (i = 0; i < INDEX_FROM_BIT(paging_nframes); i++) {
		if (paging_frames[i] != 0xFFFFFFFF) {
			// at least one bit is free here.
			for (j = 0; j < 32; j++) {
				unsigned int toTest = 0x1 << j;
				if ( !(paging_frames[i]&toTest) )
					return i*4*8+j;
			}
		}
	}
	
	kprintf("\n\nFUCK: Ran out of memory in first_frame().");
	_crash();
	
	return -1;	// There's no way in hell we should get here.
}

void paging_alloc_frame(page_entry *page, int is_kernel, int is_writeable) {
	if (page->frame != 0) {
		return;
	}
	else {
		unsigned int idx = paging_first_frame();
		if (idx == (unsigned int)-1) {
			kprintf("\n\n"
					"FUCK: Ran out of memory in alloc_frame().\n"
					"      YOU SHOULD NOT SEE THIS. TELL A DEVELOPER.\n");
			_crash();
		}
		paging_set_frame(idx*0x1000);
		page->present = 1;
		page->rw = is_writeable ? 1 : 0;
		page->user = is_kernel ? 0 : 1;
		page->frame = idx;
	}
}

void paging_free_frame(page_entry *page) {
	unsigned int frame;
	if (!(frame=page->frame))
		return;
	else {
		paging_clear_frame(frame);
		page->frame = 0x0;
	}
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

page_entry *paging_get_page(unsigned int address, int make, page_directory *dir) {
    // Turn the address into an index.
    address /= 0x1000;
    // Find the page table containing this address.
    unsigned int table_idx = address / 1024;
    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else if(make)
    {
        unsigned int tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
    {
        return 0;
    }
}

void paging_initialize(unsigned int usable_mem_end) {
	//unsigned int usable_mem_end = 0x1000000;
	paging_nframes = usable_mem_end / 4096;
	paging_frames = (unsigned int*) malloc(INDEX_FROM_BIT(paging_nframes));
	memset(paging_frames, 0, INDEX_FROM_BIT(paging_nframes));
	
	paging_kernel_directory = (page_directory*)mm_kernel_end_palign;
	paging_current_directory = paging_kernel_directory;
	int i = 0;
	while (i < usable_mem_end) {
		paging_alloc_frame(paging_get_page(i, 1, paging_kernel_directory), 0, 0);
		i += 0x1000;
	}
	
	paging_set_directory(paging_kernel_directory);
}

*/

void paging_set_directory(page_directory* directory) {
	paging_current_directory = directory;
	asm volatile ("mov %0, %%cr3\n"
				  "mov %%cr0, %%eax\n"
				  "orl $0x80000000, %%eax\n"
				  "mov %%eax, %%cr0\n"
				  :: "r"(directory->phys_addr)
				  : "%eax");
}
