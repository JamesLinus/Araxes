// BlacklightEVO kernel/hardware/uart.c -- serial I/O
// Copyright (c) 2013-2015 The Cordilon Group
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <hardware/uart.h>

// TODO: This driver is not good. It is completely blocking and doesn't make good use of the FIFOs
// on 16550A UARTs. It needs to be rewritten as an interrupt-driven, FIFO-aware driver sometime
// before serial login consoles are added to the OS.

// Initialize a UART, setting the baud rate divisor and bitstream protocol for the selected port.
void uart_init_serial(unsigned short port, unsigned short divisor, unsigned char protocol) {
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, divisor & 0xFF);
	outb(port + 1, divisor >> 8);
	outb(port + 3, protocol);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
}

// Checks the status of a port's receive buffer.
bool serial_received(unsigned short port) {
	return (inb(port + 5) & 1) ? true : false;
}

// Recieves a single byte from a serial port.
unsigned char read_serial(unsigned short port) {
	while (!serial_received(port));
	return inb(port);
}

// Checks the status of a port's transmit buffer.
bool is_transmit_empty(unsigned short port) {
	return (inb(port + 5) & 0x20) ? true : false;
}

// Transmits a single byte over a serial port.
void write_serial(unsigned short port, char a) {
	if (a == '\n')
		write_serial(port, '\r');

	while (!is_transmit_empty(port));
	outb(port, a);
}

// Dumps a string to a serial port.
void uart_print(unsigned short port, char* s) {
	while (*s)
		write_serial(port, *s++);
}
