// BlacklightEVO include\fs\vfs.h -- virtual filesystem
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __FS__VFS_H
#define __FS__VFS_H

#define VFS_DEVICES_MAX 128
#define VFS_DEVICE_NAME_LENGTH 32

typedef struct {
	unsigned int size;
	bool is_directory;
	bool uses_owner;
	int owner;
} fileinfo_t;

struct vfs_device {
	bool used;
	char name[VFS_DEVICE_NAME_LENGTH];
	char* filesystem;
	unsigned int mapping;

	void (*sector_read)(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba);
	void (*sector_write)(struct vfs_device* vfs, unsigned char* buffer, unsigned int lba);

	void (*read)(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
	void (*write)(struct vfs_device* vfs, const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
	void (*create)(struct vfs_device* vfs, const char* name);
	void (*remove)(struct vfs_device* vfs, const char* name);
	void (*listdir)(struct vfs_device* vfs, const char* name, char* buffer);
	void (*info)(struct vfs_device* vfs, const char* name, fileinfo_t* buffer);
	void (*modify)(struct vfs_device* vfs, const char* name, fileinfo_t* info);
};

void vfs_init(void);

void vfs_read(const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
void vfs_write(const char* name, unsigned char* buffer, unsigned int offset, unsigned int length);
void vfs_create(const char* name);
void vfs_remove(const char* name);
void vfs_listdir(const char* name, char* buffer);
void vfs_info(const char* name, fileinfo_t* buffer);
void vfs_modify(const char* name, fileinfo_t* info);

struct vfs_device* vfs_get_free_device(void);
struct vfs_device* vfs_find_device(char* name);
char* vfs_get_device_name(char* path);
char* vfs_get_path(char* path);
char* vfs_canonical_path(char* path);
void vfs_parse(char* path);

#endif	// __FS__VFS_H
