// BlacklightEVO core\irq.cpp -- Interrupt Requests
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\idt.h"
#include "core\irq.h"
#include "core\console.h"
#include "core\gdt.h"
#include "hardware\pit.h"
#include "hardware\keyboard.h"

	extern int derp;

void irq_initialize(void) {
	outb(0x20, 0x11);							// Remap the IRQ table to interrupts 70 through 7F
	outb(0xA0, 0x11);
	outb(0x21, 0x70);
	outb(0xA1, 0x78);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	idt_add_interrupt(0x70, (unsigned int)irq0, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x71, (unsigned int)irq1, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x72, (unsigned int)irq2, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x73, (unsigned int)irq3, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x74, (unsigned int)irq4, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x75, (unsigned int)irq5, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x76, (unsigned int)irq6, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x77, (unsigned int)irq7, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x78, (unsigned int)irq8, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x79, (unsigned int)irq9, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x7A, (unsigned int)irq10, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x7B, (unsigned int)irq11, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x7C, (unsigned int)irq12, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x7D, (unsigned int)irq13, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x7E, (unsigned int)irq14, gdt_kernel_cs, 0x8E);
	idt_add_interrupt(0x7F, (unsigned int)irq15, gdt_kernel_cs, 0x8E);
	
	outb(0x20, 0x20);							// Send a few ACKs to clear out any cluttered IRQs left behind by GRUB (it happens)
	outb(0x20, 0x20);
	outb(0x20, 0x20);
}

__declspec(naked) void irq0(void) {
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
	pit_interrupt();
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq1(void) {
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
	keyboard_interrupt();
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq2(void) {
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
	console_print("IRQ2 occurred, unhandled.");

	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq3(void) {
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
	console_print("IRQ3 occurred, unhandled.");

	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq4(void) {
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
	//console_print("IRQ4 occurred, unhandled.");	// xxx - actually handle this

	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq5(void) {
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
	console_print("IRQ5 occurred, unhandled.");

	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq6(void) {
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
	console_print("IRQ6 occurred, unhandled.");

	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq7(void) {
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
	console_print("IRQ7 occurred, unhandled.");

	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq8(void) {
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
	console_print("IRQ8 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq9(void) {
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
	console_print("IRQ9 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq10(void) {
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
	console_print("IRQ10 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq11(void) {
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
	console_print("IRQ11 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq12(void) {
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
	console_print("IRQ12 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq13(void) {
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
	console_print("IRQ13 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq14(void) {
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
	console_print("IRQ14 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}

__declspec(naked) void irq15(void) {
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
	console_print("IRQ15 occurred, unhandled.");

	outb(0xA0, 0x20);
	outb(0x20, 0x20);
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
		popad
	}
	__asm iretd;
}