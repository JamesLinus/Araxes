// BlacklightEVO kernel/vbe.c -- VESA BIOS Extensions related code
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <vbe.h>

bool vbe_initialized = false;
bool vbe_have_edid = false;

struct vbe_mode_info vbe_modelist[128];
struct vbe_edid_info vbe_edid;

char vbe_oem[48];
char vbe_vendor[48];
char vbe_product[48];
char vbe_revision[48];

bool vbe_exists(void) {
	rmode_call(RMODE_CALL_VBE0);
	if (*(unsigned char*)(RMGLOBAL_VBE_BUFFER+3) == '2')
		return false;
	else
		return true;
}

bool vbe_initialize(void) {
	if (!vbe_exists())
		return vbe_initialized = false;
		
	strncpy(vbe_oem, (char*)RMPTR(*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x06)), 47);
	strncpy(vbe_vendor, (char*)RMPTR(*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x16)), 47);
	strncpy(vbe_product, (char*)RMPTR(*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x1A)), 47);
	strncpy(vbe_revision, (char*)RMPTR(*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x1E)), 47);
	
	vbe_oem[47] = '\0';
	vbe_vendor[47] = '\0';
	vbe_product[47] = '\0';
	vbe_revision[47] = '\0';
	
	vbe_get_mode_info();
	vbe_get_edid_info();
	
	return vbe_initialized = true;
}

void vbe_get_mode_info(void) {
	int i = 0;
	int j = 0;
	if (!vbe_exists())
		return;
	unsigned short* modelist = RMPTR(*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x0E));
	debug_printf(LOG_INFO "[VBE] Doing mode list iteration.\n");
	
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

void vbe_get_edid_info(void) {
	unsigned int rmodecall = 0;
	unsigned char* edidbuf = (unsigned char*)RMGLOBAL_VBE_BUFFER;
	if (!vbe_exists())
		return;
		
	rmodecall = rmode_call(RMODE_CALL_EDID);
	
	if (rmodecall == 0x00004F00) {
		vbe_have_edid = true;	
		memcpy(&vbe_edid, edidbuf, 128);
		debug_printf(LOG_INFO "[VBE] EDID information retrieved.\n");
	} else
		debug_printf(LOG_WARNING "[VBE] No EDID information was available, got 0x%08X.\n", rmodecall);
}
