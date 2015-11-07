// BlacklightEVO kernel/idt.c -- x86 IDT/ISR/IRQ
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <terminal.h>
#include <printf.h>

#include <hardware/timer.h>
#include <hardware/rtc.h>

#define IDT_ENTRIES 255

typedef struct {
	unsigned short base_0_15;
	unsigned short selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short base_16_31;
} __attribute__((packed)) idt_entry;

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idtr_entry;

extern void idt_reload(void);	// from kernel/entry.asm

extern void int_isr_0(void);
extern void int_isr_1(void);
extern void int_isr_2(void);
extern void int_isr_3(void);
extern void int_isr_4(void);
extern void int_isr_5(void);
extern void int_isr_6(void);
extern void int_isr_7(void);
extern void int_isr_8(void);
extern void int_isr_9(void);
extern void int_isr_10(void);
extern void int_isr_11(void);
extern void int_isr_12(void);
extern void int_isr_13(void);
extern void int_isr_14(void);
extern void int_isr_15(void);
extern void int_isr_16(void);
extern void int_isr_17(void);
extern void int_isr_18(void);
extern void int_isr_19(void);
extern void int_isr_20(void);
extern void int_isr_21(void);
extern void int_isr_22(void);
extern void int_isr_23(void);
extern void int_isr_24(void);
extern void int_isr_25(void);
extern void int_isr_26(void);
extern void int_isr_27(void);
extern void int_isr_28(void);
extern void int_isr_29(void);
extern void int_isr_30(void);
extern void int_isr_31(void);

extern void int_irq_0(void);
extern void int_irq_1(void);
extern void int_irq_2(void);
extern void int_irq_3(void);
extern void int_irq_4(void);
extern void int_irq_5(void);
extern void int_irq_6(void);
extern void int_irq_7(void);
extern void int_irq_8(void);
extern void int_irq_9(void);
extern void int_irq_10(void);
extern void int_irq_11(void);
extern void int_irq_12(void);
extern void int_irq_13(void);
extern void int_irq_14(void);
extern void int_irq_15(void);

extern unsigned short gdt_kernel_cs;
idt_entry idt[IDT_ENTRIES+1];
idtr_entry idtr;

isr_t idt_isr_list[IDT_ENTRIES+1];
void idt_generic_fault(struct regs* regs);

void idt_generic_interrupt(void);
void idt_exception_DE(struct regs* regs);
void idt_exception_GP(struct regs* regs);
void idt_exception_PF(struct regs* regs);

void isr_irq_generic_master(struct regs* regs) {
	debug_printf(LOG_WARNING "Recieved IRQ %d with no handler, discarding", regs->int_no - 0x20);
	outb(0x20, 0x20);
	return;
}

void isr_irq_generic_slave(struct regs* regs) {
	debug_printf(LOG_WARNING "Recieved IRQ %d with no handler, discarding", regs->int_no - 0x20);
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}

void idt_initialize(void) {
	memset(idt_isr_list, 0, sizeof(isr_t)*(IDT_ENTRIES+1));

	idtr.base = (unsigned int)idt;
	idtr.limit = sizeof(idt);

	idt_add_interrupt(0, (unsigned int)int_isr_0, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(1, (unsigned int)int_isr_1, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(2, (unsigned int)int_isr_2, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(3, (unsigned int)int_isr_3, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(4, (unsigned int)int_isr_4, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(5, (unsigned int)int_isr_5, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(6, (unsigned int)int_isr_6, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(7, (unsigned int)int_isr_7, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(8, (unsigned int)int_isr_8, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(9, (unsigned int)int_isr_9, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(10, (unsigned int)int_isr_10, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(11, (unsigned int)int_isr_11, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(12, (unsigned int)int_isr_12, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(13, (unsigned int)int_isr_13, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(14, (unsigned int)int_isr_14, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(15, (unsigned int)int_isr_15, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(16, (unsigned int)int_isr_16, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(17, (unsigned int)int_isr_17, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(18, (unsigned int)int_isr_18, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(19, (unsigned int)int_isr_19, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(20, (unsigned int)int_isr_20, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(21, (unsigned int)int_isr_21, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(22, (unsigned int)int_isr_22, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(23, (unsigned int)int_isr_23, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(24, (unsigned int)int_isr_24, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(25, (unsigned int)int_isr_25, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(26, (unsigned int)int_isr_26, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(27, (unsigned int)int_isr_27, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(28, (unsigned int)int_isr_28, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(29, (unsigned int)int_isr_29, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(30, (unsigned int)int_isr_30, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(31, (unsigned int)int_isr_31, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(32, (unsigned int)int_irq_0, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(33, (unsigned int)int_irq_1, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(34, (unsigned int)int_irq_2, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(35, (unsigned int)int_irq_3, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(36, (unsigned int)int_irq_4, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(37, (unsigned int)int_irq_5, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(38, (unsigned int)int_irq_6, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(39, (unsigned int)int_irq_7, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(40, (unsigned int)int_irq_8, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(41, (unsigned int)int_irq_9, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(42, (unsigned int)int_irq_10, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(43, (unsigned int)int_irq_11, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(44, (unsigned int)int_irq_12, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(45, (unsigned int)int_irq_13, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(46, (unsigned int)int_irq_14, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(47, (unsigned int)int_irq_15, gdt_kernel_cs, 0x8E);

	idt_reload();
	
	outb(0x20, 0x11);							// Remap the IRQ table to interrupts 20 through 2F
	io_wait();
	outb(0xA0, 0x11);
	io_wait();
	outb(0x21, 0x20);
	io_wait();
	outb(0xA1, 0x28);
	io_wait();
	outb(0x21, 0x04);
	io_wait();
	outb(0xA1, 0x02);
	io_wait();
	outb(0x21, 0x01);
	io_wait();
	outb(0xA1, 0x01);
	io_wait();
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
	
	idt_isr_list[0]  = idt_exception_DE;
	idt_isr_list[13] = idt_exception_GP;
	idt_isr_list[14] = idt_exception_PF;
	idt_isr_list[32] = isr_irq_timer;
	idt_isr_list[33] = isr_irq_generic_master;
	idt_isr_list[34] = isr_irq_generic_master;
	idt_isr_list[35] = isr_irq_generic_master;
	idt_isr_list[36] = isr_irq_generic_master;
	idt_isr_list[37] = isr_irq_generic_master;
	idt_isr_list[38] = isr_irq_generic_master;
	idt_isr_list[39] = isr_irq_generic_master;
	idt_isr_list[40] = isr_irq_rtc;
	idt_isr_list[41] = isr_irq_generic_slave;
	idt_isr_list[42] = isr_irq_generic_slave;
	idt_isr_list[43] = isr_irq_generic_slave;
	idt_isr_list[44] = isr_irq_generic_slave;
	idt_isr_list[45] = isr_irq_generic_slave;
	idt_isr_list[46] = isr_irq_generic_slave;
	idt_isr_list[47] = isr_irq_generic_slave;
}

void idt_add_interrupt(int number, unsigned int base, unsigned short selector, unsigned char flags) {
	idt[number].base_0_15 = (unsigned short)(base & 0xFFFF);
	idt[number].selector = (unsigned short)(selector);
	idt[number].zero = 0x00;
	idt[number].flags = flags;
	idt[number].base_16_31 = (unsigned short)(base >> 16);
}

void idt_isr_handler(struct regs* regs) {
	isr_t isr = idt_isr_list[regs->int_no];
	if (isr) {
		isr(regs);
	} else {
		idt_generic_fault(regs);
	}
}

void idt_generic_fault(struct regs* regs) {
	asm volatile ("cli");
	char eflags[33];
	uitoa(eflags, regs->eflags, 2, 32);
	
	current_terminal->palette = PALETTE_EGA;
	current_terminal->color = 0x4E;
	kprintf("Exception handler called, dumping registers:                                    ");
	current_terminal->color = 0x1E;
	kprintf("EAX: %8X  EBX: %8X  ECX: %8X  EDX: %8X  ESI: %8X       EDI: %8X  CS: %4X  DS: %4X  ES: %4X  FS: %4X  GS: %4X  EIP: %8X  SS: %4X  ESP: %8X  EBP: %8X  EFLAGS: %s", regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->eip, regs->ss, regs->esp, regs->ebp, eflags);
	current_terminal->color = 0x4E;
	kprintf("Unhandled exception %2X occurred, error %8X.                                ", regs->int_no, regs->err_code);
	_crash();
}

void idt_exception_DE(struct regs* regs) {
	asm volatile ("cli");
	char eflags[33];
	uitoa(eflags, regs->eflags, 2, 32);
	
	current_terminal->palette = PALETTE_EGA;
	current_terminal->color = 0x4E;
	kprintf("Exception handler called, dumping registers:                                    ");
	current_terminal->color = 0x1E;
	kprintf("EAX: %8X  EBX: %8X  ECX: %8X  EDX: %8X  ESI: %8X       EDI: %8X  CS: %4X  DS: %4X  ES: %4X  FS: %4X  GS: %4X  EIP: %8X  SS: %4X  ESP: %8X  EBP: %8X  EFLAGS: %s", regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->eip, regs->ss, regs->esp, regs->ebp, eflags);
	current_terminal->color = 0x4E;
	kprintf("Divide error occurred in kernel, error %8X. System halted.                 ", regs->err_code);
	_crash();
}

void idt_exception_GP(struct regs* regs) {
	asm volatile ("cli");
	char eflags[33];
	uitoa(eflags, regs->eflags, 2, 32);
	
	current_terminal->palette = PALETTE_EGA;
	current_terminal->color = 0x4E;
	kprintf("Exception handler called, dumping registers:                                    ");
	current_terminal->color = 0x1E;
	kprintf("EAX: %8X  EBX: %8X  ECX: %8X  EDX: %8X  ESI: %8X       EDI: %8X  CS: %4X  DS: %4X  ES: %4X  FS: %4X  GS: %4X  EIP: %8X  SS: %4X  ESP: %8X  EBP: %8X  EFLAGS: %s", regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->eip, regs->ss, regs->esp, regs->ebp, eflags);
	current_terminal->color = 0x4E;
	kprintf("General protection fault occurred in kernel, error %8X. System halted.     ", regs->err_code);
	_crash();
}

void idt_exception_PF(struct regs* regs) {
	unsigned int cr2;
	asm volatile ("cli");
	asm volatile ("mov %%cr2, %0" : "=r"(cr2));
	char eflags[33];
	uitoa(eflags, regs->eflags, 2, 32);
	
	current_terminal->palette = PALETTE_EGA;
	current_terminal->color = 0x4E;
	kprintf("Exception handler called, dumping registers:                                    ");
	current_terminal->color = 0x1E;
	kprintf("EAX: %8X  EBX: %8X  ECX: %8X  EDX: %8X  ESI: %8X       EDI: %8X  CS: %4X  DS: %4X  ES: %4X  FS: %4X  GS: %4X  EIP: %8X  SS: %4X  ESP: %8X  EBP: %8X  EFLAGS: %s", regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi, regs->cs, regs->ds, regs->es, regs->fs, regs->gs, regs->eip, regs->ss, regs->esp, regs->ebp, eflags);
	int n = current_terminal->width - kprintf("CR2: %8X  - %s%s%s", cr2, (regs->err_code & 0x01 ? "present, " : "non-present, "), (regs->err_code & 0x02 ? "write fault, " : "read fault, "), (regs->err_code & 0x04 ? "user mode" : "kernel mode"));
	char* pad = malloc(n+1);
	memset(pad, ' ', n);
	pad[n] = '\0';
	kprintf("%s", pad);
	current_terminal->color = 0x4E;
	kprintf("Page fault occured in kernel, error %8X. System halted.                    ", regs->err_code);
	_crash();
}
