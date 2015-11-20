// BlacklightEVO kernel/fs/mbr.c -- Master Boot Record structures
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <fs/mbr.h>

// Not all of these will be supported, but we recognize them.
// TODO: Replace magic numbers with constants.
char* mbr_type[256] = {
	[0x00] = "Empty",
	[0x01] = "FAT12 < 32 MB",
	[0x04] = "FAT16 < 32 MB",
	[0x06] = "FAT16 > 32 MB",
	[0x07] = "HPFS/NTFS",
	[0x08] = "Logical-sectored FAT",
	[0x0B] = "FAT32 CHS",
	[0x0C] = "FAT32 LBA",
	[0x0E] = "FAT16 LBA",
	[0x0F] = "LBA Extended",
	
	[0x14] = "Hidden FAT16 < 32 MB",
	[0x16] = "Hidden FAT16 > 32 MB",
	[0x17] = "Hidden HPFS/NTFS",
	[0x1A] = "Simple File System",
	[0x1C] = "Hidden FAT32 LBA",
	[0x1E] = "Hidden FAT16 LBA",
	[0x1F] = "Hidden LBA Extended",
	
	[0x27] = "Hidden NTFS Recovery",
	
	[0x35] = "OS/2 (AIX) JFS",
	[0x39] = "Plan 9",
	
	[0x42] = "Windows dynamic disk container",
	[0x4D] = "QNX POSIX Primary",
	[0x4E] = "QNX POSIX Secondary",
	[0x4F] = "QNX POSIX Tertiary",
	
	[0x80] = "MINIX old",
	[0x81] = "MINIX new",
	[0x82] = "Linux swap",
	[0x83] = "Linux native",
	[0x85] = "Linux extended",
	[0x8A] = "AiR-BOOT Linux kernel image",
	[0x8E] = "Linux LVM",
	
	[0x93] = "Linux hidden native",
	
	[0xA5] = "386BSD/FreeBSD slice",
	[0xA6] = "OpenBSD slice",
	[0xA7] = "NextSTEP",
	[0xA8] = "Apple UFS",
	[0xA9] = "NetBSD slice",
	[0xAB] = "Apple boot",
	[0xAF] = "Apple HFS/HFS+",
	
	[0xBE] = "Solaris 8 boot",
	[0xBF] = "Solaris (Sun disklabel)",
	
	[0xE0] = "EVOfs",
	[0xEB] = "BeOS BFS",
	[0xED] = "GPT hybrid MBR",
	[0xEE] = "GPT protective MBR",
	[0xEF] = "EFI system partition",
	
	[0xFB] = "VMware VMFS",
	[0xFC] = "VMware swap / VMKCORE",
	[0xFD] = "Linux RAID superblock",
};
