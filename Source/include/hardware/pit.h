// BlacklightEVO hardware\irq.h -- Programmable Interval Timer
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __HARDWARE__PIT_H
#define __HARDWARE__PIT_H

void pit_initialize(int frequency, bool reset_ticks);
extern int pit_ticks, pit_frequency;

void pit_interrupt(void);

#endif  // __HARDWARE__PIT_H