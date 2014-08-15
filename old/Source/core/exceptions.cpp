// BlacklightEVO core\exceptions.cpp -- x86 CPU exception handlers
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\idt.h"
#include "core\console.h"

__declspec(naked) void exception_int00(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Divide Error");
}

__declspec(naked) void exception_int02(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Non-Maskable Interrupt");
}

__declspec(naked) void exception_int04(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Overflow");
}

__declspec(naked) void exception_int05(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Bound Range Exceeded");
}

__declspec(naked) void exception_int06(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Invalid Opcode");
}

__declspec(naked) void exception_int07(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Device Not Available");
}

__declspec(naked) void exception_int08(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Double Fault");
}

__declspec(naked) void exception_int09(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Coprocessor Segment Overrun");
}

__declspec(naked) void exception_int0A(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Invalid TSS");
}

__declspec(naked) void exception_int0B(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Segment Not Present");
}

__declspec(naked) void exception_int0C(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Stack Segment Fault");
}

__declspec(naked) void exception_int0D(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "General Protection Fault");
}

__declspec(naked) void exception_int0E(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Page Fault");
}

__declspec(naked) void exception_int10(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Floating-Point Exception");
}

__declspec(naked) void exception_int11(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Alignment Check");
}

__declspec(naked) void exception_int12(void) {
	__asm {
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
	panic(__FILE__, __LINE__, "Machine Check Exception");
}