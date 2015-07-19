// BlacklightEVO kernel/include/hardware/timer.h -- x86 Programmable Interval Timer
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__HARDWARE__TIMER_H
#define __KERNEL__INCLUDE__HARDWARE__TIMER_H

void timer_initialize(int frequency, bool reset_ticks);
void isr_irq_timer(/*struct regs* regs*/);

#endif	// __KERNEL__INCLUDE__HARDWARE__TIMER_H
