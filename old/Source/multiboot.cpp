// BlacklightEVO multiboot.cpp -- Multiboot and kernel args
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

/*
 * Kernel Argument Parser
 *
 * Parses arguments passed by, ie, a Multiboot bootloader.
 *
 * Part of the ToAruOS Kernel.
 * (C) 2011 Kevin Lange
 */

#include "global.h"
#include "multiboot.h"
#include "core\console.h"
#include "fs\kramdisk.h"
#include "hardware\keyboard.h"

extern short VIDEO_MODE_X;
extern short VIDEO_MODE_Y;
extern char VIDEO_MODE_BPP;
extern bool VIDEO_FORCE_VBE;
extern bool VIDEO_FORCE_BOCHS;

extern int DEFAULT_FONT_WIDTH;
extern int DEFAULT_FONT_HEIGHT;
extern unsigned char* DEFAULT_FONT_DATA;

extern unsigned char fb_font_vga[];
extern unsigned char fb_font_fixed[];
extern unsigned char fb_font_terminus[];

extern multiboot_info_t* mboot_info;

extern bool serial_debugging;

void multiboot_locate_ramdisk(vfs_device* vfs, char* name) {			// Perhaps this should be in kramdisk.cpp? Fuck it.
	struct multiboot_mod_list* module;
	if (!(mboot_info->flags & MULTIBOOT_INFO_MODS))
		return;

	if (!(mboot_info->mods_count))
		return;

	module = (multiboot_mod_list*)mboot_info->mods_addr;

	for (unsigned int i = 0; i < mboot_info->mods_count; i++) {
		debug_printf(UART_BASE_RS0, "Module get, cmdline = \"%s\", size = %d\n", (const char*)module[i].cmdline, module[i].mod_end - module[i].mod_start);
		if (!strcmp((const char*)module[i].cmdline, "/system/initrd.krd"))
			kramdisk_init(vfs, name, (char*)module[i].mod_start);
	}
}

void* multiboot_find_end_of_ram(void) {
	if (!(mboot_info->flags & MULTIBOOT_INFO_MEMORY))
		return 0;
	else
		return (void*)(mboot_info->mem_upper + 0x100000);
}

// From ToAruOS, pillaged and modified by us -- see toaru-license.txt
void multiboot_parse_args(char *_arg) {
	/* Sanity check... */
	if (!_arg) { return; }

	char * arg = strdup(_arg);
	char * pch;         /* Tokenizer pointer */
	char * save;        /* We use the reentrant form of strtok */
	char * argv[1024];  /* Command tokens (space-separated elements) */
	int    tokenid = 0; /* argc, basically */

	/* Tokenize the arguments, splitting at spaces */
	pch = strtok_r(arg," ",&save);
	if (!pch) { return; }
	while (pch != NULL) {
		argv[tokenid] = (char *)pch;
		++tokenid;
		pch = strtok_r(0," ",&save);
	}
	argv[tokenid] = 0;
	/* Tokens are now stored in argv. */


	for (int i = 0; i < tokenid; ++i) {
		/* Parse each provided argument */
		char * pch_i;
		char * save_i;
		char * argp[1024];
		int    argc = 0;
		pch_i = strtok_r(argv[i],"=",&save_i);
		if (!pch_i) { continue; }
		while (pch_i != NULL) {
			argp[argc] = (char *)pch_i;
			++argc;
			pch_i = strtok_r(0,"=,",&save_i);
		}
		argp[argc] = 0;

		if (!strcmp(argp[0],"video")) {
			if (argc < 4) {
				VIDEO_MODE_X = 800;
				VIDEO_MODE_Y = 600;
				VIDEO_MODE_BPP = 32;
			} else {
				VIDEO_MODE_X = atoi(argp[1]);
				VIDEO_MODE_Y = atoi(argp[2]);
				VIDEO_MODE_BPP = atoi(argp[3]);
				if (VIDEO_MODE_BPP > 32)
					VIDEO_MODE_BPP = 32;
				if (argc > 4) {
					if (!strcmp(argp[4],"vbe"))
						VIDEO_FORCE_VBE = true;
					else if (!strcmp(argp[4],"bochs"))
						VIDEO_FORCE_BOCHS = true;
				}
			}
		} else if (!strcmp(argp[0],"font")) {
			if (argc < 2) {
				DEFAULT_FONT_WIDTH = 6;
				DEFAULT_FONT_HEIGHT = 13;
				DEFAULT_FONT_DATA = fb_font_fixed;
			} else {
				if (!strcmp(argp[1],"fixed")) {
					DEFAULT_FONT_WIDTH = 6;
					DEFAULT_FONT_HEIGHT = 13;
					DEFAULT_FONT_DATA = fb_font_fixed;
				} else if (!strcmp(argp[1],"vga")) {
					DEFAULT_FONT_WIDTH = 8;
					DEFAULT_FONT_HEIGHT = 16;
					DEFAULT_FONT_DATA = fb_font_vga;
				} else if (!strcmp(argp[1],"terminus")) {
					DEFAULT_FONT_WIDTH = 8;
					DEFAULT_FONT_HEIGHT = 14;
					DEFAULT_FONT_DATA = fb_font_terminus;
				} else {
					DEFAULT_FONT_WIDTH = 6;
					DEFAULT_FONT_HEIGHT = 13;
					DEFAULT_FONT_DATA = fb_font_fixed;
				}
			}
		} else if (!strcmp(argp[0],"keyb")) {
			if (argc < 2) {
				keyboard_map = set1_qwerty_map;
				keyboard_shift_map = set1_qwerty_shift_map;
			} else {
				if (!strcmp(argp[1],"qwerty")) {
					keyboard_map = set1_qwerty_map;
					keyboard_shift_map = set1_qwerty_shift_map;
				} else if (!strcmp(argp[1],"dvorak")) {
					keyboard_map = set1_dvorak_map;
					keyboard_shift_map = set1_dvorak_shift_map;
				} else {
					keyboard_map = set1_qwerty_map;
					keyboard_shift_map = set1_qwerty_shift_map;
				}
			}
		} else if (!strcmp(argp[0],"vga")) {
			VIDEO_MODE_X = 0;
			VIDEO_MODE_Y = 0;
			VIDEO_MODE_BPP = 0;
		} else if (!strcmp(argp[0],"serial")) {
			serial_debugging = true;
		}
	}
}