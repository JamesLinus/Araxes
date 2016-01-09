// BlacklightEVO kernel/hardware/timer.c -- x86 Programmable Interval Timer
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <printf.h>
#include <hardware/timer.h>

uint32_t timer_ticks;
uint32_t timer_frequency;
bool timer_initialized = false;
uint16_t timer_divisor;

// Initializes the PIT to a specified frequency and optionally resets the PIT tick counter.
void timer_initialize(int frequency, bool reset_ticks) {
	if (reset_ticks)
		timer_ticks = 0;

	// The lowest frequency the PIT can go is 18.2 Hz, a divisor of 65536 (represented as 0).
	if (frequency > 18) {
		timer_divisor = 1193180 / frequency;
		timer_frequency = frequency;
	} else {
		timer_divisor = 0;
		timer_frequency = 18;
	}

	// Reinitialize the PIT with the new divisor and set it to repeating mode.
	outb(0x43, 0x36);
	outb(0x40, (timer_divisor & 0xFF));
	outb(0x40, ((timer_divisor >> 8) & 0xFF));

	timer_initialized = true;
}

void isr_irq_timer(/*struct regs* regs*/) {
	if (!timer_initialized) {
		outb(0x20, 0x20);
		return;
	}
	timer_ticks++;

	/*if (CONSOLE_IS_FRAMEBUFFER) {
	 *	__asm cli;
	 *	fb_flip();
	 *	__asm sti;
	 *}
	 */
		
	outb(0x20, 0x20);
}
