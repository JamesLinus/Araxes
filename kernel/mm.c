// BlacklightEVO kernel/mm.cpp -- memory management crap
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
//#include <core\console.h>
#include <mm.h>
//#include <hardware\uart.h>

static mm_free_block mm_free_block_list_head = { 0, 0 };
static const size_t overhead = sizeof(size_t);
static const size_t align_to = 16;

void* mm_heap_end = (void*)&kernel_end;
unsigned int mm_heap_cap = 0x01000000;

void* mm_end_of_memory = (void*)0x01000000;		// PRETEND DAMMIT

void mm_clear(void) {
	memset(mm_heap_end, 0, mm_heap_cap - (unsigned int)mm_heap_end);
}

void* sbrk(size_t size) {
	// This is not actually a realistic sbrk implementation! What this does is simply an emulation to push the end-of-heap pointer along.
	if (((unsigned int)mm_heap_end + size) < mm_heap_cap) {
		mm_heap_end = (void*)((unsigned int)mm_heap_end + size);
		return (void*)((unsigned int)mm_heap_end - size);
	} else {
		if (mm_heap_cap * 2 > 0x04000000)
			_crash(/*__FILE__, __LINE__, "sbrk(%d) exceeded mm_heap_cap, mm_heap_cap * 2 > 64 MiB.\n", size*/);
		mm_heap_cap += mm_heap_cap;
		//debug_printf(UART_BASE_RS0, DEBUG_WARNING "sbrk(%d) exceeded mm_heap_cap, doubling kernel memory size to %d MiB\n", size, mm_heap_cap >> 20);
		return sbrk(size);
	}
}

void* malloc(size_t size) {
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

void* calloc(size_t number, size_t size) {
	void* ptr = malloc(number * size);
	if (!ptr)
		return NULL;
	
	memset(ptr, 0, number * size);
	return ptr;
}

void free(void* ptr) {
	if (!ptr)
		return;
	mm_free_block* block = (mm_free_block*)(((char*)ptr) - sizeof(size_t));
	block->next = mm_free_block_list_head.next;
	mm_free_block_list_head.next = block;
}
