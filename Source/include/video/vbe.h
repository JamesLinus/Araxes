// BlacklightEVO video\vbe.h -- if all else fails, use VBE
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __VIDEO__VBE_H
#define __VIDEO__VBE_H

#include "global.h"
#include "core\console.h"

extern int vbe_gfx_mode, vbe_gfx_x, vbe_gfx_y, vbe_gfx_depth;
extern void* vbe_doublebuffer, *vbe_framebuffer;
extern bool vbe_have_modelist;

void vbe_dump_modelist(void);

typedef struct {
	unsigned short mode_attributes;
	unsigned char window_attributes_a;
	unsigned char window_attributes_b;
	unsigned short window_granularity;
	unsigned short window_size;
	unsigned short window_segment_a;
	unsigned short window_segment_b;
	unsigned int window_positioning_far;
	unsigned short bytes_per_scanline;

	unsigned short width;
	unsigned short height;
	unsigned char char_width;
	unsigned char char_height;
	unsigned char planes;
	unsigned char bpp;
	unsigned char banks;
	unsigned char memory_model;
	unsigned char bank_size;
	unsigned char images;
	unsigned char reserved_1;
} vbe_modeinfo;

#endif	// __VIDEO__VBE_H