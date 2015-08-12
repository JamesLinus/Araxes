// BlacklightEVO kernel/include/global.h -- important global includes and such
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__GLOBAL_H
#define __KERNEL__INCLUDE__GLOBAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <mm.h>
#include <printf.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


// Version defines and ints
// MOVED TO MAKEFILE

extern char kernel_version_string[];
void build_kernel_version_string(void);

struct regs {
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

typedef void (*isr_t) (struct regs *);

inline void _crash(void) {
	asm volatile ("cli; hlt");
}

inline void io_wait(void) {
	asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short data);
unsigned int ind(unsigned short port);
void outd(unsigned short port, unsigned int data);

uint64_t cpu_rdtsc();
int memcmp(const void * s1, const void * s2,size_t n);
void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);

char* strcat(char *dest, const char *src);
char* strchr(const char *s, int c);
int strcmp(const char * s1, const char * s2);
char* strcpy(char *dest, const char *src);
size_t strcspn(const char *s1, const char *s2);
char* strdup(const char* str);
size_t strlen(const char *s);
char* strncpy(char *dest, const char *src, size_t n);
size_t strspn(const char *s1, const char *s2);
char* strstr(char *s1, const char *s2);
char* strtok(char *str, const char *delim);
char* strtok_r(char * str, const char * delim, char ** saveptr);

int atoi(const char * str);
char* u64toa(char* buf, uint64_t val, int base, int min);
char* uitoa(char* buf, unsigned int val, int base, int min);
char* itoa(char* buf, int val, int base, int min);

/*void gdt_initialize(void);
void gdt_add_selector(int offset, unsigned int base, unsigned int limit, unsigned char access, unsigned char flags);
unsigned short gdt_add_task(unsigned int base, unsigned int limit, bool kernel_mode);*/

void idt_initialize(void);
void idt_add_interrupt(int number, unsigned int base, unsigned short selector, unsigned char flags);

#endif	// __KERNEL__INCLUDE__GLOBAL_H
