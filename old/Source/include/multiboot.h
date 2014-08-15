// BlacklightEVO core\multiboot.h -- this file reeks of GNU
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__MULTIBOOT_H
#define __CORE__MULTIBOOT_H

#include "fs\vfs.h"

/* Copyright (C) 1999,2003,2007,2008,2009  Free Software Foundation, Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to
*  deal in the Software without restriction, including without limitation the
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
*  sell copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
*  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define MULTIBOOT_SEARCH                        8192
#define MULTIBOOT_HEADER_MAGIC                  0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC              0x2BADB002
#define MULTIBOOT_UNSUPPORTED                   0x0000fffc
#define MULTIBOOT_MOD_ALIGN                     0x00001000
#define MULTIBOOT_INFO_ALIGN                    0x00000004
     
// Flags set in the 'flags' member of the multiboot header.
#define MULTIBOOT_PAGE_ALIGN                    0x00000001
#define MULTIBOOT_MEMORY_INFO                   0x00000002
#define MULTIBOOT_VIDEO_MODE                    0x00000004
#define MULTIBOOT_AOUT_KLUDGE                   0x00010000
     
// Flags to be set in the 'flags' member of the multiboot info structure.
#define MULTIBOOT_INFO_MEMORY                   0x00000001
#define MULTIBOOT_INFO_BOOTDEV                  0x00000002
#define MULTIBOOT_INFO_CMDLINE                  0x00000004
#define MULTIBOOT_INFO_MODS                     0x00000008
#define MULTIBOOT_INFO_AOUT_SYMS                0x00000010
#define MULTIBOOT_INFO_ELF_SHDR                 0X00000020
#define MULTIBOOT_INFO_MEM_MAP                  0x00000040
#define MULTIBOOT_INFO_DRIVE_INFO               0x00000080
#define MULTIBOOT_INFO_CONFIG_TABLE             0x00000100
#define MULTIBOOT_INFO_BOOT_LOADER_NAME         0x00000200
#define MULTIBOOT_INFO_APM_TABLE                0x00000400
#define MULTIBOOT_INFO_VIDEO_INFO               0x00000800
     
struct multiboot_header {
	/* Must be MULTIBOOT_MAGIC - see above. */
	unsigned int magic;
     
	/* Feature flags. */
	unsigned int flags;
     
	/* The above fields plus this one must equal 0 mod 2^32. */
	unsigned int checksum;
     
	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	unsigned int header_addr;
	unsigned int load_addr;
	unsigned int load_end_addr;
	unsigned int bss_end_addr;
	unsigned int entry_addr;
     
	/* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
	unsigned int mode_type;
	unsigned int width;
	unsigned int height;
	unsigned int depth;
};
     
/* The symbol table for a.out. */
struct multiboot_aout_symbol_table {
	unsigned int tabsize;
	unsigned int strsize;
	unsigned int addr;
	unsigned int reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;
     
/* The section header table for ELF. */
struct multiboot_elf_section_header_table {
	unsigned int num;
	unsigned int size;
	unsigned int addr;
	unsigned int shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;
     
struct multiboot_info {
	/* Multiboot info version number */
	unsigned int flags;
     
	/* Available memory from BIOS */
	unsigned int mem_lower;
	unsigned int mem_upper;
     
	/* "root" partition */
	unsigned int boot_device;
     
	/* Kernel command line */
	unsigned int cmdline;
     
	/* Boot-Module list */
	unsigned int mods_count;
	unsigned int mods_addr;
     
	union
	{
		multiboot_aout_symbol_table_t aout_sym;
		multiboot_elf_section_header_table_t elf_sec;
	} u;
     
	/* Memory Mapping buffer */
	unsigned int mmap_length;
	unsigned int mmap_addr;
     
	/* Drive Info buffer */
	unsigned int drives_length;
	unsigned int drives_addr;
     
	/* ROM configuration table */
	unsigned int config_table;
     
	/* Boot Loader Name */
	unsigned int boot_loader_name;
     
	/* APM table */
	unsigned int apm_table;
     
	/* Video */
	unsigned int vbe_control_info;
	unsigned int vbe_mode_info;
	unsigned short vbe_mode;
	unsigned short vbe_interface_seg;
	unsigned short vbe_interface_off;
	unsigned short vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;
     
struct multiboot_mmap_entry {
	unsigned int size;
	unsigned long long addr;
	unsigned long long len;
	#define MULTIBOOT_MEMORY_AVAILABLE              1
	#define MULTIBOOT_MEMORY_RESERVED               2
	unsigned int type;
};
typedef struct multiboot_mmap_entry multiboot_memory_map_t;
     
struct multiboot_mod_list {
	/* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
	unsigned int mod_start;
	unsigned int mod_end;
     
	/* Module command line */
	unsigned int cmdline;
     
	/* padding to take it to 16 bytes (must be zero) */
	unsigned int pad;
};
typedef struct multiboot_mod_list multiboot_module_t;

void multiboot_locate_ramdisk(vfs_device* vfs, char* name);
void* multiboot_find_end_of_ram(void);
void multiboot_parse_args(char *_arg);

#endif	// __CORE__MULTIBOOT_H