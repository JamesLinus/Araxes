// BlacklightEVO hardware\floppy.h -- quick and dirty floppy driver
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __HARDWARE__FLOPPY_H
#define __HARDWARE__FLOPPY_H

struct floppy_drive {
	int number;
	int spt;
	int heads;
	int size;
}

// Config
#define FLOPPY_BASE 0x03f0
#define FLOPPY_IRQ 6

// Registers
#define FLOPPY_REG_DOR 2
#define FLOPPY_REG_MSR 4
#define FLOPPY_REG_FIFO 5
#define FLOPPY_REG_CCR 7

// Commands
#define FLOPPY_CMD_SPECIFY 3
#define FLOPPY_CMD_WRITE_DATA 5
#define FLOPPY_CMD_READ_DATA 6
#define FLOPPY_CMD_RECALIBRATE 7
#define FLOPPY_CMD_SENSE_INTERRUPT 8
#define FLOPPY_CMD_SEEK 15

// Functions


// Variables
extern struct floppy_drive floppy_drives[2];

#endif	// __HARDWARE__FLOPPY_H