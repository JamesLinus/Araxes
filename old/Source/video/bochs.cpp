// BlacklightEVO video\bochs.cpp -- Bochs Graphics Adapter
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "video\bochs.h"
#include "core\console.h"
#include "core\cpu.h"

int bochs_gfx_mode, bochs_gfx_x, bochs_gfx_y, bochs_gfx_depth;
void* bochs_doublebuffer, *bochs_framebuffer;

bool bochs_detect(unsigned short min_version) {
	//outb(0x80, 0xFF);
	//if (inb(0xE9) != 0xE9)
	//	return false;

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ID);
	if (inw(VBE_DISPI_IOPORT_DATA) >= min_version)
		return true;
	else
		return false;
}

bool qemu_detect(struct cpuinfo* info) {
	if (info->brand_string[0] == 'Q' && info->brand_string[0] == 'E' && info->brand_string[0] == 'M' && info->brand_string[0] == 'U')
		return true;
	else
		return false;
}

int bochs_set_mode(int x, int y, int depth) {
												// FUCK this hack. We should be detecting a BGA before this,
												// making sure we have an LFB, and then set everything up properly.

	bochs_framebuffer = (void*)0xE0000000;		// xxx - read the LFB address from PCI BAR.
	if (qemu_detect(&cpu_info))
		bochs_framebuffer = (void*)0xF0000000;

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);	// Disable the display while we change the resolution
	outw(VBE_DISPI_IOPORT_DATA, (unsigned short)0x00);

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES);		// Set up the resolution
	outw(VBE_DISPI_IOPORT_DATA, (unsigned short)x);
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES);
	outw(VBE_DISPI_IOPORT_DATA, (unsigned short)y);
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP);
	outw(VBE_DISPI_IOPORT_DATA, (unsigned short)depth);
	
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);	// Enable the display and LFB.
	outw(VBE_DISPI_IOPORT_DATA, (unsigned short)0x41);

	bochs_gfx_x = x;
	bochs_gfx_y = y;
	bochs_gfx_depth = depth;

	return 0;
}