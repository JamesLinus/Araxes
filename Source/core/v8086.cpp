// BlacklightEVO core\v8086.cpp -- virtual 8086 mode, the bane of my existence
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\irq.h"
#include "core\console.h"

// This code is bad and pillaged from Chris Giese's v86 monitor.

typedef struct
{
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax; /* PUSHA/POP */
	unsigned ds, es, fs, gs;
	unsigned which_int, err_code;
	unsigned eip, cs, eflags, user_esp, user_ss; /* INT nn/IRET */
	unsigned v_es, v_ds, v_fs, v_gs; /* V86 mode only */
} uregs_t;

#define BPERL		16	/* byte/line for dump */

void dump(void *data_p, unsigned count)
{
	unsigned char *data = (unsigned char *)data_p;
	unsigned byte1, byte2;

	while(count != 0)
	{
		for(byte1 = 0; byte1 < BPERL; byte1++)
		{
			if(count == 0)
				break;
			printf("%02X ", data[byte1]);
			count--;
		}
		printf("\t");
		for(byte2 = 0; byte2 < byte1; byte2++)
		{
			if(data[byte2] < ' ')
				printf(".");
			else
				printf("%c", data[byte2]);
		}
		printf("\n");
		data += BPERL;
	}
}
/*****************************************************************************
*****************************************************************************/
void dump_iopb(void)
{
	unsigned i, offset, mask;

/* dump IOPB only if it was initially all-ones */
	if(g_tss_iopb[127] != 0xFF)
		return;
	printf("The following I/O ports were accessed:\n");
	for(i = 0; i < 1024; i++)
	{
		offset = i >> 3;
		mask = 0x01 << (i & 7);
		if((g_tss_iopb[offset] & mask) == 0)
			printf("%03X ", i);
	}
	printf("\n");
}
/*****************************************************************************
*****************************************************************************/
void dump_uregs(uregs_t *regs)
{
	printf("EDI=%08X    ESI=%08X    EBP=%08X    ESP=%08X    EBX=%08X\n",
		regs->edi, regs->esi, regs->ebp, regs->esp, regs->ebx);
	printf("EDX=%08X    ECX=%08X    EAX=%08X     DS=%08X     ES=%08X\n",
		regs->edx, regs->ecx, regs->eax, regs->ds, regs->es);
	printf(" FS=%08X     GS=%08X intnum=%08X  error=%08X    EIP=%08X\n",
		regs->fs, regs->gs, regs->which_int, regs->err_code,
		regs->eip);
	printf(" CS=%08X EFLAGS=%08X", regs->cs, regs->eflags);
//	if((regs->eflags & 0x20000uL) || (regs->cs & 0x03))
		printf("   uESP=%08X    uSS=%08X",
			regs->user_esp, regs->user_ss);
	printf("\n");
//	if(regs->eflags & 0x20000uL)
		printf("vES=%04X    vDS=%04X    vFS=%04X    vGS=%04X\n",
		regs->v_es & 0xFFFF, regs->v_ds & 0xFFFF,
		regs->v_fs & 0xFFFF, regs->v_gs & 0xFFFF);
}
/* from START.ASM */
extern unsigned char g_tss_iopb[];
extern unsigned g_kvirt_to_phys;
/*****************************************************************************
*****************************************************************************/
static unsigned long to_linear(unsigned seg, unsigned off)
{
	return (seg & 0xFFFF) * 16L + off;
}
/*****************************************************************************
*****************************************************************************/
unsigned peekb(unsigned seg, unsigned off)
{
	return *(unsigned char *)(to_linear(seg, off) - g_kvirt_to_phys);
}
/*****************************************************************************
*****************************************************************************/
unsigned peekw(unsigned seg, unsigned off)
{
	return *(unsigned short *)(to_linear(seg, off) - g_kvirt_to_phys);
}
/*****************************************************************************
*****************************************************************************/
static unsigned long peekl(unsigned seg, unsigned off)
{
	return *(unsigned int *)(to_linear(seg, off) - g_kvirt_to_phys);
}
/*****************************************************************************
*****************************************************************************/
void pokeb(unsigned seg, unsigned off, unsigned val)
{
	*(unsigned char *)(to_linear(seg, off) - g_kvirt_to_phys) = val;
}
/*****************************************************************************
*****************************************************************************/
void pokew(unsigned seg, unsigned off, unsigned val)
{
	*(unsigned short *)(to_linear(seg, off) - g_kvirt_to_phys) = val;
}
/*****************************************************************************
*****************************************************************************/
static void pokel(unsigned seg, unsigned off, unsigned long val)
{
	*(unsigned int *)(to_linear(seg, off) - g_kvirt_to_phys) = val;
}
/*****************************************************************************
*****************************************************************************/
static void v86_enable_port(unsigned port)
{
	unsigned mask;

	mask = 0x01 << (port & 7);
	port >>= 3;
	g_tss_iopb[port] &= ~mask;
}
/*****************************************************************************
xxx - GPF if EIP > 0xFFFF ?
*****************************************************************************/
static unsigned v86_fetch8(uregs_t *regs)
{
	unsigned byte;

	byte = peekb(regs->cs, regs->eip);
	regs->eip = (regs->eip + 1) & 0xFFFF;
	return byte;
}
/*****************************************************************************
xxx - next four functions should fault (stack fault; exception 0Ch)
if stack straddles 0xFFFF
*****************************************************************************/
void v86_push16(uregs_t *regs, unsigned value)
{
	regs->user_esp = (regs->user_esp - 2) & 0xFFFF;
	pokew(regs->user_ss, regs->user_esp, value);
}
/*****************************************************************************
*****************************************************************************/
static unsigned v86_pop16(uregs_t *regs)
{
	unsigned rv;

	rv = peekw(regs->user_ss, regs->user_esp);
	regs->user_esp = (regs->user_esp + 2) & 0xFFFF;
	return rv;
}
/*****************************************************************************
*****************************************************************************/
static void v86_push32(uregs_t *regs, unsigned long value)
{
	regs->user_esp = (regs->user_esp - 4) & 0xFFFF;
	pokel(regs->user_ss, regs->user_esp, value);
}
/*****************************************************************************
*****************************************************************************/
static unsigned long v86_pop32(uregs_t *regs)
{
	unsigned long rv;

	rv = peekl(regs->user_ss, regs->user_esp);
	regs->user_esp = (regs->user_esp + 4) & 0xFFFF;
	return rv;
}
/*****************************************************************************
*****************************************************************************/
void v86_int(uregs_t *regs, unsigned int_num)
{
/* push return IP, CS, and FLAGS onto V86 mode stack */
	v86_push16(regs, regs->eflags);
	v86_push16(regs, regs->cs);
	v86_push16(regs, regs->eip);
/* disable interrupts */
	regs->eflags &= ~0x200;
/* load new CS and IP from IVT */
	int_num *= 4;
	regs->eip = (regs->eip & ~0xFFFF) | peekw(0, int_num + 0);
	regs->cs = peekw(0, int_num + 2);
}
/*****************************************************************************
*****************************************************************************/
#define	PFX_ES		0x001
#define	PFX_CS		0x002
#define	PFX_SS		0x004
#define	PFX_DS		0x008
#define	PFX_FS		0x010
#define	PFX_GS		0x020

#define	PFX_OP32	0x040
#define	PFX_ADR32	0x080
#define	PFX_LOCK	0x100
#define	PFX_REPNE	0x200
#define	PFX_REP		0x400

int v86_emulate(uregs_t *regs)
{
	unsigned init_eip, prefix, i;

/* save current EIP so we can re-try instructions
instead of skipping over or emulating them */
	init_eip = regs->eip;
/* consume prefix bytes */
	prefix = 0;
	while(1)
	{
		i = v86_fetch8(regs);
		switch(i)
		{
		case 0x26:
			prefix |= PFX_ES;
			break;
		case 0x2E:
			prefix |= PFX_CS;
			break;
		case 0x36:
			prefix |= PFX_SS;
			break;
		case 0x3E:
			prefix |= PFX_DS;
			break;
		case 0x64:
			prefix |= PFX_FS;
			break;
		case 0x65:
			prefix |= PFX_GS;
			break;
		case 0x66:
			prefix |= PFX_OP32;
			break;
		case 0x67:
			prefix |= PFX_ADR32;
			break;
		case 0xF0:
			prefix |= PFX_LOCK;
			break;
		case 0xF2:
			prefix |= PFX_REPNE;
			break;
		case 0xF3:
			prefix |= PFX_REP;
			break;
		default:
			goto END;
		}
	}
END:
	switch(i)
	{
/* PUSHF */
	case 0x9C:
		if(prefix & PFX_OP32)
			v86_push32(regs, regs->eflags);
		else
			v86_push16(regs, regs->eflags);
		return 0;
/* POPF */
	case 0x9D:
		if(prefix & PFX_OP32)
		{
			if(regs->user_esp > 0xFFFC)
				return +1;
			regs->eflags = v86_pop32(regs);
		}
		else
		{
			if(regs->user_esp > 0xFFFE)
				return +1;
/* tarnation!		regs->eflags = v86_pop16(regs); */
			regs->eflags = (regs->eflags & 0xFFFF0000L) |
				v86_pop16(regs);
		}
		return 0;
/* INT nn */
	case 0xCD:
		i = v86_fetch8(regs); /* get interrupt number */
		v86_int(regs, i);
		return 0;
/* IRET */
	case 0xCF:
/* pop (E)IP, CS, (E)FLAGS */
		if(prefix & PFX_OP32)
		{
			if(regs->user_esp > 0xFFF4)
				return +1;
			regs->eip = v86_pop32(regs);
			regs->cs = v86_pop32(regs);
			regs->eflags = v86_pop32(regs);
		}
		else
		{
			if(regs->user_esp > 0xFFFA)
				return +1;
			regs->eip = v86_pop16(regs);
			regs->cs = v86_pop16(regs);
			regs->eflags = (regs->eflags & 0xFFFF0000L) |
				v86_pop16(regs);
		}
		return 0;
/************************************
I/O functions are not (yet) emulated. We just enable the
appropriate port in the IOPB and retry the instruction.
**************************************/
/* IN AL,imm8 */
	case 0xE4:
/* OUT imm8,AL */
	case 0xE6:
		i = v86_fetch8(regs);
		v86_enable_port(i);
/* restore original EIP -- we will re-try the instruction */
		regs->eip = init_eip;
		return 0;
/* IN [E]AX,imm8 */
	case 0xE5:
/* OUT imm8,[E]AX */
	case 0xE7:
		i = v86_fetch8(regs);
		v86_enable_port(i);
		v86_enable_port(i + 1);
		if(prefix & PFX_OP32)
		{
			v86_enable_port(i + 2);
			v86_enable_port(i + 3);
		}
		regs->eip = init_eip;
		return 0;
/* INSB */
	case 0x6C:
/* OUTSB */
	case 0x6E:
/* IN AL,DX */
	case 0xEC:
/* OUT DX,AL */
	case 0xEE:
		i = regs->edx & 0xFFFF;
		v86_enable_port(i);
		regs->eip = init_eip;
		return 0;
/* INSW, INSD */
	case 0x6D:
/* OUTSW, OUTSD */
	case 0x6F:
/* IN [E]AX,DX */
	case 0xED:
/* OUT DX,[E]AX */
	case 0xEF:
		i = regs->edx & 0xFFFF;
		v86_enable_port(i);
		v86_enable_port(i + 1);
		if(prefix & PFX_OP32)
		{
			v86_enable_port(i + 2);
			v86_enable_port(i + 3);
		}
		regs->eip = init_eip;
		return 0;
/* CLI */
	case 0xFA:
		regs->eflags &= ~0x200;
		return 0;
/* STI */
	case 0xFB:
		regs->eflags |= 0x200;
		return 0;
	}
/* anything else */
	printf("Error in V86 mode at CS:IP=%04X:%04X\n",
		regs->cs, init_eip);
	printf("Dump of bytes at CS:EIP:\n");
	dump((void *)(to_linear(regs->cs, init_eip) - g_kvirt_to_phys), 16);
	return -1;
}

/*****************************************************************************
Sets up V86 mode stack and initializes some registers
*****************************************************************************/
void init_v86_regs(uregs_t *regs)
{
/* V86 mode stack. This MUST be in conventional memory (below 1 meg)
but still be accessible to the pmode kernel: */
	static char _v86_stack[4096];
/**/
	char *v86_stack;
	unsigned s;

/* start with all registers zeroed */
	memset(regs, 0, sizeof(uregs_t));
/* point to BOTTOM (highest addess) of stack */
	v86_stack = _v86_stack + sizeof(_v86_stack);
/* v86_stack is virtual address. Convert it to physical address
and align it so bottom 4 bits == 0x0F
's' must be >= 0x10000 so we can set ESP=0xFFFF, below. */
	s = ((unsigned)v86_stack + g_kvirt_to_phys - 16) | 0x0F;
/* init stack */
	regs->user_esp = 0xFFFF;
	regs->user_ss = (s - 0xFFFF) / 16;
/* init kernel data segment registers */
	regs->ds = regs->es = regs->fs = regs->gs = 0x18;
/* init EFLAGS: set RF=0, NT=0, IF=0, and reserved bits */
#if 0
	regs->eflags = 0x00020002L; /* VM=1, IOPL=0 */
#else
	regs->eflags = 0x00023002L; /* VM=1, IOPL=3 */
#endif
}
/*****************************************************************************
Call real-mode interrupt handler in V86 mode
*****************************************************************************/
static void do_v86_int(uregs_t *regs, unsigned int_num)
{
	unsigned ivt_off;

/* convert int_num to IVT index */
	ivt_off = (int_num & 0xFF) * 4;
/* fetch CS:IP of real-mode interrupt handler */
	regs->cs = peekw(0, ivt_off + 2);
	regs->eip = peekw(0, ivt_off + 0);
/* do it */
	start_v86(regs);
}