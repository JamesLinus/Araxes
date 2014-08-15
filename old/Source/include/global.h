// BlacklightEVO include\global.h -- global functions and macros that are useful
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "video\vt100.h"

#define EXTERN		extern "C"
#define EXPORT		EXTERN __declspec(dllexport)	// Declares DLL export - rarely used
#define IMPORT		EXTERN __declspec(dllimport)	// Declares DLL import - rarely used

EXTERN int _fltused;
EXTERN long _ftol2_sse();
#define NULL	(void*)0

EXTERN __int64 __stdcall __allmul(__int64 multiplicand, __int64 multiplier);
//#pragma intrinsic(__allmul)

#define FALSE	0								// Because preprocessor
#define TRUE	1

#define BIT(x, pos) ((x) & (1<<(pos)))

#define DEBUG_INFO VT100_SGR_NORMAL "[INFO] "
#define DEBUG_WARNING VT100_SGR_BOLD "[WARNING] "
#define DEBUG_ERROR VT100_SGR_BOLD VT100_SGR_REVERSE "[ERROR] "
#define DEBUG_FATAL VT100_SGR_BOLD VT100_SGR_REVERSE "[FATAL] "

struct regs {
	unsigned int ds, es, fs, gs;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int eip, cs, eflags, ss;
};

#pragma region Port I/O
EXTERN int _outp(unsigned short, int);
EXTERN unsigned short _outpw(unsigned short, unsigned long);
EXTERN unsigned long _outpd(unsigned short, unsigned int);
EXTERN int _inp(unsigned short);
EXTERN unsigned short _inpw(unsigned short);
EXTERN unsigned long _inpd(unsigned short);
EXTERN void *_ReturnAddress();

#define outb(prt, val)	_outp(prt, val)
#define inb(prt)		((unsigned char)_inp(prt))
#define outw(prt, val)	_outpw(prt, val)
#define inw(prt)		((unsigned short)_inpw(prt))
#define outd(prt, val)	_outpd(prt, val)
#define ind(prt)		((unsigned long)_inpd(prt))
#define getRetAddr		_ReturnAddress

#pragma intrinsic(_ReturnAddress, _outp, _inp, _outpw, _inpw, _outpd, _inpd)
#pragma endregion

//EXTERN long long __alldiv(long long, long long);
//EXTERN unsigned long long __aullrem(unsigned long long, unsigned long long);
//EXTERN unsigned long long __aulldiv(unsigned long long, unsigned long long);
//#pragma intrinsic(__alldiv, __aullrem, __aulldiv)


#pragma region Constructors

#pragma endregion

unsigned long long cpu_rdtsc(void);

int _cdecl memcmp(const void * s1, const void * s2,size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);

char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);
int strcmp(const char * s1, const char * s2);
char *strcpy(char *dest, const char *src);
size_t strcspn(const char *s1, const char *s2);
char* strdup(const char* str);
size_t strlen(const char *s);
char *strncpy(char *dest, const char *src, size_t n);
size_t strspn(const char *s1, const char *s2);
char *strstr(char *s1, const char *s2);
char *strtok(char *str, const char *delim);
char * strtok_r(char * str, const char * delim, char ** saveptr);

void simd_memcpy_mmx(void* dest, void* src, size_t num);

int atoi(const char * str);

char* itoa(int val, int base);
char* itoa(int val, int base, int min);
char* itoa(char* buf, int val, int base);
char* itoa(char* buf, int val, int base, int min);

char* lltoa(long long val, int base, int min);
char* lltoa(char* buf, long long val, int base, int min);

char *dtoa(double num);
char *dtoa(double num, int precision);
char *dtoa(char* buf, double num);
char *dtoa(char* buf, double num, int precision);

#endif	//__GLOBAL_H