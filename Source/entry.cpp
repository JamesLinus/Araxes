// BlacklightEVO entry.cpp -- kernel entry point
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "kernel.h"
#include "multiboot.h"

#define MULTIBOOT_FLAGS 0


#pragma code_seg(push, ".mboot")
__declspec(naked) void multiboot_header(void) {
	__asm {
			dd(MULTIBOOT_HEADER_MAGIC)
			dd(MULTIBOOT_FLAGS)
			dd(-(MULTIBOOT_HEADER_MAGIC + (MULTIBOOT_FLAGS)))
	}
}
#pragma code_seg(pop)

__declspec(naked) void multiboot_entry(void) {
    __asm {
			mov esp, 0x98000					; Gives us a fair bit of stack space before anything gets smashed

			xor ecx, ecx						; Zero out EFLAGS
			push ecx
			popfd

			push ebx
			push eax
			call kmain

			cli

		halt:
			hlt
			jmp halt
    }
}
