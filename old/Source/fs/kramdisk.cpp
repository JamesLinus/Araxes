// BlacklightEVO fs\kramdisk.cpp -- KRAMdisk
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\mm.h"
#include "core\console.h"
#include "fs\kramdisk.h"
#include "fs\vfs.h"

void kramdisk_init(struct vfs_device* vfs, char* name, char* address) {
	if (!vfs) {
		printf("\nkramdisk_init: vfs == 0, out of devices?\n");
		return;
	}
	vfs->used = true;
	strncpy(vfs->name, name, VFS_DEVICE_NAME_LENGTH);
	vfs->name[VFS_DEVICE_NAME_LENGTH-1] = '\0';
	vfs->filesystem = KRAMDISK_FILESYSTEM_NAME;
	vfs->mapping = (unsigned int)address;
	
	vfs->sector_read = kramdisk_sector_read;
	vfs->sector_write = kramdisk_sector_write;

	vfs->read = kramdisk_read;
	vfs->write = kramdisk_write;
	vfs->create = kramdisk_create;
	vfs->remove = kramdisk_remove;
	vfs->listdir = kramdisk_listdir;
	vfs->info = kramdisk_info;
	vfs->modify = kramdisk_modify;
}

char* kramdisk_find(struct vfs_device* vfs, const char* name) {
	char vname[25] = {0};
	char* address = (char*)vfs->mapping;
	char* vpath = vfs_get_path((char*)name);
	char* temp = vpath;
	char* temp1;
	bool pathed = false;
	
	while (temp) {
		temp1 = strtok((pathed ? 0 : vpath), "\\");
		if (!temp1)
			break;
		temp = temp1;
		pathed = true;
	}

	temp1 = 0;

	while (address < (char*)vfs->mapping + 512) {
		memcpy(vname, address, 24);
		if (!strcmp(vname, temp)) {
		//printf(vname);
			temp1 = address - vfs->mapping;
			break;
		}
		address += 32;
	}

	free(vpath);
	return temp1;
}

void kramdisk_sector_read(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba) {
	memcpy(buffer, (void*)(vfs->mapping+lba*512), 512);
}

void kramdisk_sector_write(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba) {
	memcpy((void*)(vfs->mapping+lba*512), buffer, 512);
}

void kramdisk_read(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length) {
	char* address = (char*)vfs->mapping;
	char* temp = kramdisk_find(vfs, name);
	//if (!temp) {
	//	printf("wtf"); //return;
	//}
		//return;

	address += *(unsigned int*)(vfs->mapping + temp + 24);
	
	memcpy(buffer, address + offset, length);
	//printf(" - length %d, address %#X, byte %X\n", length, address, *(unsigned char*)address+offset);
}

void kramdisk_write(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length) {
	return;
}

void kramdisk_create(struct vfs_device* vfs, const char* name) {
	return;
}

void kramdisk_remove(struct vfs_device* vfs, const char* name) {
	return;
}

void kramdisk_listdir(struct vfs_device* vfs, const char* name, char* buffer) {
	char vname[25] = {0};
	char* address = (char*)(vfs->mapping + kramdisk_find(vfs, name));
	//printf("\nvfs->mapping = %#8X, address = %#8X", vfs->mapping, address);
	char* data = buffer;
	memset(data, 0, 512);
	//printf("\ndata = %#8X, *data = %#2X", data, *(unsigned char*)data);
	address = (char*)vfs->mapping;				// xxx - something wrong with kramdisk_find. I think it's the "" usage.

	while (address < (char*)vfs->mapping + 512) {
		memcpy(vname, address, 24);
		//printf("\nwhat do we have: '%s'", vname);
		if (strcmp(vname, "")) {				// Fucking strcmp returns 0 on success
			//printf(" - found one");
			strcat(data, vname);
			strcat(data, "\xFF");
		}
		address += 32;
	}

	//printf("\n*data: %s\n", data);

}

void kramdisk_info(struct vfs_device* vfs, const char* name, fileinfo_t* buffer) {
	//printf("\n\n\nDOES THIS EVEN GET CALLED GODDAMMIT\n\n\n");
	char* address = (char*)vfs->mapping;
	fileinfo_t* fileinfo = buffer;
	char* temp = kramdisk_find(vfs, name);
	//if (!temp)
	//	printf("wtf");

	address += (unsigned int)temp;
	fileinfo->is_directory = false;
	fileinfo->size = *(unsigned int*)(address + 28);
	//printf("address = %#8X, size = %#8X\n", address, fileinfo->size);
	//return fileinfo;
}

void kramdisk_modify(struct vfs_device* vfs, const char* name, fileinfo_t* info) {
	return;
}