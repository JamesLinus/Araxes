// BlacklightEVO kernel/printf.h -- kvsprintf and friends
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// kvsprintf supports the following specifiers:
//  - %s	null-terminated string
//  - %x	lowercase hexidecimal uint32_t
//  - %X	uppercase hexidecimal uint32_t
//  - %p	uppercase hexidecimal void* with 0x prefix
//  - %u	decimal uint32_t
//  - %d,i	decimal int32_t
//  - %l	decimal uint64_t (non-standard -- should be replaced by %llu)
//  - %c	single character

#include <global.h>
#include <printf.h>
#include <terminal.h>
#include <hardware/uart.h>

int kvsnprintf(char* str, size_t size, const char* fmt, va_list va) {
	char tmp[22];	// Enough to hold a 64-bit unsigned integer.
	char ch;
	char* buf = str;
	char* sztmp;
	int i;

	while ((ch = *(fmt++))) {
		if ((size_t)(buf - str) == size-1)
			ch = 0;
		if (ch != '%') {
			*buf++ = ch;
		} else if (ch) {
			int w = 0;
			bool pf = false;
			int length = 0;
		nastygoto:
			ch = *(fmt++);
			if (ch == '#') {
				pf = true;
				ch = *(fmt++);
			}
			if (ch >= '0' && ch <= '9') {
				w = w * 10 + (ch - '0');
				goto nastygoto;
			}
			switch (ch) {
				case 'l':
					length++;
					goto nastygoto;
				case 'u':
					i = 0;
					if (length < 2)
						uitoa(tmp, va_arg(va, unsigned int), 10, w);
					else
						u64toa(tmp, va_arg(va, uint64_t), 10, w);
					while (tmp[i])
						*buf++ = tmp[i++];
					break;
				case 'd':
				case 'i':
					i = 0;
					itoa(tmp, va_arg(va, int), 10, w);
					while (tmp[i])
						*buf++ = tmp[i++];
					break;
				case 'x':
				case 'X':
					i = 0;
					if (length < 2)
						uitoa(tmp, va_arg(va, unsigned int), 16, w);
					else
						u64toa(tmp, va_arg(va, uint64_t), 16, w);
					if (pf) {
						*buf++ = '0';
						*buf++ = (ch == 'x' ? 'x' : 'X');
					}
					while (tmp[i]) {
						*buf++ = (ch == 'x' && tmp[i] <= 'F' && tmp[i] >= 'A') ? tmp[i] + 32 : tmp[i];
						i++;
					}
					break;
				case 'p':
					i = 0;
					uitoa(tmp, va_arg(va, unsigned int), 16, w);
					*buf++ = '0';
					*buf++ = 'x';
					while (tmp[i])
						*buf++ = tmp[i++];
					break;
				case 'c':
					*buf++ = (char)(va_arg(va, int));
					break;
				case 's':
					i = 0;
					sztmp = va_arg(va, char*);
					while (sztmp[i])
						*buf++ = sztmp[i++];
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
	ret = kvsnprintf(buf, 2048, fmt, va);
	va_end(va);
	
	console_print(buf);
	free(buf);
	return ret;
}

int ksnprintf(char* s, size_t size, const char *fmt, ...) {
	int ret;
	va_list va;
	va_start(va,fmt);
	ret = kvsnprintf(s, size, fmt, va);
	va_end(va);
	return ret;
}

int _debug_printf(const char* fmt, ...) {
	//char buf[2048];
	char* buf = (char*)malloc(2048);
	int ret;
	memset(buf, 0, 2048);

	va_list va;
	va_start(va, fmt);

	ret = kvsnprintf(buf, 2048, fmt, va);
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

	kvsnprintf(buf, 2048, fmt, args);

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


