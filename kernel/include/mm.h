// BlacklightEVO kernel/include/mm.h -- header for the MM crap
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__MM_H
#define __KERNEL__INCLUDE__MM_H
#include <multiboot.h>

typedef struct mm_free_block {
	size_t size;
	struct mm_free_block* next;
} mm_free_block;

typedef struct {
	uint32_t size;
	uint32_t base_addr_low;
	uint32_t base_addr_high;
	uint32_t length_low;
	uint32_t length_high;
	uint32_t type;
} memory_map_t;

extern void* mm_kernel_end_palign;
extern void* mm_heap_end;
extern uint32_t kernel_end;
extern uint32_t mm_heap_cap;

void mm_clear(void);
void mm_create_mmap(multiboot_info_t* multiboot);
void mm_dump_phys_mmap(void);

void* sbrk(size_t size, bool page_align);
void* malloc(size_t size);
void* calloc(size_t number, size_t size);
void free(void* ptr);

#endif	// __KERNEL__INCLUDE__MM_H
