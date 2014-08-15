// BlacklightEVO core\idt.cpp -- Interrupt Descriptor Table
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\gdt.h"
#include "core\idt.h"
#include "core\exceptions.h"
#include "core\console.h"

idt_entry idt[IDT_ENTRIES];
idtr_entry idtr;

void idt_generic_interrupt(void);

void idt_initialize(void) {
	idtr.base = (unsigned int)idt;
	idtr.limit = IDT_ENTRIES * 8;

	for (int i = 0; i < 256; i++)
		idt_add_interrupt(i, (unsigned int)idt_generic_interrupt, gdt_kernel_cs, 0x8E);		// Initialize the IDT with generic interrupts.

	idt_add_interrupt(0x00, (unsigned int)exception_int00, gdt_kernel_cs, 0x8E);
	//idt_add_interrupt(0x01, (unsigned int)exception_int01, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x02, (unsigned int)exception_int02, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x04, (unsigned int)exception_int04, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x05, (unsigned int)exception_int05, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x06, (unsigned int)exception_int06, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x07, (unsigned int)exception_int07, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x08, (unsigned int)exception_int08, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x09, (unsigned int)exception_int09, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x0A, (unsigned int)exception_int0A, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x0B, (unsigned int)exception_int0B, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x0C, (unsigned int)exception_int0C, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x0D, (unsigned int)exception_int0D, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x10, (unsigned int)exception_int10, gdt_kernel_cs, 0x8E);

	__asm {
			lidt [idtr]
	}
}

void idt_add_interrupt(int number, unsigned int base, unsigned short selector, unsigned char flags) {
	idt[number].base_0_15 = (unsigned short)(base & 0xFFFF);
	idt[number].selector = (unsigned short)(selector);
	idt[number].zero = 0x00;
	idt[number].flags = flags;
	idt[number].base_16_31 = (unsigned short)(base >> 16);
}

__declspec(naked) void idt_generic_interrupt(void) {
	__asm {
		pushad
		push ds
		push es
		push fs
		push gs

		mov ax, 0x18
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
	}
	console_print("Unhandled interrupt occurred.");
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
		iretd;
	}
}