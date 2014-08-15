// BlacklightEVO video\vbox.cpp -- VirtualBox display adapter
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "video\vbox.h"
#include "video\bochs.h"						// VBox adapter seems to just be the Bochs adapter on crack
#include "core\console.h"
#include "hardware\pci.h"

int vbox_gfx_mode, vbox_gfx_x, vbox_gfx_y, vbox_gfx_depth;
void* vbox_doublebuffer, *vbox_framebuffer;

bool vbox_detect(void) {
	if (pci_exists(0x80EE, 0xBEEF))				// xxx - magic numbers is evil
		return true;
	else
		return false;
}

void vbox_enable_display(void) {
	unsigned short flags;

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
	flags = inw(VBE_DISPI_IOPORT_DATA);

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
	outw(VBE_DISPI_IOPORT_DATA, flags | VBE_DISPI_ENABLED);
}

void vbox_set_mode(unsigned short x, unsigned short y, unsigned short depth, unsigned short flags, unsigned short cx, unsigned short cy) {
	if (!vbox_detect())
		return;
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_XRES);
	outw(VBE_DISPI_IOPORT_DATA, x);
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_YRES);
	outw(VBE_DISPI_IOPORT_DATA, y);
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_VIRT_WIDTH);
	outw(VBE_DISPI_IOPORT_DATA, x);
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_BPP);
	outw(VBE_DISPI_IOPORT_DATA, depth);

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
	outw(VBE_DISPI_IOPORT_DATA, flags | VBE_DISPI_ENABLED);

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_X_OFFSET);
	outw(VBE_DISPI_IOPORT_DATA, cx);
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_Y_OFFSET);
	outw(VBE_DISPI_IOPORT_DATA, cy);

	vbox_gfx_x = x;
	vbox_gfx_y = y;
	vbox_gfx_depth = depth;

	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_FB_BASE_HI);
	vbox_framebuffer = (void*)((unsigned int)inw(VBE_DISPI_IOPORT_DATA) << 16);
}

void vbox_disable_vbe(void) {
	if (!vbox_detect())
		return;
	outw(VBE_DISPI_IOPORT_INDEX, VBE_DISPI_INDEX_ENABLE);
	outw(VBE_DISPI_IOPORT_DATA, 0);
}