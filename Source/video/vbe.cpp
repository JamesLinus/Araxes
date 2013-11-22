// BlacklightEVO video\vbe.cpp -- if all else fails, use VBE
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "video\vbe.h"
#include "core\console.h"

int vbe_gfx_mode, vbe_gfx_x, vbe_gfx_y, vbe_gfx_depth;
void* vbe_doublebuffer, *vbe_framebuffer;
bool vbe_have_modelist = false;

void vbe_dump_modelist() {
	unsigned short* modelist = (unsigned short*)0x5060;
	unsigned short mode, x, y, depth;

	if (!vbe_have_modelist)
		return;

	while (*modelist != 0xFFFF) {
		mode = *modelist;
		modelist += 1;
		x = *modelist;
		modelist += 1;
		y = *modelist;
		modelist += 1;
		depth = *modelist;
		modelist += 1;
		printf("Mode %#4x: %dx%dx%d\n", mode, x, y, depth);
	}
}