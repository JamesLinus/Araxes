// BlacklightEVO kernel/mm.cpp -- memory management crap
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <terminal.h>
#include <gdt.h>
#include <mm.h>
//#include <hardware\uart.h>
#include <multiboot.h>

//static mm_free_block mm_free_block_list_head = { 0, 0 };
static const size_t overhead = sizeof(size_t);
static const size_t align_to = 16;

//void* mm_kernel_end_palign = (void*)(((unsigned int)&kernel_end & 0xFFFFF000) + 0x1000);
void* mm_heap_end = (void*)((unsigned int)&kernel_end/* + 0x100000*/);
unsigned int mm_heap_cap = 0x01000000;

void* mm_start_of_memory =	(void*)0x00100000;		// PRETEND DAMMIT
void* mm_end_of_memory =	(void*)0x01000000;		// PRETEND DAMMIT

unsigned char mm_phys_mmap[4096];

void mm_clear(void) {
	// THIS IS THE KICKIN RAD-EST WAY TO CRASH THE SYSTEM BY FAR
	memset(mm_heap_end, 0, mm_heap_cap - (unsigned int)mm_heap_end);
}

void mm_create_mmap(multiboot_info_t* multiboot) {
	unsigned int usable_memory = 0;
	unsigned int usable_low_memory = 0;
	memory_map_t* mmap = (memory_map_t*)multiboot->mmap_addr;
	memset(mm_phys_mmap, 0, 4096);
	mm_phys_mmap[0] = 0xFF;
	
	kprintf("\nWe have a multiboot memory map:\n");
	kprintf(" --------------------------------------------------\n");
	kprintf(" | Address            | Length             | Type |\n");
		
	while(mmap < (memory_map_t*)(multiboot->mmap_addr + multiboot->mmap_length)) {
		if (mmap->type) {
			if (!(mmap->base_addr_high) && mmap->base_addr_low >= 0x100000 && mmap->length_low < 0x100000) {
				debug_printf(LOG_INFO "Adding to mm_phys_mmap:");
				mm_phys_mmap[(mmap->base_addr_low) >> 20] = (unsigned char)mmap->type & 0xFF;
				debug_printf(" m[%u]=%u\n",(mmap->base_addr_low) >> 20, mmap->type);
			}
			else if (!(mmap->base_addr_high) && mmap->base_addr_low >= 0x100000) {
				unsigned int mem_size = mmap->length_low & 0xFFF00000;
				debug_printf(LOG_INFO "Adding to mm_phys_mmap:");
				while (mem_size) {
					mem_size -= 0x100000;
					mm_phys_mmap[(mmap->base_addr_low + mem_size) >> 20] = (unsigned char)mmap->type & 0xFF;
					debug_printf(" m[%u]=%u",(mmap->base_addr_low + mem_size) >> 20, mmap->type);
				}
				debug_printf("\n");
				if (mmap->type == 1)
					usable_memory += mmap->length_low;
			}
			else if (!(mmap->base_addr_high) && mmap->base_addr_low < 0x100000 && mmap->type == 1)
				usable_low_memory += mmap->length_low;
		}
		kprintf(" | 0x%8X%8X | 0x%8X%8X | 0x%2X |\n", mmap->base_addr_high, mmap->base_addr_low, mmap->length_high, mmap->length_low, mmap->type);
		
		mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
	}
	kprintf(" --------------------------------------------------\n");
	kprintf(" Usable memory below 1 MiB: %u KiB (%u pages)\n", usable_low_memory / 1024, usable_low_memory / 4096);
	kprintf(" Usable memory above 1 MiB: %u KiB (%u pages)\n", usable_memory / 1024, usable_memory / 4096);
	kprintf(" Usable memory (total):     %u KiB (%u pages)\n\n", (usable_low_memory + usable_memory) / 1024, (usable_low_memory + usable_memory) / 4096);
	
	kprintf("CREATING PAGE TABLES\n");
	
	paging_kernel_directory = sbrk(sizeof(page_directory), true);
	memset(paging_kernel_directory, 0, sizeof(page_directory));
	paging_kernel_directory->phys_addr = (unsigned int)paging_kernel_directory->phys_tables;
	
	kprintf("paging_kernel_directory = %#8X\n", paging_kernel_directory);
	/*for (unsigned int i = 0; i < 4096; i++) {
		if (mm_phys_mmap[i]) {
			if (!(paging_kernel_directory->tables[i >> 2])) {
				paging_kernel_directory->tables[i >> 2] = sbrk(sizeof(page_table), true);
				memset(paging_kernel_directory->tables[i >> 2], 0, sizeof(page_table));
				paging_kernel_directory->tables[i >> 2] = ((unsigned int)paging_kernel_directory->tables[i >> 2] | 0x07);
				kprintf("created page table %d ", i >> 2);
			}
			kprintf("%d=[%d][%d] - ", i, i>>2, 256 * (i & 0x03));
			for (unsigned int j = 256 * (i & 0x03); j < (256 * (i & 0x03)) + 256; j++) {
				paging_kernel_directory->tables[i >> 2]->pages[j].present = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].rw = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].user = 1;
				//paging_kernel_directory->tables[i >> 2]->pages[j].frame = (unsigned int)(((i >> 2 << 20) + (j << 12)) >> 12);
				paging_kernel_directory->tables[i >> 2]->pages[j].frame = (unsigned int)((((unsigned int)i << 20) + ((unsigned int)j << 12)) >> 12);
			}
		}
	}*/
	
	
	for (unsigned int i = 0; i < 4096; i++) {
		if (mm_phys_mmap[i]) {
			if (!(paging_kernel_directory->tables[i >> 2])) {
				paging_kernel_directory->tables[i >> 2] = sbrk(sizeof(page_table), true);
				memset(paging_kernel_directory->tables[i >> 2], 0, sizeof(page_table));
				paging_kernel_directory->phys_tables[i >> 2] = ((unsigned int)paging_kernel_directory->tables[i >> 2] | 0x07);
				//kprintf("created page table %d %#8X", i >> 2, paging_kernel_directory->tables[i >> 2]);
			}
			//kprintf("%d=[%d][%d] - ", i, i>>2, 256 * (i & 0x03));
			for (unsigned int j = 256 * (i & 0x03); j < (256 * (i & 0x03)) + 256; j++) {
				//if (i==3)
				//	kprintf("%u ", j);
				//if (i == 0 && j == 0)
				//	kprintf(" doing %u->%u: %#8X->%#8X ", i >> 2, j, &paging_kernel_directory->tables[i >> 2]->pages[j], *(unsigned int*)(&paging_kernel_directory->tables[i >> 2]->pages[j]));
				paging_kernel_directory->tables[i >> 2]->pages[j].present = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].rw = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].user = 1;
				//paging_kernel_directory->tables[i >> 2]->pages[j].frame = (unsigned int)(((i >> 2 << 20) + (j << 12)) >> 12);
				//paging_kernel_directory->tables[i >> 2]->pages[j].frame = (unsigned int)((((unsigned int)i << 20) + ((unsigned int)(j%256) << 12)) >> 12);
				paging_kernel_directory->tables[i >> 2]->pages[j].frame = (unsigned int)(i * 256 + j%256);
				//if (i == 3 && j == 1022)
				//	kprintf(" doing %u->%u: %#8X->%#8X ", i >> 2, j, &paging_kernel_directory->tables[i >> 2]->pages[j], *(unsigned int*)(&paging_kernel_directory->tables[i >> 2]->pages[j]));
				//if (i == 3 && j == 1022)
				//	kprintf(" doing %u->%u: %#8X->%#8X ", i >> 2, j+1, &paging_kernel_directory->tables[i >> 2]->pages[j+1], *(unsigned int*)(&paging_kernel_directory->tables[i >> 2]->pages[j+1]));
			}
		}
	}
	
	// KILL MEEEEEEEEEEEEEEEE
	//   -this function
	
	/*kprintf("\n");
	//kprintf("%#8X\n", (unsigned int)((4 >> 2 << 20) + (0 << 12) >> 12) << 12);
	kprintf("paging_kernel_directory->tables[0]->pages[0]    = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[0]));
	kprintf("paging_kernel_directory->tables[0]->pages[1]    = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[1]));
	kprintf("paging_kernel_directory->tables[0]->pages[255]  = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[255]));
	kprintf("paging_kernel_directory->tables[0]->pages[256]  = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[256]));
	kprintf("paging_kernel_directory->tables[0]->pages[512]  = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[512]));
	kprintf("paging_kernel_directory->tables[0]->pages[768]  = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[768]));
	kprintf("paging_kernel_directory->tables[0]->pages[1020] = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[1020]));
	kprintf("paging_kernel_directory->tables[0]->pages[1021] = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[1021]));
	kprintf("paging_kernel_directory->tables[0]->pages[1022] = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[1022]));
	kprintf("paging_kernel_directory->tables[0]->pages[1023] = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[0]->pages[1023]));
	kprintf("paging_kernel_directory->tables[1]->pages[0]    = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[1]->pages[0]));
	kprintf("paging_kernel_directory->tables[1]->pages[256]  = %#8X\n", *(unsigned int*)(&paging_kernel_directory->tables[1]->pages[256]));*/

}

void* sbrk(size_t size, bool page_align) {
	// This is not actually a realistic sbrk implementation! What this does is simply an emulation to push the end-of-heap pointer along.
	if (page_align && ((unsigned int)mm_heap_end & 0x00000FFF))
		mm_heap_end = (void*)(((unsigned int)mm_heap_end & 0xFFFFF000) + 0x1000);
	
	if (((unsigned int)mm_heap_end + size) < mm_heap_cap) {
		mm_heap_end = (void*)((unsigned int)mm_heap_end + size);
		return (void*)((unsigned int)mm_heap_end - size);
	} else {
		if (mm_heap_cap * 2 > 0x04000000)
			_crash(/*__FILE__, __LINE__, "sbrk(%d) exceeded mm_heap_cap, mm_heap_cap * 2 > 64 MiB.\n", size*/);
		mm_heap_cap += mm_heap_cap;
		//debug_printf(UART_BASE_RS0, DEBUG_WARNING "sbrk(%d) exceeded mm_heap_cap, doubling kernel memory size to %d MiB\n", size, mm_heap_cap >> 20);
		return sbrk(size, page_align);
	}
}

/*void* malloc(size_t size) {
	size = (size + sizeof(size_t) + (align_to - 1)) & ~ (align_to - 1);
	mm_free_block* block = mm_free_block_list_head.next;
	mm_free_block** head = &(mm_free_block_list_head.next);
	while (block != 0) {
		if (block->size >= size) {
			*head = block->next;
			return ((char*)block) + sizeof(size_t);
		}
		head = &(block->next);
		block = block->next;
	}

	block = (mm_free_block*)sbrk(size);
	block->size = size;

	return ((char*)block) + sizeof(size_t);
}

void free(void* ptr) {
	if (!ptr)
		return;
	mm_free_block* block = (mm_free_block*)(((char*)ptr) - sizeof(size_t));
	block->next = mm_free_block_list_head.next;
	mm_free_block_list_head.next = block;
}*/


typedef long Align;

union header {
	struct {
		union header *ptr;
		unsigned size;
	} s;
	Align x;
};

typedef union header Header;

static Header base;
static Header *freep = NULL;

#define NALLOC 	1024

Header *morecore(unsigned int nu) {
	char *cp;
	void free(void*);
	Header *up;
	
	if (nu < NALLOC)
		nu = NALLOC;
	
	cp = (char*)sbrk(nu * sizeof(Header), false);	
	
	if (cp == (char *) -1)
		return NULL;
		
	up = (Header*) cp;
	up->s.size = nu;
	free((void*)(up+1));
	return freep;
}

void* malloc(size_t size) {
	Header *p, *prevp;
	unsigned nunits;

	nunits = (size+sizeof(Header)-1)/sizeof(Header) + 1;

	if ((prevp = freep) == NULL) {
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}
	for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {

	if (p->s.size >= nunits) {
		if (p->s.size == nunits)
			prevp->s.ptr = p->s.ptr;
		else {
			p->s.size -= nunits;
			p += p->s.size;
			p->s.size = nunits;
		}
		freep = prevp;
		return (void*) (p+1);
	}
	if ( p == freep)
		if ((p = morecore(nunits)) == NULL)
			return NULL;
	}
}

void free(void *ap) {
	Header *bp, *p;
	
	bp = (Header*) ap - 1;

	for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
	if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
		break;

	if (bp + bp->s.size == p->s.ptr) {
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else
		bp->s.ptr = p->s.ptr;
		
	if ( p + p->s.size == bp ) {
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else
		p->s.ptr = bp;
		
	freep = p;
}


			
		
			 


void* calloc(size_t number, size_t size) {
	void* ptr = malloc(number * size);
	if (!ptr)
		return NULL;
	
	memset(ptr, 0, number * size);
	return ptr;
}
