// BlacklightEVO hardware\pcicodes.h -- PCI device and vendor mpping
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __HARDWARE__PCICODES_H
#define __HARDWARE__PCICODES_H

struct pci_vendor_info {
	unsigned short id;
	char* name;
	char* long_name;
};

struct pci_device_info {
	unsigned short vendor;
	unsigned short id;
	char* name;
};

extern struct pci_vendor_info pci_vendor_list[];
extern struct pci_device_info pci_device_list[];

extern int pci_vendor_info_length;
extern int pci_device_info_length;

#endif	// __HARDWARE__PCICODES_H