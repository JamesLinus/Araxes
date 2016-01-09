// BlacklightEVO kernel/x86/acpi.c -- Advanced Configuration and Power Interface
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <x86/acpi.h>

void* acpi_rsdp = NULL;

void* acpi_get_rsdp(void) {
	int i = 0, j = 0;
	void* ebda = (void*)((*(unsigned short*)0x0000040E) << 4);
	void* rsdp = NULL;
	
	for (i = 0; i < 1024; i += 16) {
		if (!strncmp(ebda+i, "RSD PTR ", 8)) {
			kprintf("Found RSDP at 0x%08X.\n", ebda+i);
			rsdp = ebda+i;
		}
	}
	
	for (i = 0; i < 0x20000; i += 16) {
		if (!strncmp((void*)(0xE0000+i), "RSD PTR ", 8)) {
			kprintf("Found RSDP at 0x%08X.\n", 0xE0000+i);
			rsdp = 0xE0000+i;
		}
	}
	
	if (!rsdp) {
		kprintf("Did not find an RSDP.\n");
		return acpi_rsdp = NULL;
	}
	
	for (i = 0; i < 0x20; i++) {
		j += *(char*)(rsdp+i);
	}
	
	if (j &= 0xFF) {
		kprintf("RSDP checksum invalid (came out to %d.)\n", j);
		return acpi_rsdp = NULL;
	}
}