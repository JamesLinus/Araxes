// BlacklightEVO kernel/include/vbe.h -- VESA BIOS Extensions related code
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__VBE_H
#define __KERNEL__INCLUDE__VBE_H

#define VBE_ENUMERATION_DEBUG 0				// 0 for none, 1 for magic, 2 for more magic

struct vbe_info_block {
	unsigned char signature[4];
	unsigned short version;
	unsigned int oem_string;
	unsigned int capabilities;
	unsigned int modelist;
	unsigned short memory_size;
	unsigned short oem_revision;
	unsigned int oem_vendor;
	unsigned int oem_product_name;
	unsigned int oem_product_revision;
	unsigned char reserved[222];
	unsigned char oem_data[256];
} __attribute__((packed));

struct vbe_mode_info {
	unsigned short mode;
	unsigned short width;
	unsigned short height;
	unsigned char  depth;
	unsigned short attributes;
	void* framebuffer;
};

struct vbe_edid_info {
	unsigned char padding[8];
	unsigned short manufacturer;
	unsigned short model;
	unsigned int serial;
	unsigned char mfg_week;
	unsigned char mfg_year;
	unsigned char edid_version_major;
	unsigned char edid_version_minor;
	unsigned char input_type;
	unsigned char phys_horizontal;
	unsigned char phys_vertical;
	unsigned char gamma_factor;
	unsigned char dpms_flags;
	
	struct {
		unsigned char divisor1;
		unsigned char divisor2;
		unsigned char red_y;
		unsigned char red_x;
		unsigned char green_y;
		unsigned char green_x;
		unsigned char blue_y;
		unsigned char blue_x;
		unsigned char white_y;
		unsigned char white_x;
	} __attribute__((packed)) chroma;
	
	unsigned char established_timings1;
	unsigned char established_timings2;
	unsigned char mfg_timing;
	unsigned short standard_timings[8];
	struct {
		unsigned char hfreq;
		unsigned char vfreq;
		unsigned char hactive;
		unsigned char hblank;
		unsigned char hactive2_hblank2;
		unsigned char vactive;
		unsigned char vblank;
		unsigned char vactive2_vblank2;
		unsigned char hsync;
		unsigned char hsync_pulse;
		unsigned char vsync_vsyncpulse;
		unsigned char vhsync_vhsyncpulse;
		unsigned char hsize_mm;
		unsigned char vsize_mm;
		unsigned char hsize2_vsize2;
		unsigned char hborder;
		unsigned char vborder;
		unsigned char display_type;
	} __attribute__((packed)) dtd[4];
	unsigned char reserved;
	unsigned char checksum;
} __attribute__((packed));

extern unsigned char* multiboot_vbe_info;

extern bool vbe_initialized;
extern struct vbe_mode_info vbe_modelist[128];
extern struct vbe_edid_info vbe_edid;
extern char vbe_oem[64];
extern char vbe_vendor[64];
extern char vbe_product[64];
extern char vbe_revision[64];

bool vbe_exists(void);
bool vbe_initialize(void);
void vbe_get_mode_info(void);
void vbe_get_edid_info(void);

#endif	// __KERNEL__INCLUDE__VBE_H
