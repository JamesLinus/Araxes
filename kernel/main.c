// BlacklightEVO kernel/main.c -- kernel main file
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <stdbool.h> 
#include <stddef.h>
#include <stdint.h>

#include <global.h>
#include <mm.h>
#include <vga.h>


void kernel_main(uint32_t magic, uint32_t multiboot)
{
	vga_terminal_initialize();
	console_print("BlacklightEVO 0.1.0\n");
	gdt_initialize();
	console_print("GDT ");
	//idt_initialize();
	console_print("IDT ");
	console_print("\nLoaded.");
}
