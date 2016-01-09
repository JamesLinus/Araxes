// BlacklightEVO kernel/include/fb_font.h -- all sorts of weird fonts
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__FB_FONT_H
#define __KERNEL__INCLUDE__FB_FONT_H

extern uint8_t fb_font_vga[4096];			// 8x16
extern uint8_t fb_font_fixed[3328];		// 6x13

const char* get_font_name(uint8_t* font);

#endif	// __KERNEL__INCLUDE__FB_FONT_H
