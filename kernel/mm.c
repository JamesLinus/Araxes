// BlacklightEVO kernel/mm.cpp -- memory management crap
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <terminal.h>
#include <gdt.h>
#include <mm.h>
//#include <hardware\uart.h>
#include <multiboot.h>

#define MM_DEBUG 0

static const size_t overhead = sizeof(size_t);
static const size_t align_to = 16;

void* mm_heap_end = (void*)((uint32_t)&kernel_end);
uint32_t mm_heap_cap = 0x01000000;

void* mm_start_of_memory =	(void*)0x00100000;	// PRETEND DAMMIT
void* mm_end_of_memory =	(void*)0x01000000;	// PRETEND DAMMIT

uint8_t mm_phys_mmap[4096];

void mm_create_mmap(multiboot_info_t* multiboot) {
	uint32_t usable_memory = 0;
	uint32_t usable_low_memory = 0;
	memory_map_t* mmap = (memory_map_t*)multiboot->mmap_addr;
	memset(mm_phys_mmap, 0, 4096);
	mm_phys_mmap[0] = 0xFF;
	
#if MM_DEBUG
	kprintf("\nWe have a multiboot memory map:\n");
	kprintf(" --------------------------------------------------\n");
	kprintf(" | Address            | Length             | Type |\n");
#endif
		
	while(mmap < (memory_map_t*)(multiboot->mmap_addr + multiboot->mmap_length)) {
		if (mmap->type) {
			if (!(mmap->base_addr_high) && mmap->base_addr_low >= 0x100000 && mmap->length_low < 0x100000) {
#if MM_DEBUG
				debug_printf(LOG_INFO "Adding to mm_phys_mmap:");
#endif
				mm_phys_mmap[(mmap->base_addr_low) >> 20] = (uint8_t)mmap->type & 0xFF;
#if MM_DEBUG
				_debug_printf(" m[%u]=%u\n",(mmap->base_addr_low) >> 20, mmap->type);
#endif
			}
			else if (!(mmap->base_addr_high) && mmap->base_addr_low >= 0x100000) {
				uint32_t mem_size = mmap->length_low & 0xFFF00000;
#if MM_DEBUG
				debug_printf(LOG_INFO "Adding to mm_phys_mmap:");
#endif
				while (mem_size) {
					mem_size -= 0x100000;
					mm_phys_mmap[(mmap->base_addr_low + mem_size) >> 20] = (uint8_t)mmap->type & 0xFF;
					//_debug_printf(" m[%u]=%u",(mmap->base_addr_low + mem_size) >> 20, mmap->type);
				}
#if MM_DEBUG
				_debug_printf("\n");
#endif
				if (mmap->type == 1)
					usable_memory += mmap->length_low;
			}
			else if (!(mmap->base_addr_high) && mmap->base_addr_low < 0x100000 && mmap->type == 1)
				usable_low_memory += mmap->length_low;
		}
#if MM_DEBUG
		kprintf(" | 0x%8X%8X | 0x%8X%8X | 0x%2X |\n", (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low, (uint32_t)mmap->length_high, (uint32_t)mmap->length_low, (uint32_t)mmap->type);
#endif
		
		mmap = (memory_map_t*) ( (uint32_t)mmap + mmap->size + sizeof(uint32_t) );
	}
#if MM_DEBUG
	kprintf(" --------------------------------------------------\n");
	kprintf(" Usable memory below 1 MiB: %u KiB (%u pages)\n", usable_low_memory / 1024, usable_low_memory / 4096);
	kprintf(" Usable memory above 1 MiB: %u KiB (%u pages)\n", usable_memory / 1024, usable_memory / 4096);
	kprintf(" Usable memory (total):     %u KiB (%u pages)\n\n", (usable_low_memory + usable_memory) / 1024, (usable_low_memory + usable_memory) / 4096);
	
	kprintf("CREATING PAGE TABLES\n");
#endif
	
	paging_kernel_directory = sbrk(sizeof(page_directory), true);
	memset(paging_kernel_directory, 0, sizeof(page_directory));
	paging_kernel_directory->phys_addr = (uint32_t)paging_kernel_directory->phys_tables;
	
#if MM_DEBUG
	kprintf("paging_kernel_directory = %p\n", paging_kernel_directory);
#endif
	
	for (uint32_t i = 0; i < 4096; i++) {
		if (mm_phys_mmap[i]) {
			if (!(paging_kernel_directory->tables[i >> 2])) {
				paging_kernel_directory->tables[i >> 2] = sbrk(sizeof(page_table), true);
				memset(paging_kernel_directory->tables[i >> 2], 0, sizeof(page_table));
				paging_kernel_directory->phys_tables[i >> 2] = ((uint32_t)paging_kernel_directory->tables[i >> 2] | 0x07);
			}
			for (uint32_t j = 256 * (i & 0x03); j < (256 * (i & 0x03)) + 256; j++) {
				paging_kernel_directory->tables[i >> 2]->pages[j].present = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].rw = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].user = 1;
				paging_kernel_directory->tables[i >> 2]->pages[j].frame = (uint32_t)(i * 256 + j%256);
			}
		}
	}
	
	// KILL MEEEEEEEEEEEEEEEE
	//   -this function

}

void* sbrk(size_t size, bool page_align) {
	// This is not actually a realistic sbrk implementation! What this does is simply an emulation to push the end-of-heap pointer along.
	if (page_align && ((uint32_t)mm_heap_end & 0x00000FFF))
		mm_heap_end = (void*)(((uint32_t)mm_heap_end & 0xFFFFF000) + 0x1000);
	
	if (((uint32_t)mm_heap_end + size) < mm_heap_cap) {
		mm_heap_end = (void*)((uint32_t)mm_heap_end + size);
		return (void*)((uint32_t)mm_heap_end - size);
	} else {
		if (mm_heap_cap * 2 > 0x04000000)
			_crash(/*__FILE__, __LINE__, "sbrk(%d) exceeded mm_heap_cap, mm_heap_cap * 2 > 64 MiB.\n", size*/);
		mm_heap_cap += mm_heap_cap;
		//debug_printf(UART_BASE_RS0, DEBUG_WARNING "sbrk(%d) exceeded mm_heap_cap, doubling kernel memory size to %d MiB\n", size, mm_heap_cap >> 20);
		return sbrk(size, page_align);
	}
}

void mm_dump_phys_mmap(void) {
	uint32_t start = 0;
	uint32_t type = 0;
	int i;
	for (i = 0; i < 4096; i++) {
		if (mm_phys_mmap[i] == type)
			continue;
		else {
			if (i) {
				kprintf("%8p - %8p  (type %lu)\n", (void*)(start*0x100000), (void*)((i-1)*0x100000+0xFFFFF), type);
			}
			type = mm_phys_mmap[i];
			start = i;
		}
	}	
	kprintf("%8p - %8p  (type %lu)\n", (void*)(start*0x100000), (void*)((i-1)*0x100000+0xFFFFF), type);
}

// A pretty strange but effective memory allocator by Kernighan and Ritchie. I'm not sure how this
// ancient code actually works. I probably should, but I'm afraid that if I try to figure out how
// it works, it'll stop working.

// UPDATE 2016-01-09: I need to figure out how this thing works, because it relies on "unsigned"
// meaning "unsigned int", which is in the C standard not a fixed width datatype. morecore()
// assigns a uint32_t value to "unsigned size", which could potentially have disastrous results.
// My current line of thought is "get rid of this mess and replace it with something sane", but
// the dumb hacker in me wants to see how long I can ride it out.

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

Header *morecore(uint32_t nu) {
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

// Not K&R, as you can probably guess.
void* calloc(size_t number, size_t size) {
	void* ptr = malloc(number * size);
	if (!ptr)
		return NULL;
	
	memset(ptr, 0, number * size);
	return ptr;
}
