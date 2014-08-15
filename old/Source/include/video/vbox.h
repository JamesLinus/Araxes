// BlacklightEVO video\vbox.cpp -- VirtualBox display adapter
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __VIDEO__VBOX_H
#define __VIDEO__VBOX_H

#define VBE_DISPI_INDEX_FB_BASE_HI 0x0B

#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_GETCAPS 0x02
#define VBE_DISPI_8BIT_DAC 0x20
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80

extern int vbox_gfx_mode, vbox_gfx_x, vbox_gfx_y, vbox_gfx_depth;
extern void* vbox_doublebuffer, *vbox_framebuffer;

bool vbox_detect(void);
void vbox_enable_display(void);
void vbox_set_mode(unsigned short x, unsigned short y, unsigned short depth, unsigned short flags, unsigned short cx, unsigned short cy);
void vbox_disable_vbe(void);

#endif	// __VIDEO__VBOX_H