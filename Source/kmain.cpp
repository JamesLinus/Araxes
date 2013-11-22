// BlacklightEVO kmain.cpp -- kernel main file
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net


/* Things that need to be done at some point, but aren't major (the todo list from hell):
 *
 * - Kernel configuration
 *   - We're going to do this as part of the multiboot command line
 * 
 * I'm sure I'll think of more.
 */

#include "global.h"
#include "kernel.h"
#include "logo.h"
#include "multiboot.h"
#include "core\cpu.h"
#include "core\console.h"
#include "core\gdt.h"
#include "core\idt.h"
#include "core\irq.h"
#include "core\math.h"
#include "core\mm.h"
#include "video\bochs.h"
#include "video\fb_font.h"
#include "video\fb_text.h"
#include "video\vbe.h"
#include "video\vbox.h"
#include "video\vt100.h"
#include "hardware\keyboard.h"
#include "hardware\pci.h"
#include "hardware\pit.h"
#include "hardware\uart.h"
#include "fs\kramdisk.h"
#include "fs\nullfs.h"
#include "fs\vfs.h"

char kernel_version_string[128] = {0};
const char* mottos[] = {
	"Extensible, Versatile, Open",
	"Slowly Gaining On ToAruOS",
	"Where Sysadmins Come To Die",
	"Randomized Motto Edition"
};

unsigned int derp;
short herp;
float quux;

extern size_t rmode_subkern_len;
extern unsigned char rmode_subkern[];

void(*subkern)() = (void(*)())0x3000;

short VIDEO_MODE_X = 800;
short VIDEO_MODE_Y = 600;
char VIDEO_MODE_BPP = 32;
bool VIDEO_FORCE_VBE = false;
bool VIDEO_FORCE_BOCHS = false;

bool serial_debugging = false;
bool tried24bpp = false;

unsigned int mboot_magic;
multiboot_info_t* mboot_info;

vbe_modeinfo* mode_info;


EXPORT void kmain(unsigned long magic, unsigned long multiboot) {
	mm_clear();

	mboot_magic = magic;
	mboot_info = (multiboot_info_t*)multiboot;

	if (mboot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic(__FILE__, __LINE__, "Multiboot magic number = 0x%X, expected 0x2BADB002", mboot_magic);
	
	if (!(mboot_info->flags & MULTIBOOT_INFO_CMDLINE))
		panic(__FILE__, __LINE__, "No mboot_info->cmdline - is the kernel properly configured?");
	multiboot_parse_args((char*)(mboot_info->cmdline));
	
	if (serial_debugging)
		init_serial(UART_BASE_RS0, UART_SPEED_9600, UART_PROTOCOL_8N1);
	debug_printf(UART_BASE_RS0, DEBUG_INFO "BlacklightEVO: Logging on rs0.\n", kernel_version_string);

	cpu_detect(&cpu_info);

	if (VIDEO_MODE_X == 0 || VIDEO_MODE_Y == 0 || VIDEO_MODE_BPP == 0)
		goto nasty_vga_goto;					// Fuck all of this.

	memcpy((void*)0x3000, rmode_subkern, rmode_subkern_len);

	__asm {
		xor ax, ax
		mov ebx, 0x5020
		mov word ptr [ebx], ax;
		add ebx, 0x10
		mov word ptr [ebx], ax;
		add ebx, 0x10
		mov byte ptr [ebx], al;
	}
	subkern();
	__asm mov ebx, 0x5050
	__asm mov eax, dword ptr [ebx];
	__asm mov [derp], eax;						// This is so badly unprofessional.

	if (derp == 0x5060)
		vbe_have_modelist = true;

	debug_printf(UART_BASE_RS0, DEBUG_INFO "Returned from subkern(), vbe_have_modelist = %s.\n", (vbe_have_modelist ? "true" : "false"));
	
	memcpy((void*)0x3000, rmode_subkern, rmode_subkern_len);


	if (VIDEO_MODE_X == 0 || VIDEO_MODE_Y == 0 || VIDEO_MODE_BPP == 0)
		;										// Do nothing -- we're using VGA terminal.
	else if (!VIDEO_FORCE_VBE) {
		if (vbox_detect()) {
			vbox_set_mode(VIDEO_MODE_X, VIDEO_MODE_Y, VIDEO_MODE_BPP, VBE_DISPI_LFB_ENABLED, 0, 0);
			console_mode = CONSOLE_VBOX;
			console_palette = PALETTE_XTERM;
			console_fg = 253;
			console_bg = 233;
			fb_update_setup();
		} else if (bochs_detect(0xB0C2) || VIDEO_FORCE_BOCHS) {
			bochs_set_mode(VIDEO_MODE_X, VIDEO_MODE_Y, VIDEO_MODE_BPP);
			console_mode = CONSOLE_BOCHS;
			console_palette = PALETTE_XTERM;
			console_fg = 253;
			console_bg = 233;
			fb_update_setup();
		} else
			goto nasty_vbe_goto;				// xxx - no gotos goddammit
	} else {
		nasty_vbe_goto:
		__asm {
			mov ax, [VIDEO_MODE_X]
			mov ebx, 0x5020
			mov word ptr [ebx], ax;
			mov ax, [VIDEO_MODE_Y]
			add ebx, 0x10
			mov word ptr [ebx], ax;
			mov al, [VIDEO_MODE_BPP]
			add ebx, 0x10
			mov byte ptr [ebx], al;
		}
		subkern();
		__asm mov ebx, 0x5050
		__asm mov eax, dword ptr [ebx];
		__asm mov [vbe_framebuffer], eax;
		if (vbe_framebuffer) {
			mode_info = (vbe_modeinfo*)0x8000;
			debug_printf(UART_BASE_RS0, DEBUG_INFO "VBE mode: width %d, height %d, bpp %d, physwidth %d, scanwidth %d\n", mode_info->width, mode_info->height, mode_info->bpp, (mode_info->width * mode_info->bpp / 8), mode_info->bytes_per_scanline);
			vbe_gfx_x = VIDEO_MODE_X;
			vbe_gfx_y = VIDEO_MODE_Y;
			vbe_gfx_depth = VIDEO_MODE_BPP;
			console_mode = CONSOLE_VBE;
			console_palette = PALETTE_XTERM;
			console_fg = 253;
			console_bg = 233;
			fb_update_setup();
		} else {
			if (!tried24bpp) {
				//tried24bpp = true;
				//VIDEO_MODE_BPP = 24;
				//memcpy((void*)0x3000, rmode_subkern, rmode_subkern_len);
				//goto nasty_vbe_goto;
			}
		}
	}

	debug_printf(UART_BASE_RS0, DEBUG_INFO "Returned from subkern().\n");
	
nasty_vga_goto:
	strcat(kernel_version_string, "Release ");
	strcat(kernel_version_string, itoa(KERNEL_VERSION_MAJOR, 10));
	strcat(kernel_version_string, ".");
	strcat(kernel_version_string, itoa(KERNEL_VERSION_MINOR, 10));
	strcat(kernel_version_string, ".");
	strcat(kernel_version_string, itoa(KERNEL_VERSION_WEEK >> 8, 16, 2));
	strcat(kernel_version_string, "w");
	strcat(kernel_version_string, itoa(KERNEL_VERSION_WEEK & 0xFF, 16, 2));
	strcat(kernel_version_string, " (" KERNEL_VERSION_NAME ")");
	console_clear();

	if (CONSOLE_IS_FRAMEBUFFER) {
		fb_draw_bitmap(4, 4, 64, 64, 255, logo_64);
		fb_draw_bitmap(4+32, 4+32, 64, 64, 128, logo_64);
		fb_draw_bitmap(4+64, 4+64, 64, 64, 128, logo_64);
		while (console_cursor_y * fb_font_height < 68)
			printf("\n");
	}

	printf("BlacklightEVO %s: Extensible, Versatile, Open\n", kernel_version_string);
	gdt_initialize();
	console_print("GDT ");
	idt_initialize();
	console_print("IDT ");
	irq_initialize();
	console_print("IRQ ");
	pit_initialize(60, true);
	console_print("PIT ");
	__asm sti;
	__asm {
		finit
		__emit 0x0F								// mov eax, cr4
		__emit 0x20								// fucking Visual C++ inline asm doesn't know what cr4 is
		__emit 0xE0
		__emit 0x0D								// or eax, 0x200
		__emit 0x00
		__emit 0x02
		__emit 0x00
		__emit 0x00
		__emit 0x0F								// mov cr4, eax
		__emit 0x22
		__emit 0xE0
		mov eax, cr0
		and eax, 0xFFFFFFFB
		or eax, 0x20
		mov cr0, eax
	}
	console_print("FPU/SSE ");
	//__asm cli;
	keyboard_initialize();
	//__asm sti;
	console_print("KBD ");
	vfs_init();
	console_print("VFS ");

	if (cpu_info.mmx == false)
		panic(__FILE__, __LINE__, "Processor does not have MMX -- cannot proceed with startup!");

	printf("\nAccording to cpu_info, we are running on a(n) %s processor, family %d, model %d, stepping %d. We %s a constant TSC.\n", cpu_info.brand_string, cpu_info.family, cpu_info.model, cpu_info.stepping, (cpu_info.constant_tsc ? "have" : "do not have"));

	printf("\nKernel started from %s with command line: %s\n", ((mboot_info->flags & MULTIBOOT_INFO_BOOT_LOADER_NAME) ? (char*)(mboot_info->boot_loader_name) : "unknown multiboot-compliant loader"), mboot_info->cmdline);

	printf("We used %s to configure a framebuffer at %dx%dx%d.\n", CONSOLE_MODE_STRING, fb_gfx_x, fb_gfx_y, fb_gfx_depth);

	//outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_FB_BASE_HI);
	//printf(" LFB: 0x%8X\n",((unsigned int)inw(VBE_DISPI_IOPORT_DATA) << 16));

	pci_bdf* pci = pci_get_bdf(0x80EE, 0xBEEF);
	printf("0x80EE:0xBEEF - BAR0: %#8X\n", pci_read_config_dword(pci, PCI_BAR0));
	free(pci);

	/*vfs_parse("HD0:foo\\bar\\baz\\qux\\quux");
	printf("\n");
	vfs_parse(":foo\\bar\\baz\\qux\\quux");
	printf("\n");
	vfs_parse("foo\\bar\\baz\\qux\\quux");*/

	

	//printf("Console type: %s%s%s. ", (console_mode == CONSOLE_VGA ? "" : "Graphical framebuffer ("), CONSOLE_MODE_STRING, (console_mode == CONSOLE_VGA ? "" : ")"));
	//printf("Width: %d, Height: %d (physical %dx%dx%d) ", console_width, console_height, fb_gfx_x, fb_gfx_y, fb_gfx_depth);
	//printf("Font: %s, %dx%d.\n", get_font_name(fb_font), fb_font_width, fb_font_height);
	//printf("Using %s, fg: %d, bg: %d.\n", (console_palette == PALETTE_EGA ? "EGA palette" : console_palette == PALETTE_XTERM ? "xterm palette" : "truecolour"), console_fg, console_bg);
	//printf("Framebuffer address: 0x%8X\n\n", (console_mode == CONSOLE_VGA ? 0xB8000 : (unsigned int)fb_framebuffer));
	// Begin doing things that are actually useful.

	printf("Kernel memory usage: %#X (%f%%)\n", mm_heap_end, (double)(unsigned int)mm_heap_end / (double)(unsigned int)mm_heap_cap * 100.0);
	printf("End of RAM: %#8X\n\n",multiboot_find_end_of_ram());
	
	//printf("Dumping VBE modelist:\n");
	//vbe_dump_modelist();

	//fb_flip();
	
	//extern void vfs_dump();
	//vfs_dump();
	
	//char* vpath = vfs_canonical_path("RAMDISK:");
	//char* path_device = vfs_get_device_name(vpath);
	
	//printf("KRAMDISK first byte: %X\n", *(unsigned char*)(vfs_find_device("KRAMDISK")->mapping));
	//char* t = (char*)malloc(5120);
	//memset(t, 0, 5120);
	//vfs_listdir("KRAMDISK:", t);
	//char* s = t;
	//strcpy(t, "Hello, world!");
	//printf("t = , *t = %s\n", t);

	//free(t);
	printf("We have a KRAMDISK device. Attempting to dump KRAMDISK:license.txt...\n\n\n");
	fileinfo_t f;
	vfs_info("KRAMDISK:license.txt", &f);
	//printf("KRAMDISK:license.txt - size = %d\n", f.size);
	char* ft = (char*)malloc(f.size + 1);
	memset(ft, 0, f.size + 1);
	vfs_read("KRAMDISK:license.txt", (unsigned char*)ft, 0, f.size);
	//printf("derp");
	//printf("ft first byte: %X", *(unsigned char*)ft);
	console_print(ft);
	//free(ft);

	
	printf("\nDumping PCI device list:\n");
	pci_dump();
	printf("\n");

	printf("\n\n\nDumping you at a dumb terminal, since there's nothing left to do. Have fun.\n\n");

	for (;;)
		console_putchar(keyboard_getchar());

	__asm {
		cli
		hlt
	}

	__asm ret; 
}