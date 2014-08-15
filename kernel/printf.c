// BlacklightEVO kernel/printf.h -- kvsprintf and friends
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <printf.h>

int kvsprintf(char* str, char* fmt, va_list va) {
	char bf[12];
	char ch, cj;
	char* buf = str;
	char* ss;
	int i;

	while ((ch = *(fmt++))) {
		if (ch != '%') {
			*buf++ = ch;
		} else if (ch) {
			int w = 0;
			ch = *(fmt++);
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
	
int kprintf(char *fmt, ...) {
	char buf[2048];
	int ret;
	memset(buf, 0, 2048);
	va_list va;
	va_start(va,fmt);
	ret = kvsprintf(buf, fmt, va);
	va_end(va);
	
	console_print(buf);
	return ret;
}

int ksprintf(char* s,char *fmt, ...) {
	int ret;
	va_list va;
	va_start(va,fmt);
	ret = kvsprintf(s, fmt, va);
	va_end(va);
	return ret;
}


