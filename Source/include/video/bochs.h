// BlacklightEVO video\bochs.h -- Bochs Graphics Adapter
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __VIDEO__BOCHS_H
#define __VIDEO__BOCHS_H

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_IOPORT_DAC_WRITE_INDEX  0x03C8
#define VBE_DISPI_IOPORT_DAC_DATA         0x03C9

#define VBE_DISPI_INDEX_ID 0
#define VBE_DISPI_INDEX_XRES 1
#define VBE_DISPI_INDEX_YRES 2
#define VBE_DISPI_INDEX_BPP 3
#define VBE_DISPI_INDEX_ENABLE 4
#define VBE_DISPI_INDEX_BANK 5
#define VBE_DISPI_INDEX_VIRT_WIDTH 6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 7
#define VBE_DISPI_INDEX_X_OFFSET 8
#define VBE_DISPI_INDEX_Y_OFFSET 9

extern int bochs_gfx_mode, bochs_gfx_x, bochs_gfx_y, bochs_gfx_depth;
extern void* bochs_doublebuffer, *bochs_framebuffer;

bool bochs_detect(unsigned short min_version);
int bochs_set_mode(int x, int y, int depth);
void bochs_update_cursor(void);
void bochs_clear_screen(void);
void bochs_scroll(void);
void bochs_putchar(char c);

#endif	// __VIDEO__BOCHS_H