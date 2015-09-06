// BlacklightEVO kernel/include/vbe.h -- VESA BIOS Extensions related code
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__VBE_H
#define __KERNEL__INCLUDE__VBE_H

struct vbe_mode_info {
	unsigned short mode;
	unsigned short width;
	unsigned short height;
	unsigned char  depth;
	unsigned short attributes;
	void* framebuffer;
};

extern bool vbe_initialized;
extern struct vbe_mode_info vbe_modelist[128];
extern char vbe_oem[48];
extern char vbe_vendor[48];
extern char vbe_product[48];
extern char vbe_revision[48];

bool vbe_exists(void);
bool vbe_initialize(void);
void vbe_get_mode_info(void);

#endif	// __KERNEL__INCLUDE__VBE_H
