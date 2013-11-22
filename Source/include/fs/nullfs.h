// BlacklightEVO fs\nullfs.h -- NULL filesystem
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __FS__NULLFS_H
#define __FS__NULLFS_H

#include "global.h"
#include "core\mm.h"
#include "core\console.h"
#include "fs\vfs.h"

#define NULLFS_FILESYSTEM_ID 0
#define NULLFS_FILESYSTEM_NAME "NullFS"

void nullfs_init(struct vfs_device* vfs, char* name);

void nullfs_sector_read(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba);
void nullfs_sector_write(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba);

void nullfs_read(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
void nullfs_write(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
void nullfs_create(struct vfs_device* vfs, const char* name);
void nullfs_remove(struct vfs_device* vfs, const char* name);
void nullfs_listdir(struct vfs_device* vfs, const char* name, char* buffer);
void nullfs_info(struct vfs_device* vfs, const char* name, fileinfo_t* buffer);
void nullfs_modify(struct vfs_device* vfs, const char* name, fileinfo_t* info);

#endif	// __FS__NULLFS_H