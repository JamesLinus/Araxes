// BlacklightEVO kernel/vbe.c -- VESA BIOS Extensions related code
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <vbe.h>

struct vbe_mode_info vbe_modelist[128];

bool vbe_exists(void) {
	rmode_call(RMODE_CALL_VBE0);
	if (*(unsigned char*)(RMGLOBAL_VBE_BUFFER+3) == '2')
		return false;
	else
		return true;
}

void vbe_get_mode_info(void) {
	int i = 0;
	int j = 0;
	if (!vbe_exists())
		return;
	unsigned short* modelist = RMPTR(*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x0E));
	
	for (i = 0; j < 127; i++) {
		if (modelist[i] == 0xFFFF) {
			vbe_modelist[j++].mode = 0xFFFF;
			break;
		}
		if (modelist[i] & 0x100) {
			vbe_modelist[j++].mode = modelist[i];
		}
	}
	vbe_modelist[127].mode = 0xFFFF;
	
	for (i = 0; vbe_modelist[i].mode != 0xFFFF; i++) {
		*(unsigned short*)(RMGLOBAL_VIDEO_MODE) = vbe_modelist[i].mode;
		rmode_call(RMODE_CALL_VBE1);
		vbe_modelist[i].width = *(unsigned short*)(RMGLOBAL_VBE_BUFFER+0x12);
		vbe_modelist[i].height = *(unsigned short*)(RMGLOBAL_VBE_BUFFER+0x14);
		vbe_modelist[i].depth = *(unsigned char*)(RMGLOBAL_VBE_BUFFER+0x19);
		vbe_modelist[i].attributes = *(unsigned short*)(RMGLOBAL_VBE_BUFFER);
		vbe_modelist[i].framebuffer = (void*)*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x28);
	}
	
	debug_printf(LOG_INFO "[VBE] Added %i modes to vbe_modelist.\n", j);
}
