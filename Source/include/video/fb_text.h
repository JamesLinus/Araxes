// BlacklightEVO video\fb_text.h -- framebuffer text stuff
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __VIDEO__FB_TEXT_H
#define __VIDEO__FB_TEXT_H

#define HEADER_PIXEL(data, pixel) {\
pixel[3] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
pixel[2] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
pixel[1] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
pixel[0] = 0; \
data += 4; \
}

extern unsigned int xterm_palette[256];

extern int fb_gfx_mode, fb_gfx_x, fb_gfx_y, fb_gfx_depth;
extern unsigned int* fb_doublebuffer, *fb_triplebuffer;
extern void* fb_framebuffer;
extern unsigned char* fb_font;
extern int fb_font_width, fb_font_height;

void fb_flip(void);
void fb_putpixel(int x, int y, unsigned int c);
void fb_update_setup(void);
void fb_update_cursor(bool draw);
void fb_clear_screen(void);
void fb_scroll(void);
void fb_putchar(char c);
void fb_draw_bitmap(int x, int y, int width, int height, char* data);
void fb_draw_bitmap(int x, int y, int width, int height, int alpha, char* data);

#endif	//__VIDEO__FB_TEXT_H