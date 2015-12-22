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
struct vbe_info_block vbe_info;

char vbe_oem[64];
char vbe_vendor[64];
char vbe_product[64];
char vbe_revision[64];

bool vbe_exists(void) {
	if (rmode_call(RMODE_CALL_VBE0) == 0x004F)
		return true;
	else
		return false;
}

bool vbe_initialize(void) {
	if (!vbe_exists())
		return vbe_initialized = false;
	
	memcpy(&vbe_info, (void*)RMGLOBAL_VBE_BUFFER, 512);
	
#if VBE_ENUMERATION_DEBUG
	kprintf("\n\n");
#endif

#if VBE_ENUMERATION_DEBUG == 2
	kprintf(VT100_SGR_BOLD "VBE debugging set to \"more magic\", dumping more magic:\n" VT100_SGR_NORMAL);
	for (int i = 0; i < 16; i++)
		kprintf("%02X ", ((unsigned char*)RMGLOBAL_VBE_BUFFER)[i]);
	kprintf("\n");
	for (int i = 16; i < 32; i++)
		kprintf("%02X ", ((unsigned char*)RMGLOBAL_VBE_BUFFER)[i]);
	kprintf("\n");
	for (int i = 32; i < 48; i++)
		kprintf("%02X ", ((unsigned char*)RMGLOBAL_VBE_BUFFER)[i]);
	kprintf("\n");
	for (int i = 48; i < 64; i++)
		kprintf("%02X ", ((unsigned char*)RMGLOBAL_VBE_BUFFER)[i]);
	kprintf("\n\n");
#endif
	
	if (RMPTR(vbe_info.oem_string))
		strncpy(vbe_oem, (char*)RMPTR(vbe_info.oem_string), 63);
	if (RMPTR(vbe_info.oem_vendor))
		strncpy(vbe_vendor, (char*)RMPTR(vbe_info.oem_vendor), 63);
	if (RMPTR(vbe_info.oem_product_name))
		strncpy(vbe_product, (char*)RMPTR(vbe_info.oem_product_name), 63);
	if (RMPTR(vbe_info.oem_product_revision))
		strncpy(vbe_revision, (char*)RMPTR(vbe_info.oem_product_revision), 63);
#if VBE_ENUMERATION_DEBUG
	kprintf("OEM=\"%s\" ", vbe_oem);
#endif
	vbe_oem[63] = '\0';
	vbe_vendor[63] = '\0';
	vbe_product[63] = '\0';
	vbe_revision[63] = '\0';
	
	vbe_initialized = true;
	
	vbe_get_mode_info();
	vbe_get_edid_info();
	
	return vbe_initialized;
}

void vbe_get_mode_info(void) {
	int i = 0;
	int j = 0;
	unsigned short* modelist, *p;
	
	if (!vbe_initialized)
		return;
	
	modelist = RMPTR(vbe_info.modelist);
		
	p = modelist;
#if VBE_ENUMERATION_DEBUG
	kprintf("PTR=0x%08X 0x%08X ", modelist, vbe_info.modelist);
#endif
	debug_printf(LOG_INFO "[VBE] Doing mode list iteration.\n");
	while (*p++ != 0xFFFF)
		;
	size_t modelist_size = (unsigned int)p - (unsigned int)modelist;
	modelist = (unsigned short*) malloc(modelist_size);
	if (!modelist)
		return;
	memcpy(modelist, RMPTR(vbe_info.modelist), modelist_size);
	
	for (i = 0; j < 127; i++) {
		if (modelist[i] == 0xFFFF) {
			vbe_modelist[j++].mode = 0xFFFF;
			break;
		}
		if (modelist[i] >= 0x100) {
			vbe_modelist[j++].mode = modelist[i];
		}
	}
	vbe_modelist[127].mode = 0xFFFF;

#if VBE_ENUMERATION_DEBUG
	kprintf("\n");
#endif
	for (i = 0; vbe_modelist[i].mode != 0xFFFF; i++) {
		*(unsigned short*)(RMGLOBAL_VIDEO_MODE) = vbe_modelist[i].mode;
#if VBE_ENUMERATION_DEBUG
		kprintf("M%03X ", vbe_modelist[i].mode);
#endif
		rmode_call(RMODE_CALL_VBE1);
		vbe_modelist[i].width = *(unsigned short*)(RMGLOBAL_VBE_BUFFER+0x12);
		vbe_modelist[i].height = *(unsigned short*)(RMGLOBAL_VBE_BUFFER+0x14);
		vbe_modelist[i].depth = *(unsigned char*)(RMGLOBAL_VBE_BUFFER+0x19);
		if (!vbe_modelist[i].width || !vbe_modelist[i].height || !vbe_modelist[i].depth) {
			vbe_modelist[i].mode = 0;
			vbe_modelist[i].width = 0;
			vbe_modelist[i].height = 0;
			vbe_modelist[i].depth = 0;
			continue;
		}
		
		vbe_modelist[i].attributes = *(unsigned short*)(RMGLOBAL_VBE_BUFFER);
		vbe_modelist[i].framebuffer = (void*)*(unsigned int*)(RMGLOBAL_VBE_BUFFER+0x28);
	}
	
#if VBE_ENUMERATION_DEBUG
	kprintf("\n\n");
#endif

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
