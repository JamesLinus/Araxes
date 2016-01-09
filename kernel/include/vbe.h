// BlacklightEVO kernel/include/vbe.h -- VESA BIOS Extensions related code
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__VBE_H
#define __KERNEL__INCLUDE__VBE_H

#define VBE_ENUMERATION_DEBUG 0				// 0 for none, 1 for magic, 2 for more magic

struct vbe_info_block {
	uint8_t signature[4];
	uint16_t version;
	uint32_t oem_string;
	uint32_t capabilities;
	uint32_t modelist;
	uint16_t memory_size;
	uint16_t oem_revision;
	uint32_t oem_vendor;
	uint32_t oem_product_name;
	uint32_t oem_product_revision;
	uint8_t reserved[222];
	uint8_t oem_data[256];
} __attribute__((packed));

struct vbe_mode_info {
	uint16_t mode;
	uint16_t width;
	uint16_t height;
	uint8_t  depth;
	uint16_t attributes;
	void* framebuffer;
};

struct vbe_edid_info {
	uint8_t padding[8];
	uint16_t manufacturer;
	uint16_t model;
	uint32_t serial;
	uint8_t mfg_week;
	uint8_t mfg_year;
	uint8_t edid_version_major;
	uint8_t edid_version_minor;
	uint8_t input_type;
	uint8_t phys_horizontal;
	uint8_t phys_vertical;
	uint8_t gamma_factor;
	uint8_t dpms_flags;
	
	struct {
		uint8_t divisor1;
		uint8_t divisor2;
		uint8_t red_y;
		uint8_t red_x;
		uint8_t green_y;
		uint8_t green_x;
		uint8_t blue_y;
		uint8_t blue_x;
		uint8_t white_y;
		uint8_t white_x;
	} __attribute__((packed)) chroma;
	
	uint8_t established_timings1;
	uint8_t established_timings2;
	uint8_t mfg_timing;
	uint16_t standard_timings[8];
	struct {
		uint8_t hfreq;
		uint8_t vfreq;
		uint8_t hactive;
		uint8_t hblank;
		uint8_t hactive2_hblank2;
		uint8_t vactive;
		uint8_t vblank;
		uint8_t vactive2_vblank2;
		uint8_t hsync;
		uint8_t hsync_pulse;
		uint8_t vsync_vsyncpulse;
		uint8_t vhsync_vhsyncpulse;
		uint8_t hsize_mm;
		uint8_t vsize_mm;
		uint8_t hsize2_vsize2;
		uint8_t hborder;
		uint8_t vborder;
		uint8_t display_type;
	} __attribute__((packed)) dtd[4];
	uint8_t reserved;
	uint8_t checksum;
} __attribute__((packed));

extern uint8_t* multiboot_vbe_info;

extern bool vbe_initialized;
extern bool vbe_have_edid;
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
