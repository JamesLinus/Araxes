// BlacklightEVO kernel/include/hardware/uart.h -- serial I/O
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__HARDWARE__UART_H
#define __KERNEL__INCLUDE__HARDWARE__UART_H

#define UART_BASE_RS0 0x3F8
#define UART_BASE_RS1 0x2F8
#define UART_BASE_RS2 0x3E8
#define UART_BASE_RS3 0x2E8

#define UART_SPEED_1200 96
#define UART_SPEED_2400 48
#define UART_SPEED_9600 12
#define UART_SPEED_19200 6
#define UART_SPEED_38400 3
#define UART_SPEED_115200 1

#define UART_PROTOCOL_7N1 0x02
#define UART_PROTOCOL_8N1 0x03

extern unsigned short serial_debugging;

void uart_init_serial(unsigned short port, unsigned short divisor, unsigned char protocol);
bool serial_received(unsigned short port); 
unsigned char read_serial(unsigned short port);
bool is_transmit_empty(unsigned short port); 
void write_serial(unsigned short port, char a);

void uart_print(unsigned short port, char* s);

#endif	// __KERNEL__INCLUDE__HARDWARE__UART_H
