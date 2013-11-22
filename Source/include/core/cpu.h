// BlacklightEVO video\cpu.h -- CPU identification and weirdness
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__CPU_H
#define __CORE__CPU_H

#include "global.h"

EXTERN unsigned __int64 __rdtsc();
#pragma intrinsic(__rdtsc)

struct regs_s {
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
};

struct cpuinfo {
	char vendor_string[13];
	char brand_string[48];

	double current_clock_rate;

	int family;									// 11:8
	int model;									// 7:4
	int stepping;								// 3:0
	int type;									// 13:12
	int ext_family;								// 27:20
	int ext_model;								// 19:16

	bool fpu;									// 0x00000001 : EDX
	bool vme;
	bool de;
	bool pse;
	bool tsc;
	bool msr;
	bool pae;
	bool mce;
	bool cx8;
	bool apic;
	bool reserved;
	bool sep;
	bool mtrr;
	bool pge;
	bool mca;
	bool cmov;
	bool pat;
	bool pse36;
	bool pn;
	bool clflush;
	bool reserved1;
	bool dts;
	bool acpi;
	bool mmx;
	bool fxr;
	bool sse;
	bool sse2;
	bool ss;
	bool ht;
	bool tm;
	bool ia64;
	bool pbe;

	bool sse3;									// 0x00000001 : ECX
	bool pclmulqdq;
	bool dtes64;
	bool monitor;
	bool ds_cpl;
	bool vmx;
	bool smx;
	bool est;
	bool tm2;
	bool ssse3;
	bool cid;
	bool reserved2;
	bool fma;
	bool cx16;
	bool xtpr;
	bool pdcm;
	bool reserved3;
	bool pcid;
	bool dca;
	bool sse4_1;
	bool sse4_2;
	bool x2apic;
	bool movbe;
	bool popcnt;
	bool tscdeadline;
	bool aes;
	bool xsave;
	bool osxsave;
	bool avx;
	bool f16c;
	bool rdrand;
	bool hypervisor;

	bool constant_tsc;							// Kinda like the Linux kernel flag
	bool invariant_tsc;							// Sort of like constant_tsc
};

extern struct cpuinfo cpu_info;					// Confusing? The contents of this shit is worse.

struct regs_s cpuid(unsigned int function);
void cpu_detect(struct cpuinfo* info);
void cpu_reset(void);

#endif	// __CORE__CPU_H