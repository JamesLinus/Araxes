// BlacklightEVO kernel/include/hardware/timer.h -- x86 Programmable Interval Timer
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__HARDWARE__TIMER_H
#define __KERNEL__INCLUDE__HARDWARE__TIMER_H

void timer_initialize(int frequency, bool reset_ticks);
void isr_irq_timer(/*struct regs* regs*/);

#endif	// __KERNEL__INCLUDE__HARDWARE__TIMER_H
