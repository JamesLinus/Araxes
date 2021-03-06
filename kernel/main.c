// BlacklightEVO kernel/main.c -- kernel main file
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
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
#include <hardware/rtc.h>
#include <hardware/uart.h>
#include <vbe.h>

#include <x86/acpi.h>
#define EVOBOOT_BOOTLOADER_MAGIC 0x4D525655

uint16_t serial_debugging = UART_BASE_RS0;	// can be a port base or 0 for don't enable
extern const char nasm_version_string[];

struct terminal_info default_terminal;			// Default terminal (VGA 80x25 text)
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

void kernel_main(uint32_t magic, multiboot_info_t* multiboot, uint32_t oldmagic, uint32_t pcicfg)
{
	build_kernel_version_string();
	vga_terminal_initialize(current_terminal, 80, 25, (void*)0xB8000);
	kprintf(VT100_SGR_BOLD "BlacklightEVO %s - Release 1 (EVOlution)\n" VT100_SGR_NORMAL, kernel_version_string);
	kprintf("Built by %s on %s (GCC %s, %s)\n", KERNEL_BUILD_USER, __DATE__, __VERSION__, nasm_version_string);
	
	if (magic == MULTIBOOT_BOOTLOADER_MAGIC)
		kprintf("Image loaded via Multiboot-compatible bootloader.\n");
	else if (magic == EVOBOOT_BOOTLOADER_MAGIC)
		kprintf("Image loaded via EVOboot protocol%s.\n", (oldmagic == MULTIBOOT_BOOTLOADER_MAGIC ? " (via Multiboot-compatible bootloader)" : ""));
	else {
		kprintf("FUCK: magic = 0x%8lX\n", magic);
		_crash();
	}
	
	kprintf("multiboot structure = %8p\n", multiboot);
	kprintf("pcicfg = 0x%8lX\n", pcicfg);
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
	
	
	if (multiboot->flags & MULTIBOOT_INFO_MODS) {
		debug_printf(LOG_INFO "Enumerating %d Multiboot modules.\n", multiboot->mods_count);
		multiboot_module_t* mod = (multiboot_module_t*)multiboot->mods_addr;
		for (int i = 0; i < (int)multiboot->mods_count; i++) {
			size_t mod_length = mod[i].mod_end - mod[i].mod_start;
			debug_printf(LOG_INFO " - Module %d: %s - start %p, end %p, length %p\n", i, (char*)mod[i].cmdline ? (char*)mod[i].cmdline : "none", (void*)mod[i].mod_start, (void*)mod[i].mod_end, (void*)mod_length);
			mm_heap_end += (mod_length % 0x1000 ? (mod_length & 0xFFFFF000) + 0x1000 : mod_length);
		}
	}
	
	if (!(multiboot->flags & MULTIBOOT_INFO_MEM_MAP)) {
		kprintf("\n\nFUCK: We didn't get a memory map. We need a memory map.");
		_crash();
	}
	
	mm_create_mmap(multiboot);
	paging_set_directory(paging_kernel_directory);
	console_print("PG ");
	
	rtc_initialize();
	console_print("RTC ");
	
	if (vbe_initialize())
		console_print("VBE ");
	
	kprintf(VT100_SGR_BOLD "\nLoaded.\n\n" VT100_SGR_NORMAL);
	
	pci_enumerate();
	
	acpi_get_rsdp();
	
	for (;;);
}
