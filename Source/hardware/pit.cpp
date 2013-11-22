// BlacklightEVO hardware\pit.cpp -- Programmable Interval Timer
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\cpu.h"
#include "core\console.h"
#include "video\fb_text.h"
#include "hardware\pit.h"
#include "hardware\uart.h"

int pit_ticks, pit_frequency;
unsigned short pit_divisor;
bool pit_initialized = false;

unsigned long long last_tsc, tsc_difference;
unsigned long long tsc_diff[6] = {0};
double possible_clock_rate;



void pit_initialize(int frequency, bool reset_ticks) {
	if (reset_ticks)
		pit_ticks = 0;

	if (frequency > 18) {
		pit_divisor = 1193180 / frequency;
		pit_frequency = frequency;
	} else {
		pit_divisor = 0;
		pit_frequency = 18;
	}

	outb(0x43, 0x36);							// Command to reinitialize the PIT it repeating mode
	outb(0x40, (pit_divisor & 0xFF));
	outb(0x40, ((pit_divisor >> 8) & 0xFF));

	pit_initialized = true;
	last_tsc = __rdtsc();
}

void pit_interrupt(void) {
	if (!pit_initialized)
		return;
	pit_ticks++;
	tsc_difference = __rdtsc() - last_tsc;
	last_tsc = __rdtsc();						// Kinda? Close enough I guess.

	if (pit_ticks % 60 == 50)
		tsc_diff[5] = tsc_difference;
	if (pit_ticks % 60 == 40)
		tsc_diff[4] = tsc_difference;
	if (pit_ticks % 60 == 30)
		tsc_diff[3] = tsc_difference;
	if (pit_ticks % 60 == 20)
		tsc_diff[2] = tsc_difference;
	if (pit_ticks % 60 == 10)
		tsc_diff[1] = tsc_difference;
	if (pit_ticks % 60 == 00)
		tsc_diff[0] = tsc_difference;
	
	if (pit_ticks % 60 == 0) {
		possible_clock_rate = tsc_diff[0] + tsc_diff[1] + tsc_diff[2] + tsc_diff[3] + tsc_diff[4] + tsc_diff[5];
		possible_clock_rate /= 6;
		possible_clock_rate *= pit_frequency;
		possible_clock_rate /= 1000000;

		//debug_printf(UART_BASE_RS0, DEBUG_INFO "Possible clock rate: %f MHz; are we doing it right?\n", possible_clock_rate);
	}

	if (CONSOLE_IS_FRAMEBUFFER) {
		//__asm cli;
		fb_flip();
		//__asm sti;
	}

	/*if (pit_ticks % 18 == 0) {
		uart_print(UART_BASE_RS0, "Tick: ");
		uart_print(UART_BASE_RS0, itoa(pit_ticks, 10));
		uart_print(UART_BASE_RS0, "\r\n");
	}*/
}