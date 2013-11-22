// BlacklightEVO video\fb_text.cpp -- framebuffer text stuff
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\cpu.h"
#include "core\mm.h"
#include "hardware\pit.h"
#include "hardware\uart.h"
#include "video\fb_font.h"
#include "video\fb_text.h"
#include "video\bochs.h"
#include "video\vbox.h"
#include "video\vbe.h"
#include "core\console.h"


void* fb_text_fb = (void*)0x000B8000;

bool* fb_modified = 0;
int fb_gfx_mode, fb_gfx_x, fb_gfx_y, fb_gfx_depth;
unsigned int* fb_doublebuffer = 0;
unsigned int* fb_triplebuffer = 0;
unsigned int* fb_quadbuffer = 0;
void* fb_framebuffer;
int fb_font_width, fb_font_height;
unsigned char* fb_font;

bool fb_scrolling = false;
bool fb_using_cursor = true;
bool currently_flipping = false;

int DEFAULT_FONT_WIDTH = 6;
int DEFAULT_FONT_HEIGHT = 13;
unsigned char* DEFAULT_FONT_DATA = fb_font_fixed;

unsigned int fb_clut[16] = {
	0xFF000000,
	0xFF0000AA,
	0xFF00AA00,
	0xFF00AAAA,
	0xFFAA0000,
	0xFFAA00AA,
	0xFFAA5500,
	0xFFAAAAAA,
	0xFF555555,
	0xFF5555FF,
	0xFF55FF55,
	0xFF55FFFF,
	0xFFFF5555,
	0xFFFF55FF,
	0xFFFFFF55,
	0xFFFFFFFF
};

	int mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

void* memset_4(void* s, int d, size_t n) {
	size_t nn = n / 4;
	__asm {
		mov eax, [d]
		mov edi, [s]
		mov ecx, [nn]
		rep stosd
	}
	return s;
}

//#pragma intrinsic(memcpy)

void* memcpy_4(void* dest, const void* src, size_t n) {
	//return memcpy(dest,src,n);
	size_t nn = n / 4;
	__asm {
		mov esi, [src]
		mov edi, [dest]
		mov ecx, [nn]
		rep movsd
	}
	return dest;
}

void* memcpy_32_to_16(void* dest, const void* src, size_t n) {
	unsigned short sh = 0;
	size_t nn = n / 4;
	unsigned short *dp = (unsigned short*)dest;
	const unsigned int *sp = (const unsigned int*)src; dest;
	while (nn--) {
		*dp++ = (unsigned short)(((*sp & 0xFF) >> 3) | (((*sp & 0xFF00) >> 10) << 5) | (((*sp & 0xFF0000) >> 19) << 11));
		sp++;
	}
	return dest;
}

void* memcpy_32_to_15(void* dest, const void* src, size_t n) {
	unsigned short sh = 0;
	size_t nn = n / 4;
	unsigned short *dp = (unsigned short*)dest;
	const unsigned int *sp = (const unsigned int*)src; dest;
	while (nn--) {
		*dp++ = (unsigned short)(((*sp & 0xFF) >> 3) | (((*sp & 0xFF00) >> 11) << 5) | (((*sp & 0xFF0000) >> 19) << 10));
		sp++;
	}
	return dest;
}

/*void *memcpy_4(void *dest, const void *src, size_t n) {
	size_t nn = n / 4;
	unsigned int *dp = (unsigned int*)dest;
	const unsigned int *sp = (const unsigned int*)src;
	while (nn--)
		//if (*dp != *sp)
			*dp++ = *sp++;
		//else
			//dp++, sp++;
	return dest;
}*/

void *memcpy_skip0(void *dest, const void *src, size_t n) {
	char *dp = (char*)dest;
	const char *sp = (const char*)src;
	while (n--) {
		if (n % 4 == 0)
			sp++;
		else {
			//if (*dp != *sp)
				*dp++ = *sp++;
			//else
				//dp++, sp++;
		}
	}
	return dest;
}

void fb_flip(void) {
	//if (console_mode == CONSOLE_VBOX)			// If you click the close button in VBox but don't shut down the VM
	//	vbox_enable_display();					// or save its state, you need to re-enable the display.
	unsigned long long t;


	if (currently_flipping)
		return;

	t = __rdtsc();

	currently_flipping = true;

	int x = console_cursor_x * fb_font_width;
	int y = console_cursor_y * fb_font_height;

	int cy;
	for (cy = fb_font_height - 2; cy < fb_font_height; cy++) {
		fb_modified[y + cy] = true;
		fb_modified[y + cy - (y >= fb_font_height ? fb_font_height : 0)] = true;
	}

	for (int i = 0; i < fb_gfx_y; i++)
		if (fb_modified[i]) {
			//memcpy_4((void*)((unsigned int)fb_triplebuffer+fb_gfx_x*4*i), (void*)((unsigned int)fb_doublebuffer+fb_gfx_x*4*i), fb_gfx_x * 4);
		}

	if (fb_gfx_depth == 32) {
		//memcpy_4(fb_framebuffer, fb_triplebuffer, fb_gfx_x * fb_gfx_y * 4);
		if (fb_scrolling) {
			// simd_memcpy_mmx
			simd_memcpy_mmx(fb_framebuffer, fb_doublebuffer, fb_gfx_y * fb_gfx_x * 4);
			fb_scrolling = false;
			for (int i = 0; i < fb_gfx_y; i++)
				fb_modified[i] = false;
			
			t = (__rdtsc() - t);
			debug_printf(UART_BASE_RS0, "Scroll time (clock cycles): %u\n", (unsigned int)t);
			currently_flipping = false;
			return;
		}

		for (int i = 0; i < fb_gfx_y; i++) {
			if (fb_modified[i]) {
				simd_memcpy_mmx((void*)((unsigned int)fb_framebuffer+fb_gfx_x*4*i), (void*)((unsigned int)fb_doublebuffer+fb_gfx_x*4*i), fb_gfx_x * 4);
				fb_modified[i] = false;
			}
		}
	}

	else if (fb_gfx_depth == 24) {
		//memcpy_skip0(fb_framebuffer, fb_triplebuffer, fb_gfx_x * fb_gfx_y * 4);
		if (fb_scrolling) {
			memcpy_skip0(fb_framebuffer, fb_doublebuffer, fb_gfx_y * fb_gfx_x * 4);
			fb_scrolling = false;
			for (int i = 0; i < fb_gfx_y; i++)
				fb_modified[i] = false;
			t = (__rdtsc() - t);
			debug_printf(UART_BASE_RS0, "Scroll time (clock cycles): %u\n", (unsigned int)t);
			currently_flipping = false;
			return;
		}

		for (int i = 0; i < fb_gfx_y; i++) {
			if (fb_modified[i]) {
				memcpy_skip0((void*)((unsigned int)fb_framebuffer+fb_gfx_x*3*i), (void*)((unsigned int)fb_doublebuffer+fb_gfx_x*4*i), fb_gfx_x * 4);
				fb_modified[i] = false;
			}
		}
	}

	else if (fb_gfx_depth == 16) {
		//memcpy_skip0(fb_framebuffer, fb_triplebuffer, fb_gfx_x * fb_gfx_y * 4);
		if (fb_scrolling) {
			memcpy_32_to_16(fb_framebuffer, fb_doublebuffer, fb_gfx_y * fb_gfx_x * 4);
			fb_scrolling = false;
			for (int i = 0; i < fb_gfx_y; i++)
				fb_modified[i] = false;
			
			t = (__rdtsc() - t);
			debug_printf(UART_BASE_RS0, "Scroll time (clock cycles): %u\n", (unsigned int)t);
			currently_flipping = false;
			return;
		}

		for (int i = 0; i < fb_gfx_y; i++) {
			if (fb_modified[i]) {
				memcpy_32_to_16((void*)((unsigned int)fb_framebuffer+fb_gfx_x*2*i), (void*)((unsigned int)fb_doublebuffer+fb_gfx_x*4*i), fb_gfx_x * 4);
				fb_modified[i] = false;
			}
		}
	}

	else if (fb_gfx_depth == 15) {
		//memcpy_skip0(fb_framebuffer, fb_triplebuffer, fb_gfx_x * fb_gfx_y * 4);
		if (fb_scrolling) {
			memcpy_32_to_15(fb_framebuffer, fb_doublebuffer, fb_gfx_y * fb_gfx_x * 4);
			fb_scrolling = false;
			for (int i = 0; i < fb_gfx_y; i++)
				fb_modified[i] = false;
			
			t = (__rdtsc() - t);
			debug_printf(UART_BASE_RS0, "Scroll time (clock cycles): %u\n", (unsigned int)t);
			currently_flipping = false;
			return;
		}

		for (int i = 0; i < fb_gfx_y; i++) {
			if (fb_modified[i]) {
				memcpy_32_to_15((void*)((unsigned int)fb_framebuffer+fb_gfx_x*2*i), (void*)((unsigned int)fb_doublebuffer+fb_gfx_x*4*i), fb_gfx_x * 4);
				fb_modified[i] = false;
			}
		}
	}
			
	for (int i = 0; i < fb_gfx_y; i++)
		fb_modified[i] = false;

	
	if (pit_ticks % 60 > 30)
		fb_update_cursor(true);
	else
		fb_update_cursor(false);

	t = (__rdtsc() - t);
	
	if (pit_ticks % 60 == 0)
		debug_printf(UART_BASE_RS0, "Refresh time (clock cycles): %u\n", (unsigned int)t);


	//for (int i = 0; i < fb_gfx_x * fb_gfx_y; i++)
		//((unsigned int*)fb_framebuffer)[i] = fb_doublebuffer[i];
	
	fb_scrolling = false;
	currently_flipping = false;
}

void fb_putpixel_triple(int x, int y, unsigned int c) {
	((unsigned int*)fb_triplebuffer)[y*fb_gfx_x + x] = c;
	fb_modified[y] = true;
}

void fb_putpixel_direct(int x, int y, unsigned int c) {			// xxx - does not do blending
	if (fb_gfx_depth == 32) {
		((unsigned int*)fb_framebuffer)[y*fb_gfx_x + x] = c;
	} else if (fb_gfx_depth == 24) {
		((unsigned char*)fb_framebuffer)[(y*fb_gfx_x + x)*3] = c & 0xFF;
		((unsigned char*)fb_framebuffer)[(y*fb_gfx_x + x)*3 + 1] = c & 0xFF00;
		((unsigned char*)fb_framebuffer)[(y*fb_gfx_x + x)*3 + 2] = c & 0xFF0000;
	} else if (fb_gfx_depth == 16) {
		((unsigned short*)fb_framebuffer)[y*fb_gfx_x + x] = (unsigned short)(((c & 0xFF) >> 3) | (((c & 0xFF00) >> 10) << 5) | (((c & 0xFF0000) >> 19) << 11));
	}
}

void fb_putpixel(int x, int y, unsigned int c) {
	unsigned int src = c, dest;
	unsigned int src_alpha, src_red, src_green, src_blue, dest_alpha, dest_red, dest_green, dest_blue;
	if ((src >> 24) == 0xFF)
		fb_doublebuffer[y*fb_gfx_x + x] = src;
	else {
		dest = fb_doublebuffer[y*fb_gfx_x + x];
		src_alpha = (src >> 24) & 0xFF;
		src_red = (src >> 16) & 0xFF;
		src_green = (src >> 8) & 0xFF;
		src_blue = src & 0xFF;
		dest_red = (dest >> 16) & 0xFF;
		dest_green = (dest >> 8) & 0xFF;
		dest_blue = dest & 0xFF;

		dest_red = (src_alpha * (src_red-dest_red) >> 8) + dest_red;
		dest_green = (src_alpha * (src_green-dest_green) >> 8) + dest_green;
		dest_blue = (src_alpha * (src_blue-dest_blue) >> 8) + dest_blue;
		
		fb_doublebuffer[y*fb_gfx_x + x] = dest_blue | (dest_green << 8) | (dest_red << 16) | 0xFF000000;
	}
	fb_modified[y] = true;
}

void fb_font_set(int width, int height, void* data) {
	fb_font_width = width;						// xxx - get this the right way (font format mebbe)
	fb_font_height = height;
	fb_font = (unsigned char*)data;

	console_width = fb_gfx_x / fb_font_width;
	console_height = fb_gfx_y / fb_font_height;
}

void fb_update_setup(void) {
	switch (console_mode) {
		case CONSOLE_VGA:
			return;								// What the fuck are we doing here if the console's VGA?
		case CONSOLE_BOCHS:
			fb_gfx_mode = bochs_gfx_mode;
			fb_gfx_x = bochs_gfx_x;
			fb_gfx_y = bochs_gfx_y;
			fb_gfx_depth = bochs_gfx_depth;
			fb_framebuffer = bochs_framebuffer;
			break;
		case CONSOLE_VBOX:
			fb_gfx_mode = vbox_gfx_mode;
			fb_gfx_x = vbox_gfx_x;
			fb_gfx_y = vbox_gfx_y;
			fb_gfx_depth = vbox_gfx_depth;
			fb_framebuffer = vbox_framebuffer;
			break;
		case CONSOLE_VBE:
			fb_gfx_mode = vbe_gfx_mode;
			fb_gfx_x = vbe_gfx_x;
			fb_gfx_y = vbe_gfx_y;
			fb_gfx_depth = vbe_gfx_depth;
			fb_framebuffer = vbe_framebuffer;
			break;
	}

	fb_font_set(DEFAULT_FONT_WIDTH, DEFAULT_FONT_HEIGHT, DEFAULT_FONT_DATA);

	if (fb_doublebuffer)
		free(fb_doublebuffer);
	if (fb_modified)
		free(fb_modified);

	//fb_doublebuffer = (unsigned int*)fb_framebuffer;
	fb_doublebuffer = (unsigned int*)malloc(fb_gfx_x * fb_gfx_y * 4);

	fb_modified = (bool*)malloc(fb_gfx_y * sizeof(bool));
	for (int i = 0; i < fb_gfx_y; i++)
		fb_modified[i] = true;
}

void fb_update_cursor(bool draw) {
	unsigned int fgcolour;
	if (console_palette == PALETTE_EGA)
		fgcolour = fb_clut[console_attrib & 0x0F];
	else if (console_palette == PALETTE_XTERM)
		fgcolour = xterm_palette[console_fg];
	else if (console_palette == PALETTE_TRUECOLOUR)
		fgcolour = console_fg;
	int x = console_cursor_x * fb_font_width;
	int y = console_cursor_y * fb_font_height;

	int cx, cy;
	for (cy = fb_font_height - 2; cy < fb_font_height; cy++) {
		for (cx = 0; cx < fb_font_width; cx++) {
			if (draw)
				fb_putpixel_direct(x + cx, y + cy, fgcolour);
			//else
				//fb_putpixel_triple(x + cx, y + cy, fgcolour);
			//else
				fb_modified[y + cy] = true;

			fb_modified[y + cy - (y >= fb_font_height ? fb_font_height : 0)] = true;
		}
	}
}

void fb_clear_screen(void) {
	unsigned int bgcolour;
	if (console_palette == PALETTE_EGA)
		bgcolour = fb_clut[console_attrib >> 4];
	else if (console_palette == PALETTE_XTERM)
		bgcolour = xterm_palette[console_bg];
	else if (console_palette == PALETTE_TRUECOLOUR)
		bgcolour = console_bg;

	for (int i = 0; i < fb_gfx_x * fb_gfx_y; i++)
		fb_doublebuffer[i] = bgcolour;
	
	for (int i = 0; i < fb_gfx_y; i++)
		fb_modified[i] = true;
}

void fb_scroll(void) {
	unsigned int bgcolour;
	if (console_palette == PALETTE_EGA)
		bgcolour = fb_clut[console_attrib >> 4];
	else if (console_palette == PALETTE_XTERM)
		bgcolour = xterm_palette[console_bg];
	else if (console_palette == PALETTE_TRUECOLOUR)
		bgcolour = console_bg;

	//memcpy(fb_doublebuffer, (void*)((unsigned int)fb_doublebuffer+80*2),(fb_gfx_x * fb_gfx_y - fb_gfx_x * fb_font_height)*4);

	//memcpy_4
	//simd_memcpy_mmx
	simd_memcpy_mmx(fb_doublebuffer, (void*)((unsigned int)fb_doublebuffer+fb_gfx_x * fb_font_height * 4),(fb_gfx_x * fb_gfx_y - fb_gfx_x * fb_font_height)*4);

	//for (int i = 0; i < fb_gfx_x * fb_gfx_y - fb_gfx_x * fb_font_height; i++)
	//	fb_doublebuffer[i] = fb_doublebuffer[i+fb_gfx_x * fb_font_height];

	//for (int i = fb_gfx_x * fb_gfx_y - fb_gfx_x * fb_font_height; i < fb_gfx_x * fb_gfx_y; i++)
	//	fb_doublebuffer[i] = bgcolour;

	memset_4((void*)((unsigned int)fb_doublebuffer+(fb_gfx_x * fb_gfx_y - fb_gfx_x * fb_font_height)*4), bgcolour, (fb_gfx_x * fb_font_height)*4);

	for (int i = 0; i < fb_gfx_y; i++)
		fb_modified[i] = true;

	fb_scrolling = true;
}

void fb_putchar(char c) {
	unsigned int fgcolour;
	unsigned int bgcolour;

	if (console_palette == PALETTE_EGA)
		fgcolour = fb_clut[console_attrib & 0x0F];
	else if (console_palette == PALETTE_XTERM)
		fgcolour = xterm_palette[console_fg];
	else if (console_palette == PALETTE_TRUECOLOUR)
		fgcolour = console_fg;

	if (console_palette == PALETTE_EGA)
		bgcolour = fb_clut[console_attrib >> 4];
	else if (console_palette == PALETTE_XTERM)
		bgcolour = xterm_palette[console_bg];
	else if (console_palette == PALETTE_TRUECOLOUR)
		bgcolour = console_bg;

	int x = console_cursor_x * fb_font_width;
	int y = console_cursor_y * fb_font_height;

	int cx, cy;
	unsigned char* glyph = fb_font + (int)c * fb_font_height;
 
	for (cy=0; cy < fb_font_height; cy++) {
		for (cx=0; cx < fb_font_width; cx++) {
			fb_putpixel(x + cx, y + cy, glyph[cy] & mask[cx] ? fgcolour : bgcolour);
		}
	}
	//__asm cli;
	//fb_flip();
	//__asm sti;
}

void fb_draw_bitmap(int x, int y, int width, int height, char* data) {
	fb_draw_bitmap(x, y, width, height, 255, data);
}

void fb_draw_bitmap(int x, int y, int width, int height, int alpha, char* data) {
	char* bitmap_data = data;
	unsigned char pixel[4] = {0};
	for (int cy = 0; cy < height; cy++) {
		for (int cx = 0; cx < width; cx++) {
			pixel[0] = ((((bitmap_data[2] - 33) & 0x3) << 6) | ((bitmap_data[3] - 33)));
			pixel[1] = ((((bitmap_data[1] - 33) & 0xF) << 4) | ((bitmap_data[2] - 33) >> 2));
			pixel[2] = (((bitmap_data[0] - 33) << 2) | ((bitmap_data[1] - 33) >> 4));
			pixel[3] = (unsigned char)alpha;
			bitmap_data += 4;
			fb_putpixel(x + cx, y + cy, *((unsigned int*)pixel));
		}
	}
}