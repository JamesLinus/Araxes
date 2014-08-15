// BlacklightEVO fs\kramdisk.h -- KRAMdisk
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __FS__KRAMDISK_H
#define __FS__KRAMDISK_H

#include "global.h"
#include "core\mm.h"
#include "core\console.h"
#include "fs\vfs.h"

#define KRAMDISK_FILESYSTEM_ID 1
#define KRAMDISK_FILESYSTEM_NAME "KRAMdisk"

void kramdisk_init(struct vfs_device* vfs, char* name, char* address);

void kramdisk_sector_read(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba);
void kramdisk_sector_write(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba);

void kramdisk_read(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
void kramdisk_write(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
void kramdisk_create(struct vfs_device* vfs, const char* name);
void kramdisk_remove(struct vfs_device* vfs, const char* name);
void kramdisk_listdir(struct vfs_device* vfs, const char* name, char* buffer);
void kramdisk_info(struct vfs_device* vfs, const char* name, fileinfo_t* buffer);
void kramdisk_modify(struct vfs_device* vfs, const char* name, fileinfo_t* info);

#endif	// __FS__KRAMDISK_H