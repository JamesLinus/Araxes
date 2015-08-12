// BlacklightEVO kernel/main.c -- kernel main file
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <stdbool.h> 
#include <stddef.h>
#include <stdint.h>

#include <global.h>
#include <terminal.h>
#include <gdt.h>
#include <mm.h>
#include <vga.h>
#include <printf.h>
#include <multiboot.h>

#include <hardware/timer.h>
#include <hardware/uart.h>

#define EVOBOOT_BOOTLOADER_MAGIC 0x4D525655

unsigned short serial_debugging = UART_BASE_RS0;	// can be a port base or 0 for don't enable
extern const char nasm_version_string[];

struct terminal_info default_terminal;				// Default terminal (VGA 80x25 text)
struct terminal_info* current_terminal = &default_terminal;

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

void kernel_main(unsigned int magic, multiboot_info_t* multiboot, unsigned int oldmagic)
{
	current_terminal->textbuffer = (unsigned char*)0xB8000;		// set up default vga terminal
	build_kernel_version_string();
	vga_terminal_initialize(current_terminal, 80, 25);
	kprintf("BlacklightEVO %s - Release 1 (EVOlution)\n", kernel_version_string);
	kprintf("Build date %s (GCC %s, %s)\n", __DATE__, __VERSION__, nasm_version_string);
	
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
		kprintf("Image loaded via Multiboot-compatible bootloader.\n");
	else if (magic == EVOBOOT_BOOTLOADER_MAGIC)
		kprintf("Image loaded via EVOboot protocol%s.\n", (oldmagic == MULTIBOOT_BOOTLOADER_MAGIC ? " (via Multiboot-compatible bootloader)" : ""));
	else {
		kprintf("FUCK: magic = 0x%8X\n", magic);
		_crash();
	}
	gdt_initialize();
	console_print("GDT ");
	idt_initialize();
	console_print("IDT ");
	timer_initialize(60, true);
	asm volatile("sti");
	console_print("PIT ");
	if (serial_debugging) {
		const char* whatportisit = (serial_debugging == UART_BASE_RS0 ? "RS0" : \
			(serial_debugging == UART_BASE_RS1 ? "RS1" : \
			(serial_debugging == UART_BASE_RS2 ? "RS2" : \
			(serial_debugging == UART_BASE_RS3 ? "RS3" : "WTF"))));
		uart_init_serial(serial_debugging, UART_SPEED_9600, UART_PROTOCOL_8N1);
		kprintf("UART=%s@9600-8N1 ", whatportisit);
		debug_printf(LOG_INFO "BlacklightEVO %s - Release 1 (EVOlution)\n", kernel_version_string);
		debug_printf(LOG_INFO "Kernel dump console on %s@9600-8N1.\n", whatportisit);
	}
	
	kprintf("\nmm_heap_end = %8p\n", mm_heap_end);
	
	if (multiboot->flags & MULTIBOOT_INFO_MODS) {
		kprintf("We have %d modules! What are they? Let's find out:\n", multiboot->mods_count);
		multiboot_module_t* mod = (multiboot_module_t*)multiboot->mods_addr;
		for (int i = 0; i < (int)multiboot->mods_count; i++) {
			size_t mod_length = mod[i].mod_end - mod[i].mod_start;
			kprintf(" - Module %d: %s - start %p, end %p, length %p\n", i, (char*)mod[i].cmdline ? (char*)mod[i].cmdline : "none", (void*)mod[i].mod_start, (void*)mod[i].mod_end, (void*)mod_length);
			mm_heap_end += (mod_length % 0x1000 ? (mod_length & 0xFFFFF000) + 0x1000 : mod_length);
			kprintf("   mm_heap_end = %8p\n", mm_heap_end);
		}
	}
	
	//kprintf("Quick printf test! %#8X; 1 - 4 = %d\n", 0xC0FFEE, 1 - 4);
	
	if (!(multiboot->flags & MULTIBOOT_INFO_MEM_MAP)) {
		kprintf("\n\nFUCK: We didn't get a memory map. We need a memory map.");
		_crash();
	}
	
	/*kprintf("mm_heap_end = %#8X\n", mm_heap_end);
	sbrk(1111, false);
	kprintf("mm_heap_end = %#8X\n", mm_heap_end);
	sbrk(1111, true);
	kprintf("mm_heap_end = %#8X\n", mm_heap_end);
	sbrk(4096, true);
	kprintf("mm_heap_end = %#8X\n", mm_heap_end);*/
	
	mm_create_mmap(multiboot);
	kprintf("mm_heap_end = %8p\n", mm_heap_end);
	paging_set_directory(paging_kernel_directory);
	console_print("PG ");
	
	console_print("\nLoaded.\n\n");
	
	kprintf("A 64-bit integer (2^33): %llu\n", (uint64_t)1<<33);
	kprintf("Two formats of the same hex: %#X %#x\n", 0x2BADB002, 0x2BADB002);
	
	char tstr[10] = {0};
	ksnprintf(tstr, 8, "qwertyuiop");
	kprintf("strlen(tstr): %u - \"%s\"\n\n", (unsigned int)strlen(tstr), tstr);
	
	//crash(__FILE__, __LINE__, "Testing the crash and the terminal all in one!");
	//volatile int oops = 4 / 0;
	//kprintf("we shouldn't get here");
	for (;;);
}
