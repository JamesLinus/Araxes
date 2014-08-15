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
#include <printf.h>

char kernel_version_string[24] = {0};
void build_kernel_version_string(void) {
	kernel_version_string[0] = KERNEL_VERSION_MAJOR + '0';
	kernel_version_string[1] = '.';
	kernel_version_string[2] = KERNEL_VERSION_MINOR + '0';
	kernel_version_string[3] = '.';
	kernel_version_string[4] = KERNEL_VERSION_PATCH + '0';
	
	if (KERNEL_VERSION_DEBUG > 26) {
		kernel_version_string[5] = (KERNEL_VERSION_DEBUG / 26) + 'a' - 1;
		kernel_version_string[6] = (KERNEL_VERSION_DEBUG % 26) + 'a' - 1;
	} else if (KERNEL_VERSION_DEBUG) {
		kernel_version_string[5] = KERNEL_VERSION_DEBUG + 'a' - 1;
	}
}

void kernel_main(uint32_t magic, uint32_t multiboot)
{
	build_kernel_version_string();
	vga_terminal_initialize();
	kprintf("BlacklightEVO %s - Release 1 (EVOlution)\n", kernel_version_string);
	gdt_initialize();
	console_print("GDT ");
	//idt_initialize();
	console_print("IDT ");
	console_print("\nLoaded.\n\n");
	kprintf("Quick printf test! %#8X; 1 - 4 = %d\n", 0xC0FFEE, 1 - 4);
}
