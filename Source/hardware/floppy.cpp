// BlacklightEVO hardware\floppy.h -- quick and dirty floppy driver
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "hardware\floppy.h"

struct floppy_drive floppy_drives[2];

void floppy_detect_drives(void) {
	outb(0x70, 0x10);
	unsigned char drives = inb(0x71);
	floppy_drives[0].size = drives >> 4;
	floppy_drives[1].size = drives & 0x0F;
}

void lba_hts(int drive, int lba, int *head, int *track, int *sector) {
	*head = (lba % (floppy_drives[drive].spt * floppy_drives[drive].heads)) / (floppy_drives[drive].spt);
	*track = lba / (floppy_drives[drive].spt * floppy_drives[drive].heads);
	*sector = lba % floppy_drives[drive].spt + 1;
}