// BlacklightEVO kernel/include/hardware/rtc.h -- real-time clock
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__HARDWARE__RTC_H
#define __KERNEL__INCLUDE__HARDWARE__RTC_H

#define CMOS_PORT_INDEX 0x70
#define CMOS_PORT_DATA 0x71

#define CMOS_NMI_DISABLE 0x80

#define CMOS_REGISTER_SECOND	0x00
#define CMOS_REGISTER_MINUTE	0x02
#define CMOS_REGISTER_HOUR		0x04
#define CMOS_REGISTER_WEEKDAY	0x06
#define CMOS_REGISTER_DAY		0x07
#define CMOS_REGISTER_MONTH		0x08
#define CMOS_REGISTER_YEAR		0x09
#define CMOS_REGISTER_CENTURY	0x32

#define CMOS_REGISTER_STATUS_A	0x0A
#define CMOS_REGISTER_STATUS_B	0x0B
#define CMOS_REGISTER_STATUS_C	0x0C

#define CMOS_INTERRUPT_ANY		0x80
#define CMOS_INTERRUPT_PERIODIC	0x40
#define CMOS_INTERRUPT_ALARM	0x20
#define CMOS_INTERRUPT_UPDATE	0x10

void rtc_initialize(void);
void isr_irq_rtc(/*struct regs* regs*/);

#endif	// __KERNEL__INCLUDE__HARDWARE__RTC_H
