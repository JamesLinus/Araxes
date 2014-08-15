// BlacklightEVO kernel/idt.c -- x86 IDT/ISR/IRQ
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>

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
void idt_generic_fault(struct regs *r);

void idt_generic_interrupt(void);

void idt_initialize(void) {
	memset(idt_isr_list, 0, sizeof(isr_t)*(IDT_ENTRIES+1));

	idtr.base = (unsigned int)idt;
	idtr.limit = IDT_ENTRIES+1 * 8;

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
}

void idt_add_interrupt(int number, unsigned int base, unsigned short selector, unsigned char flags) {
	idt[number].base_0_15 = (unsigned short)(base & 0xFFFF);
	idt[number].selector = (unsigned short)(selector);
	idt[number].zero = 0x00;
	idt[number].flags = flags;
	idt[number].base_16_31 = (unsigned short)(base >> 16);
}

void idt_isr_handler(struct regs* r) {
	isr_t isr;
	isr = idt_isr_list[r->int_no];
	if (isr) {
		isr(r);
	} else {
		idt_generic_fault(r);
	}
}

void idt_generic_fault(struct regs* r) {
	asm volatile ("cli");
	console_print("Unhandled interrupt occurred.");
	asm volatile ("cli; hlt;");
}
