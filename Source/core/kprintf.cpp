// BlacklightEVO core\kprintf.cpp -- so much variable argument love
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\console.h"
#include "core\mm.h"
#include "hardware\uart.h"
#include "video\fb_text.h"
#include <stdarg.h>

int printf(const char* fmt, ...) {
	//char b[1024];
	char* buf = (char*)malloc(4096);
	//char* buf = b;
	memset(buf, 0, 4096);
	//char buf[1024];
	if (!buf)
		panic(__FILE__,__LINE__,"Something fucked up in printf.");

	va_list args;
	va_start(args, fmt);

	vsprintf(buf, fmt, args);
	console_print(buf);

	va_end(args);
	free(buf);
	return strlen(buf);
}

int debug_printf(unsigned short port, const char* fmt, ...) {
	char* buf = (char*)malloc(1024);

	va_list args;
	va_start(args, fmt);

	vsprintf(buf, fmt, args);
	if (serial_debugging)
		uart_print(port, buf);

	va_end(args);
	free(buf);
	return strlen(buf);
}

void panic(char* file, int line, const char* fmt, ...) {
	char buf[1024];
	__asm cli;
	va_list args;
	va_start(args, fmt);

	vsprintf(buf, fmt, args);

	va_end(args);

	debug_printf(UART_BASE_RS0, DEBUG_FATAL "panic()!: %s", buf);

	console_palette = PALETTE_EGA;				// Swap the palette back to EGA, since we're most likely either in xterm or truecolour
	console_attrib = 0x4E;						// Bright yellow on red is a good indicator something's fucked up...
	console_print("\nRSoD: panic() called at ");
	console_print(file);
	console_print(":");
	console_print(itoa(line, 10));
	console_print(": ");

	console_print(buf);

	if (CONSOLE_IS_FRAMEBUFFER)
		fb_flip();									// Flip the framebuffer so we can actually display our message

	__asm {
	halt:
		hlt
		jmp halt
	}
}

int vsprintf(char* str, const char* fmt, va_list arg) {
	int i, min;
	// float f;
	double d;
	bool prefix;
	const char* p = fmt;
	char* buf = str;
	char* tmp, *tmp2;
	int precision = 6;

	prefix = false;
	min = 1;

	//va_arg(arg, char*);

	while (*p) {
		if (*p != '%') {
			*str = *p;
			str++;
		} else {
			p++;
			prefix = false;
			min = 1;

nasty_goto:										// xxx - "don't use goto unless you really need to" kettle, this is pot, you're black
			switch (*p)
			{
				case '.':
					p++;
					switch (*p) {
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
							precision = 0;
							while('0' <= *p && *p <= '9') {
								precision *= 10;
								precision += *p - '0';
								p++;
							}
							goto nasty_goto;
						default:
							precision = 6;
							break;
					}
				case '#':
					prefix = true;
					p++;
					goto nasty_goto;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					min = 0;
					while('0' <= *p && *p <= '9') {
						min *= 10;
						min += *p - '0';
						p++;
					}
					goto nasty_goto;
				case 'c':
					*str = (char)va_arg(arg, int);
					str++;
					break;
				case 'd':
					i = va_arg(arg, int);
					if (i < 0) {
						i = -i;
						*str = '-';
						str++;
					}
					tmp = itoa(i, 10, min);
					while (*tmp)
						*str++ = *tmp++;
					break;
				case 'f':
					d = va_arg(arg, double);
					tmp = dtoa(d, precision);
					while (*tmp)
						*str++ = *tmp++;
					break;
				/*case 'l':
					tmp = lltoa(va_arg(arg, unsigned long long), 10, min);
					while (*tmp)
						*str++ = *tmp++;
					break;*/
				case 'o':
					if (prefix) {
						*str = '0';
						str++;
					}
					tmp = itoa(va_arg(arg, unsigned int), 8, min);
					while (*tmp)
						*str++ = *tmp++;
					break;
				case 's':
					tmp = va_arg(arg, char*);
					tmp2 = (char*)malloc(512);
					strncpy(tmp2, tmp, 511);
					//while (*tmp2)
					//	*str++ = *tmp2++;
					strcat(buf, tmp2);
					str += strlen(tmp2);
					free(tmp2);
					break;
				case 'u':
					tmp = itoa(va_arg(arg, unsigned int), 10, min);
					while (*tmp)
						*str++ = *tmp++;
					break;
				case 'x':
				case 'X':
					if (prefix) {
						*str = '0';
						str++;
						*str = 'x';
						str++;
					}
					tmp = itoa(va_arg(arg, unsigned int), 16, min);
					while (*tmp)
						*str++ = *tmp++;
					break;
				default:
					*str = *p;
					str++;
					break;
			}
		}
		p++;
	}
	*str = '\0';

	return strlen(buf);
}