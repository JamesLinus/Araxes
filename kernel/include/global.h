// BlacklightEVO kernel/include/global.h -- important global includes and such
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
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

#define RMODE_CALL_VGA3 0x33414756
#define RMODE_CALL_DOWN 0x4E574F44
#define RMODE_CALL_VBE0 0x30454256
#define RMODE_CALL_VBE1 0x31454256
#define RMODE_CALL_VBE2 0x32454256
#define RMODE_CALL_EDID 0x44494445

#define RMGLOBAL_VIDEO_WIDTH	0x5010
#define RMGLOBAL_VIDEO_HEIGHT	0x5012
#define RMGLOBAL_VIDEO_DEPTH	0x5014
#define RMGLOBAL_VIDEO_MODE	0x5016
#define RMGLOBAL_VBE_BUFFER	0x5200

#define RMPTR(x) (void*)(((uint32_t)x & 0xFFFFU) + (((uint32_t)x & 0xFFFF0000U) >> 12U))

extern uint64_t timer_ms_ticks;

// Version defines and ints
// MOVED TO MAKEFILE

extern char kernel_version_string[];
void build_kernel_version_string(void);

struct regs {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
};

typedef void (*isr_t) (struct regs *);

static inline void _crash(void) {
	asm volatile ("cli; hlt");
}

static inline void io_wait(void) {
	asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

typedef struct {
	int second, minute, hour, day, month, year, weekday;
} datetime_t;

int64_t time_timestamp(datetime_t dt);

int64_t time_set(int64_t newtime);
int64_t time_get(void);
int time_get_weekday(void);

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);
uint32_t ind(uint16_t port);
void outd(uint16_t port, uint32_t data);

extern uint32_t rmode_call(uint32_t magic);

uint64_t cpu_rdtsc();
int memcmp(const void * s1, const void * s2,size_t n);
void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);

char* strcat(char *dest, const char *src);
char* strchr(const char *s, int c);
int strcmp(const char * s1, const char * s2);
int strncmp(const char * s1, const char * s2, size_t n);
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
char* uitoa(char* buf, uint32_t val, int base, int min);
char* i64toa(char* buf, int64_t val, int base, int min);
char* itoa(char* buf, int val, int base, int min);

/*void gdt_initialize(void);
void gdt_add_selector(int offset, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
uint16_t gdt_add_task(uint32_t base, uint32_t limit, bool kernel_mode);*/

void idt_initialize(void);
void idt_add_interrupt(int number, uint32_t base, uint16_t selector, uint8_t flags);

#endif	// __KERNEL__INCLUDE__GLOBAL_H
