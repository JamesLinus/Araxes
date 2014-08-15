// BlacklightEVO kernel/include/fb_font.h -- all sorts of weird fonts
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__FB_FONT_H
#define __KERNEL__INCLUDE__FB_FONT_H

extern unsigned char fb_font_vga[4096];			// 8x16
extern unsigned char fb_font_fixed[3328];		// 6x13

const char* get_font_name(unsigned char* font);

#endif	// __KERNEL__INCLUDE__FB_FONT_H
