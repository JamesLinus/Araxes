// BlacklightEVO kernel/printf.h -- kvsprintf and friends
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <printf.h>
#include <terminal.h>
#include <hardware/uart.h>

int kvsprintf(char* str, const char* fmt, va_list va) {
	char bf[12];
	char ch;
	char* buf = str;
	char* ss;
	int i;

	while ((ch = *(fmt++))) {
		if (ch != '%') {
			*buf++ = ch;
		} else if (ch) {
			int w = 0;
			bool pf = false;
			ch = *(fmt++);
			if (ch == '#') {
				pf = true;
				ch = *(fmt++);
			}
			if (ch >= '0' && ch <= '9') {
				w = ch - '0';
				ch = *(fmt++);
			}
			switch (ch) {
				case 'u':
					i = 0;
					uitoa(bf, va_arg(va, unsigned int), 10, w);
					while (bf[i])
						*buf++ = bf[i++];
					break;
				case 'd':
					i = 0;
					itoa(bf, va_arg(va, int), 10, w);
					while (bf[i])
						*buf++ = bf[i++];
					break;
				case 'x':
				case 'X':
					i = 0;
					itoa(bf, va_arg(va, unsigned int), 16, w);
					if (pf) {
						*buf++ = '0';
						*buf++ = 'x';
					}
					while (bf[i])
						*buf++ = bf[i++];
					break;
				case 'c':
					*buf++ = (char)(va_arg(va, int));
					break;
				case 's':
					i = 0;
					ss = va_arg(va, char*);
					while (ss[i])
						*buf++ = ss[i++];
					break;
				case '%':
					*buf++ = ch;
				default:
					break;
			}
		} else {
			*buf++ = 0;
		}
	}
	return buf - str;
}
	
int kprintf(const char *fmt, ...) {
	//char buf[2048];
	char* buf = (char*)malloc(2048);
	int ret;
	memset(buf, 0, 2048);
	va_list va;
	va_start(va, fmt);
	ret = kvsprintf(buf, fmt, va);
	va_end(va);
	
	console_print(buf);
	free(buf);
	return ret;
}

int ksprintf(char* s, const char *fmt, ...) {
	int ret;
	va_list va;
	va_start(va,fmt);
	ret = kvsprintf(s, fmt, va);
	va_end(va);
	return ret;
}

int debug_printf(const char* fmt, ...) {
	//char buf[2048];
	char* buf = (char*)malloc(2048);
	int ret;
	memset(buf, 0, 2048);

	va_list va;
	va_start(va, fmt);

	ret = kvsprintf(buf, fmt, va);
	if (serial_debugging)
		uart_print(serial_debugging, buf);

	va_end(va);
	free(buf);
	return ret;
}

void crash(char* file, int line, const char* fmt, ...) {
	char buf[2048], bc[12];
	memset(buf, 0, 2048);
	memset(bc, 0, 12);
	asm volatile("cli");
	va_list args;
	va_start(args, fmt);

	kvsprintf(buf, fmt, args);

	va_end(args);

	//debug_printf(UART_BASE_RS0, DEBUG_FATAL "crash()!: %s", buf);

	current_terminal->palette = PALETTE_EGA;		// Swap the palette back to EGA, since we're most likely either in xterm or truecolour
	current_terminal->color = 0x4E;					// Bright yellow on red is a good indicator something's fucked up...
	itoa(bc, line, 10, 1);
	console_print("\nRSoD: crash() called at ");
	console_print(file);
	console_print(":");
	console_print(bc);
	console_print(": ");

	console_print(buf);

	//if (CONSOLE_IS_FRAMEBUFFER)
	//	fb_flip();									// Flip the framebuffer so we can actually display our message

	_crash();
}


