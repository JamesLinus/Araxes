// BlacklightEVO kernel/include/fs/mbr.h -- Master Boot Record structures
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__FS__MBR_H
#define __KERNEL__INCLUDE__FS__MBR_H

struct mbr_partition_entry {
	unsigned char status;
	unsigned char start_head;
	unsigned char start_sector;
	unsigned char start_cylinder;
	unsigned char type;
	unsigned char end_head;
	unsigned char end_sector;
	unsigned char end_cylinder;
	unsigned int lba_start;
	unsigned int lba_length;
} __attribute__((packed));

struct mbr_sector {
	unsigned char bootstrap[446];
	struct mbr_partition_entry partitions[4];
	unsigned char signature[2];
} __attribute__((packed));

struct mbr_type {
	unsigned char id;
	char* name;
};

extern char* mbr_type[256];

#endif	// __KERNEL__INCLUDE__FS__MBR_H
