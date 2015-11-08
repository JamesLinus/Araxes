// BlacklightEVO kernel/hardware/timer.c -- x86 Programmable Interval Timer
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <printf.h>
#include <hardware/timer.h>

unsigned int timer_ticks;
unsigned int timer_frequency;
bool timer_initialized = false;
unsigned short timer_divisor;

void timer_initialize(int frequency, bool reset_ticks) {
	if (reset_ticks)
		timer_ticks = 0;

	if (frequency > 18) {
		timer_divisor = 1193180 / frequency;
		timer_frequency = frequency;
	} else {
		timer_divisor = 0;
		timer_frequency = 18;
	}

	outb(0x43, 0x36);							// Command to reinitialize the PIT it repeating mode
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

	//if (CONSOLE_IS_FRAMEBUFFER) {
		//__asm cli;
		//fb_flip();
		//__asm sti;
	//}
		
	outb(0x20, 0x20);
}
