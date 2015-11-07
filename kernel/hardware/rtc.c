// BlacklightEVO kernel/hardware/rtc.c -- real-time clock
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <hardware/rtc.h>

bool rtc_initialized = false;
bool rtc_bcd = true;

uint64_t last_tsc = 0;
uint64_t recent_tsc = 0;

#define leap_year(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

int month_lengths[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, };

// Change a BCD byte to a decimal integer.
int bcdb_to_dec(unsigned char bcd) {
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

// Resynchronize our timekeeping with the RTC's.
void rtc_synchronize(void) {
	int second, minute, hour, day, month, year, century, weekday;
	int days = 0;
	uint64_t new_time = 0;
	
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_SECOND);
	second = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_MINUTE);
	minute = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_HOUR);
	hour = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_DAY);
	day = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_MONTH);
	month = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_YEAR);
	year = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_CENTURY);
	century = inb(CMOS_PORT_DATA);
	outb(CMOS_PORT_INDEX, CMOS_REGISTER_WEEKDAY);
	weekday = inb(CMOS_PORT_DATA);
	
	// Convert from BCD to regular binary decimal format.
	if (rtc_bcd) {
		second = bcdb_to_dec(second);
		minute = bcdb_to_dec(minute);
		hour = bcdb_to_dec(hour);
		day = bcdb_to_dec(day);
		month = bcdb_to_dec(month);
		year = bcdb_to_dec(year);
		century = bcdb_to_dec(century);
	}
	
	// Make the year field the four digit year.
	year = year + century * 100;
	
	for (int i = 0; i < month-1; i++) {
		if (i == 1 && leap_year(year))
			days += 1;
		days += month_lengths[i];
	}
	
	while (year > 1970) {
		year--;
		days += (leap_year(year) ? 366 : 365);
	}
	days += day-1;
	
	new_time = 86400 * days;
	new_time += 3600 * hour;
	new_time += 60 * minute;
	new_time += second;
	
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
		rtc_bcd = false;				// I don't think I've ever seen a system set to binary mode, but if it does, handle it.
	else
		rtc_bcd = true;
	
	rtc_initialized = true;
	rtc_synchronize();
	kprintf("[RTC] New timestamp: %llu\n", time_get());
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
		if (time_get() % 180 == 0) {
			rtc_synchronize();
		}
		
		kprintf("Current timestamp: %llu - ", time_get());
		
		last_tsc = recent_tsc;
		recent_tsc = cpu_rdtsc();
		kprintf("Possible clock frequency: %llu Hz\n", recent_tsc-last_tsc);
	}
		
	outb(0xA0, 0x20);
	outb(0x20, 0x20);
}
