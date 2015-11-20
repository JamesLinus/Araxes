// BlacklightEVO tools/evofstools/offsets.c -- offset tester
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char* minimum_size(unsigned int sectors) {
	static char sizestr[16] = {};
	double d;
	int divisor = 0;
	const char* sizediv;
	
	d = (double)sectors;
	d *= 512;
	while (d >= 1024) {
		d /= 1024;
		divisor++;
	}
	
	switch (divisor) {
		case 0:
			sizediv = "B";
			break;
		case 1:
			sizediv = "KiB";
			break;
		case 2:
			sizediv = "MiB";
			break;
		case 3:
			sizediv = "GiB";
			break;
		case 4:
			sizediv = "TiB";
			break;
	}
	
	sprintf(sizestr, "%.2f %s", d, sizediv);
	return sizestr;
}

int main(int argc, char* argv[]) {
	int verbose = 0;
	int i = 1;
	int j = 0;
	for (; i < argc; i++) {
		if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
			verbose = 1;
			
		j = strtol(argv[i], NULL, 0);
	}
	unsigned int volume_sectors = 0;
	unsigned int offset_vbr = 0x00000000;
	unsigned int offset_stage2 = 0x00000001;
	unsigned int offset_reserved = 0x0000003D;
	unsigned int offset_blocktable = 0x00000040;
	unsigned int offset_data = 0;
	
	unsigned int size_blocktable = 0;
	unsigned int sectors_blocktable = 0;
	unsigned int size_data = 0;
	
	if (!j) {
		printf("How many sectors in the volume/partition? ");
		scanf("%u", &volume_sectors);
	} else {
		volume_sectors = j;
	}
	
	//printf("\nOkay, working with %u sectors (%.2f MiB).\n\n", volume_sectors, ((float)volume_sectors)*512/1024/1024);
	if (verbose)
		printf("\nOkay, working with %u sectors (%s).\n", volume_sectors, minimum_size(volume_sectors));
	
	//size_blocktable = volume_sectors/8 + ((volume_sectors % 8) ? 1 : 0);
	size_blocktable = volume_sectors;
	sectors_blocktable = size_blocktable/512 + ((size_blocktable % 512) ? 1 : 0);
	offset_data = offset_blocktable + sectors_blocktable;
	if (verbose)
		printf("Blocktable is %u bytes long (%u sectors).\n\n", size_blocktable, sectors_blocktable);
	
	if (verbose) {
		printf("VBR offset:         0x%08X (len: %u sectors)\n", offset_vbr, offset_stage2-offset_vbr);
		printf("Stage2 offset:      0x%08X (len: %u sectors)\n", offset_stage2, offset_reserved-offset_stage2);
		printf("Reserved offset:    0x%08X (len: %u sectors)\n", offset_reserved, offset_blocktable-offset_reserved);
		printf("Blocktable offset:  0x%08X (len: %u sectors)\n", offset_blocktable, offset_data-offset_blocktable);
		printf("Data area offset:   0x%08X (len: %u sectors)\n\n", offset_data, volume_sectors-offset_data);
	}
	
	printf("Total partition size:      %s\n", minimum_size(volume_sectors));
	printf("Data area size:            %s\n", minimum_size(volume_sectors-offset_data));
	printf("Total filesystem overhead: %s (%u bytes) \n", minimum_size(offset_data), offset_data*512);
	printf("                           %f%%\n", ((float)offset_data)/volume_sectors*100);
	printf("\n");
	
	return 0;
}
