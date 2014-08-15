// BlacklightEVO video\cpu.cpp -- CPU identification and weirdness
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\console.h"
#include "core\cpu.h"
#include "hardware\uart.h"

struct cpuinfo cpu_info;					// Confusing? The contents of this shit is worse.

struct regs_s cpuid(unsigned int function) {
	static struct regs_s r;
	__asm {
		mov eax, [function]
		cpuid
		mov [r.eax], eax
		mov [r.ebx], ebx
		mov [r.ecx], ecx
		mov [r.edx], edx
	}
	return r;
}

void cpu_detect(struct cpuinfo* info) {
	struct regs_s regs;
	unsigned int maxcall;
	unsigned int* uintptr;
	char* charptr;
	debug_printf(UART_BASE_RS0, DEBUG_INFO "Setting up a cpuinfo struct, this might be painful.\n");

	info->current_clock_rate = 0.0;				// We don't know what it is yet; the PIT interrupt guesses for us.

	regs = cpuid(0x00000000);					// Grab the vendor string.
	uintptr = (unsigned int*)info->vendor_string;
	uintptr[0] = regs.ebx;
	uintptr[1] = regs.edx;
	uintptr[2] = regs.ecx;
	info->vendor_string[12] = 0;

	regs = cpuid(0x00000001);					// Grab the features and such.

	info->stepping = regs.eax & 0x0F;
	info->model = (regs.eax >> 4) & 0x0F;
	info->family = (regs.eax >> 8) & 0x0F;
	info->type = (regs.eax >> 12) & 0x03;
	info->ext_model = (regs.eax >> 16) & 0x0F;
	info->ext_family = (regs.eax >> 20) & 0xFF;

	info->fpu = BIT(regs.edx, 0) ? true : false;
	info->vme = BIT(regs.edx, 1) ? true : false;
	info->de = BIT(regs.edx, 2) ? true : false;
	info->pse = BIT(regs.edx, 3) ? true : false;
	info->tsc = BIT(regs.edx, 4) ? true : false;
	info->msr = BIT(regs.edx, 5) ? true : false;
	info->pae = BIT(regs.edx, 6) ? true : false;
	info->mce = BIT(regs.edx, 7) ? true : false;
	info->cx8 = BIT(regs.edx, 8) ? true : false;
	info->apic = BIT(regs.edx, 9) ? true : false;
	info->reserved = BIT(regs.edx, 10) ? true : false;
	info->sep = BIT(regs.edx, 11) ? true : false;
	info->mtrr = BIT(regs.edx, 12) ? true : false;
	info->pge = BIT(regs.edx, 13) ? true : false;
	info->mca = BIT(regs.edx, 14) ? true : false;
	info->cmov = BIT(regs.edx, 15) ? true : false;
	info->pat = BIT(regs.edx, 16) ? true : false;
	info->pse36 = BIT(regs.edx, 17) ? true : false;
	info->pn = BIT(regs.edx, 18) ? true : false;
	info->clflush = BIT(regs.edx, 19) ? true : false;
	info->reserved1 = BIT(regs.edx, 20) ? true : false;
	info->dts = BIT(regs.edx, 21) ? true : false;
	info->acpi = BIT(regs.edx, 22) ? true : false;
	info->mmx = BIT(regs.edx, 23) ? true : false;
	info->fxr = BIT(regs.edx, 24) ? true : false;
	info->sse = BIT(regs.edx, 25) ? true : false;
	info->sse2 = BIT(regs.edx, 26) ? true : false;
	info->ss = BIT(regs.edx, 27) ? true : false;
	info->ht = BIT(regs.edx, 28) ? true : false;
	info->tm = BIT(regs.edx, 29) ? true : false;
	info->ia64 = BIT(regs.edx, 30) ? true : false;
	info->pbe = BIT(regs.edx, 31) ? true : false;

	info->sse3 = BIT(regs.ecx, 0) ? true : false;
	info->pclmulqdq = BIT(regs.ecx, 1) ? true : false;
	info->dtes64 = BIT(regs.ecx, 2) ? true : false;
	info->monitor = BIT(regs.ecx, 3) ? true : false;
	info->ds_cpl = BIT(regs.ecx, 4) ? true : false;
	info->vmx = BIT(regs.ecx, 5) ? true : false;
	info->smx = BIT(regs.ecx, 6) ? true : false;
	info->est = BIT(regs.ecx, 7) ? true : false;
	info->tm2 = BIT(regs.ecx, 8) ? true : false;
	info->ssse3 = BIT(regs.ecx, 9) ? true : false;
	info->cid = BIT(regs.ecx, 10) ? true : false;
	info->reserved2 = BIT(regs.ecx, 11) ? true : false;
	info->fma = BIT(regs.ecx, 12) ? true : false;
	info->cx16 = BIT(regs.ecx, 13) ? true : false;
	info->xtpr = BIT(regs.ecx, 14) ? true : false;
	info->pdcm = BIT(regs.ecx, 15) ? true : false;
	info->reserved3 = BIT(regs.ecx, 16) ? true : false;
	info->pcid = BIT(regs.ecx, 17) ? true : false;
	info->dca = BIT(regs.ecx, 18) ? true : false;
	info->sse4_1 = BIT(regs.ecx, 19) ? true : false;
	info->sse4_2 = BIT(regs.ecx, 20) ? true : false;
	info->x2apic = BIT(regs.ecx, 21) ? true : false;
	info->movbe = BIT(regs.ecx, 22) ? true : false;
	info->popcnt = BIT(regs.ecx, 23) ? true : false;
	info->tscdeadline = BIT(regs.ecx, 24) ? true : false;
	info->aes = BIT(regs.ecx, 25) ? true : false;
	info->xsave = BIT(regs.ecx, 26) ? true : false;
	info->osxsave = BIT(regs.ecx, 27) ? true : false;
	info->avx = BIT(regs.ecx, 28) ? true : false;
	info->f16c = BIT(regs.ecx, 29) ? true : false;
	info->rdrand = BIT(regs.ecx, 30) ? true : false;
	info->hypervisor = BIT(regs.ecx, 31) ? true : false;

	if ((info->family == 0x06 && info->model == 0x09)
		|| (info->family == 0x06 && info->model == 0x0D)
		|| (info->family == 0x0F && info->model == 0x00)
		|| (info->family == 0x0F && info->model == 0x01)
		|| (info->family == 0x0F && info->model == 0x02))
		info->constant_tsc = false;							// It's these fucking pre-Prescott P4s and Xeons, I tell you.
	else
		info->constant_tsc = true;

	
	regs = cpuid(0x80000000);					// Can we do higher calls?
	maxcall = regs.eax;

	if (maxcall == 0x80000000)
		return;

	regs = cpuid(0x80000001);
		// xxx - do something here.

	if (maxcall == 0x80000001)
		return;
	
	uintptr = (unsigned int*)info->brand_string;
	regs = cpuid(0x80000002);
	uintptr[0] = regs.eax;
	uintptr[1] = regs.ebx;
	uintptr[2] = regs.ecx;
	uintptr[3] = regs.edx;
	regs = cpuid(0x80000003);
	uintptr[4] = regs.eax;
	uintptr[5] = regs.ebx;
	uintptr[6] = regs.ecx;
	uintptr[7] = regs.edx;
	regs = cpuid(0x80000004);
	uintptr[8] = regs.eax;
	uintptr[9] = regs.ebx;
	uintptr[10] = regs.ecx;
	uintptr[11] = regs.edx;

	charptr = info->brand_string;

	while (*charptr == ' ')
		charptr++;

	strcpy(info->brand_string, charptr);
	
	if (maxcall == 0x80000002 || maxcall == 0x80000003 || maxcall == 0x80000004)
		return;

	regs = cpuid(0x80000005);
		// xxx - do something here.

	if (maxcall == 0x80000005)
		return;

	regs = cpuid(0x80000006);
		// xxx - do something here.

	if (maxcall == 0x80000006)
		return;

	regs = cpuid(0x80000007);
	
	info->invariant_tsc = BIT(regs.edx, 7) ? true : false;

	if (maxcall == 0x80000007)
		return;
}

void cpu_reset(void) {
	unsigned char tfidt[6] = {0};
	__asm {
		lidt [tfidt]
		int 66h
	}
	panic(__FILE__, __LINE__, "Couldn't reset the CPU -- wtf?");
}