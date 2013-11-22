// BlacklightEVO core\idt.h -- Interrupt Descriptor Table
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__IDT_H
#define __CORE__IDT_H

#define IDT_ENTRIES 255

typedef struct {
	unsigned short base_0_15;
	unsigned short selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short base_16_31;
} idt_entry;

typedef struct {
    unsigned short limit;
    unsigned int base;
} idtr_entry;

void idt_initialize(void);
void idt_add_interrupt(int number, unsigned int base, unsigned short selector, unsigned char flags);

/*__forceinline __declspec(naked) void idt_enter(void) {			// Force inline
	__asm {
		push ds
		push es
		push fs
		push gs
		
		mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
	}
}
__forceinline __declspec(naked) void idt_exit(void) {				// Force inline
	__asm {
		pop gs
		pop fs
		pop es
		pop ds
	}
}*/


#endif  // __CORE__IDT_H