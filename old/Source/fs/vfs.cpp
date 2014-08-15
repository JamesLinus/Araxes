// BlacklightEVO fs\vfs.cpp -- virtual filesystem
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "multiboot.h"
#include "core\mm.h"
#include "core\console.h"
#include "fs\kramdisk.h"
#include "fs\nullfs.h"
#include "fs\vfs.h"

struct vfs_device vfs_devices[VFS_DEVICES_MAX];

char vfs_current_device[VFS_DEVICE_NAME_LENGTH];
char vfs_current_directory[256];

void vfs_init() {
	for (int i = 0; i < VFS_DEVICES_MAX; i++) {
		memset(vfs_devices[i].name, 0, VFS_DEVICE_NAME_LENGTH);
		vfs_devices[i].used = false;
	}
	vfs_current_directory[0] = '\0';

	nullfs_init(vfs_get_free_device(), "NULL");
	multiboot_locate_ramdisk(vfs_get_free_device(), "KRAMDISK");

	strcpy(vfs_current_device, "NULL");
	strcpy(vfs_current_directory, "");

	// xxx - scan for devices here. might as well do it here
}

void vfs_dump() {
	for (int i = 0; i < VFS_DEVICES_MAX; i++) {
		if (vfs_devices[i].used)
			printf("VFS device %d \"%s\", type %s\n", i, vfs_devices[i].name, vfs_devices[i].filesystem);
	}
}

void vfs_read(const char* name, unsigned char* buffer, unsigned int offset, unsigned int length) {
	unsigned char* buf = buffer;
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	char* temp = vpath;
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	
	if (device)
		device->read(device, vpath, buf, offset, length);
	else
		printf("wtf no device?\n");
	//free(device);
	free(path_device);
	free(vpath);
}

void vfs_write(const char* name, unsigned char* buffer, unsigned int offset, unsigned int length) {
	unsigned char* buf = buffer;
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	char* temp = vpath;
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	
	if (device)
		device->write(device, vpath, buf, offset, length);
	//free(device);
	free(path_device);
	free(vpath);
}

void vfs_create(const char* name) {
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	char* temp = vpath;
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	
	if (device)
		device->create(device, vpath);
	//free(device);
	free(path_device);
	free(vpath);
}

void vfs_remove(const char* name) {
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	char* temp = vpath;
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	
	if (device)
		device->remove(device, vpath);
	//free(device);
	free(path_device);
	free(vpath);
}


void vfs_listdir(const char* name, char* buffer) {
	char* buf = buffer;
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	char* temp = "";
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	if (device)
		device->listdir(device, vpath, buf);
	//free(device);
	free(path_device);
	free(vpath);
	//printf("foop = , *foop = %s\n", temp);
	//return temp;
}

void vfs_info(const char* name, fileinfo_t* buffer) {
	fileinfo_t* buf = buffer;
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	
	//printf("vpath = '%s', path_device = '%s'", vpath, path_device);
	if (device)
		device->info(device, vpath, buf);
	//else
		//printf("wtf no device?\n");
	//free(device);
	free(path_device);
	free(vpath);
}
void vfs_modify(const char* name, fileinfo_t* info) {
	fileinfo_t* buf = info;
	char* vpath = vfs_canonical_path((char*)name);
	char* path_device = vfs_get_device_name(vpath);
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);
	
	if (device)
		device->modify(device, vpath, buf);
	//free(device);
	free(path_device);
	free(vpath);
}

struct vfs_device* vfs_get_free_device() {
	for (int i = 0; i < VFS_DEVICES_MAX; i++) {
		if (vfs_devices[i].used == false)
			return &vfs_devices[i];
	}
	
	return 0;
}

struct vfs_device* vfs_find_device(char* name) {
	for (int i = 0; i < VFS_DEVICES_MAX; i++) {
		if (!strcmp(vfs_devices[i].name, name))
			return &vfs_devices[i];
	}
	
	return 0;
}

char* vfs_get_device_name(char* path) {
	char* temp;
	char* vpath = (char*)malloc(strlen(path));
	strcpy(vpath, path);

	char* path_device = (char*)malloc(VFS_DEVICE_NAME_LENGTH);
	temp = strtok(vpath, ":");
	if (temp)
		strcpy(path_device, temp);
	else
		strcpy(path_device, vfs_current_device);
	free(vpath);
	return path_device;
}

char* vfs_get_path(char* path) {
	char* temp;
	char* vpath = (char*)malloc(strlen(path));
	strcpy(vpath, path);
	char* path_device = vfs_get_device_name(vpath);

	temp = path + strlen(path_device) + 1;

	free(path_device);
	free(vpath);
	return temp;
}

char* vfs_canonical_path(char* path) {
	int plen = strlen(vfs_current_device) + strlen(vfs_current_directory + strlen(path)) + 1;
	char* vpath = (char*)malloc(plen);
	memset(vpath, '\0', plen);

	strcpy(vpath, path);

	//printf("Path: %s\n", vpath);

	if (vpath[0] == ':') {
		strcpy(vpath, vfs_current_device);
		strcat(vpath, path);
	} else if (!strchr(vpath, ':')) {
		strcpy(vpath, vfs_current_device);
		strcat(vpath, ":");
		strcat(vpath, vfs_current_directory);
		strcat(vpath, "\\");
		strcat(vpath, path);
	}

	return vpath;
}

void vfs_parse(char* path) {
	char* vpath = vfs_canonical_path(path);
	char* path_device = vfs_get_device_name(vpath);
	char* temp = vpath;
	bool pathed = false;
	struct vfs_device* device = vfs_find_device(path_device);

	printf("Device: %s\n", path_device);

	while (temp) {
		temp = strtok((pathed ? 0 : vpath + strlen(path_device) + 1), "\\");
		if (!temp)
			break;
		printf("Entry: %s\n", temp);
		pathed = true;
	}

	printf("\n");
	free(vpath);
	free(path_device);
}