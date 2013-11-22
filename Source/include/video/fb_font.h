// BlacklightEVO video\fb_font.h -- all sorts of weird fonts
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __VIDEO__FB_FONT_H
#define __VIDEO__FB_FONT_H

extern unsigned char fb_font_vga[4096];			// 8x16
extern unsigned char fb_font_fixed[3328];		// 6x13
extern unsigned char fb_font_terminus[3584];	// 8x14, Copyright (C) 2006 Dimitar Toshkov Zhekov

const char* get_font_name(unsigned char* font);

#endif	// __VIDEO__FB_FONT_H