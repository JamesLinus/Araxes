// BlacklightEVO fs\nullfs.cpp -- NULL filesystem
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\mm.h"
#include "core\console.h"
#include "fs\nullfs.h"
#include "fs\vfs.h"

void nullfs_init(struct vfs_device* vfs, char* name) {
	if (!vfs) {
		printf("\nnullfs_init: vfs == 0, out of devices?\n");
		return;
	}
	vfs->used = true;
	strncpy(vfs->name, name, VFS_DEVICE_NAME_LENGTH);
	vfs->name[VFS_DEVICE_NAME_LENGTH-1] = '\0';
	vfs->filesystem = NULLFS_FILESYSTEM_NAME;
	vfs->mapping = 0;
	
	vfs->sector_read = nullfs_sector_read;
	vfs->sector_write = nullfs_sector_write;

	vfs->read = nullfs_read;
	vfs->write = nullfs_write;
	vfs->create = nullfs_create;
	vfs->remove = nullfs_remove;
	vfs->listdir = nullfs_listdir;
	vfs->info = nullfs_info;
	vfs->modify = nullfs_modify;
}

void nullfs_sector_read(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba) {
	memset(buffer, 0, 512);
}

void nullfs_sector_write(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba) {
	return;
}

void nullfs_read(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length) {
	memset(buffer, 0, length);
}

void nullfs_write(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length) {
	return;
}

void nullfs_create(struct vfs_device* vfs, const char* name) {
	return;
}

void nullfs_remove(struct vfs_device* vfs, const char* name) {
	return;
}

void nullfs_listdir(struct vfs_device* vfs, const char* name, char* buffer) {
	strcpy(buffer, "");
}

void nullfs_info(struct vfs_device* vfs, const char* name, fileinfo_t* buffer) {
	fileinfo_t* fileinfo = buffer;
	fileinfo->is_directory = false;
	fileinfo->uses_owner = false;
	fileinfo->size = 0;
}

void nullfs_modify(struct vfs_device* vfs, const char* name, fileinfo_t* info) {
	return;
}