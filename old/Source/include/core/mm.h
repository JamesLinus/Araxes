// BlacklightEVO core\mm.h -- memory management crap
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__MM_H
#define __CORE__MM_H

#include "global.h"
#include "kernel.h"

extern void* mm_heap_end;
extern unsigned int mm_heap_cap;

void mm_clear(void);
void* sbrk(size_t size);
void* malloc(size_t size);
void* calloc(size_t number, size_t size);
void free(void* ptr);

#endif __CORE__MM_H