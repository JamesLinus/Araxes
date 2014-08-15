// BlacklightEVO hardware\pcicodes.cpp -- PCI device and vendor mpping
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "hardware\pcicodes.h"

// struct pci_vendor_info {
//		unsigned short id;
//		char* name;
//		char* long_name;
// };
struct pci_vendor_info pci_vendor_list[] = {
	{ 0x1002, "ATI", "ATI Technologies Inc." },
	{ 0x1022, "AMD", "Advanced Micro Devices, Inc." },
	{ 0x106B, "Apple", "Apple Inc." },
	{ 0x8086, "Intel", "Intel Corporation" },
	{ 0x80EE, "Oracle", "Oracle Corporation" },
};

// struct pci_device_info {
//		unsigned short vendor;
//		unsigned short id;
//		char* name;
// };
struct pci_device_info pci_device_list[] = {
	{ 0x8086, 0x1237, "82440LX PCI Motherboard" },
	{ 0x8086, 0x7000, "PIIX3 PCI-to-ISA Bridge" },
	{ 0x8086, 0x7111, "PIIX4/4E/4M IDE Controller" },
	{ 0x80EE, 0x7145, "VirtualBox Display Adapter" },
	{ 0x80EE, 0xBEEF, "VirtualBox Display Adapter" },
	{ 0x80EE, 0xCAFE, "VirtualBox Guest Services" },
};

int pci_vendor_info_length = (sizeof(pci_vendor_list)/sizeof(pci_vendor_info));
int pci_device_info_length = (sizeof(pci_device_list)/sizeof(pci_device_info));