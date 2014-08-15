// BlacklightEVO kernel/include/mm.h -- header for the MM crap
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__MM_H
#define __KERNEL__INCLUDE__MM_H

typedef struct mm_free_block {
	size_t size;
	struct mm_free_block* next;
} mm_free_block;

extern uint32_t kernel_end;

void mm_clear(void);
void* sbrk(size_t size);
void* malloc(size_t size);
void* calloc(size_t number, size_t size);
void free(void* ptr);

#endif	// __KERNEL__INCLUDE__MM_H
