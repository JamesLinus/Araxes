// BlacklightEVO hardware\uart.cpp -- serial I/O
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\console.h"
#include "core\mm.h"
#include "hardware\uart.h"
#include <stdarg.h>

void init_serial(unsigned short port, unsigned short divisor, unsigned char protocol) {
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, divisor & 0xFF);
	outb(port + 1, divisor >> 8);
	outb(port + 3, protocol);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
}

bool serial_received(unsigned short port) {
	return (inb(port + 5) & 1) ? true : false;
}
 
unsigned char read_serial(unsigned short port) {	// xxx - non-blocking version
	while (!serial_received(port));
	return inb(port);
}

bool is_transmit_empty(unsigned short port) {
	return (inb(port + 5) & 0x20) ? true : false;
}
 
void write_serial(unsigned short port, char a) {	// xxx - non-blocking version
	if (a == '\n')
		write_serial(port, '\r');

	while (!is_transmit_empty(port));
	outb(port, a);
}


void uart_print(unsigned short port, char* s) {
	while (*s)
		write_serial(port, *s++);
}