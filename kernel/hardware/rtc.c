// BlacklightEVO kernel/hardware/rtc.c -- real-time clock
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <hardware/rtc.h>

extern int current_weekday;

bool rtc_initialized = false;
bool rtc_bcd = true;

uint64_t last_tsc = 0;
uint64_t recent_tsc = 0;
uint64_t probable_clock_frequency = 0;

// Change a BCD byte to a decimal integer.
int bcdb_to_dec(unsigned char bcd) {
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

// Resynchronize our timekeeping with the RTC's.
void rtc_synchronize(void) {
	int century;
	int64_t new_time = 0;
	datetime_t dt;
	
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_SECOND);
	dt.second = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_MINUTE);
	dt.minute = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_HOUR);
	dt.hour = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_DAY);
	dt.day = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_MONTH);
	dt.month = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_YEAR);
	dt.year = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_CENTURY);
	century = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_WEEKDAY);
	dt.weekday = inb(CMOS_PORT_DATA);
	
	// Convert from BCD to regular binary decimal format.
	if (rtc_bcd) {
		dt.second = bcdb_to_dec(dt.second);
		dt.minute = bcdb_to_dec(dt.minute);
		dt.hour = bcdb_to_dec(dt.hour);
		dt.day = bcdb_to_dec(dt.day);
		dt.month = bcdb_to_dec(dt.month);
		dt.year = bcdb_to_dec(dt.year);
		century = bcdb_to_dec(century);
	}
	
	// Set the current day of the week.
	current_weekday = dt.weekday;
	
	// Make the year field the four digit year.
	dt.year = dt.year + century * 100;
	
	// Convert our datetime structure to a 64-bit timestamp and save it.
	new_time = time_timestamp(dt);
	time_set(new_time);
}

// This function is a bit of a misnomer. We don't actually reinitialize the
// RTC chip itself here. That would be bad, since we'd lose the system's
// time settings. Rather we initialize what we want the RTC chip to fire
// interrupts for and start keeping track of time.
void rtc_initialize(void) {
	unsigned char status = 0;
	
	asm volatile ("cli");
	
	outb(CMOS_PORT_INDEX, CMOS_NMI_DISABLE | CMOS_REGISTER_STATUS_B);
	status = inb(CMOS_PORT_DATA);
	status = status | CMOS_INTERRUPT_UPDATE;
	outb(CMOS_PORT_INDEX, CMOS_NMI_DISABLE | CMOS_REGISTER_STATUS_B);
	outb(CMOS_PORT_DATA, status);
	
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_STATUS_C);
	inb(CMOS_PORT_DATA);
	asm volatile ("sti");

	if (status & 0x04)
		rtc_bcd = false;			// I don't think I've ever seen a system set to binary mode, but if it does, handle it.
	else
		rtc_bcd = true;
	
	rtc_initialized = true;
	rtc_synchronize();
	//kprintf("[RTC] New timestamp: %llu\n", time_get());
	recent_tsc = cpu_rdtsc();
}

void isr_irq_rtc(/*struct regs* regs*/) {
	unsigned char status = 0;
	if (!rtc_initialized) {
		outb(0xA0, 0x20);
		outb(0x20, 0x20);
		return;
	}
	
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_STATUS_C);
	status = inb(CMOS_PORT_DATA);
	
	if (status & CMOS_INTERRUPT_UPDATE) {
		time_set(time_get() + 1);
		if (time_get() % 10 == 0) {
			rtc_synchronize();
			//kprintf("Probable clock frequency: %llu Hz", probable_clock_frequency);
		}
		
		//kprintf("Current timestamp: %lld\n", time_get());
		
		last_tsc = recent_tsc;
		recent_tsc = cpu_rdtsc();
		if (last_tsc)
			probable_clock_frequency = recent_tsc-last_tsc;
	}
		
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}
