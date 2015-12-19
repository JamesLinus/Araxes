// BlacklightEVO kernel/include/hardware/pci.h -- Peripheral Component Interconnect bus
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__HARDWARE__PCI_H
#define __KERNEL__INCLUDE__HARDWARE__PCI_H

#define PCI_ENUMERATION_DEBUG 0

#define PCI_IO_CONFIG_ADDRESS   0xCF8
#define PCI_IO_CONFIG_DATA      0xCFC
#define PCI_IO_ENABLE 0x80000000

#define PCI_MAX_DEVICES 256

#define PCI_DEVICE_FREE   0
#define PCI_DEVICE_EXISTS 1
#define PCI_DEVICE_LOADED 2

struct pci_device_info {
	unsigned int bus;
	unsigned int device;
	unsigned int function;
	unsigned short vendor_id;
	unsigned short device_id;
	
	unsigned int device_handled;
};

extern struct pci_device_info pci_devices[PCI_MAX_DEVICES];

unsigned int pci_config_read_dword(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset);
unsigned short pci_config_read_word(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset);
unsigned char pci_config_read_byte(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset);

void pci_config_write_dword(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned int data);
void pci_config_write_word(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned short data);
void pci_config_write_byte(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned char data);

void pci_enumerate(void);

#endif  // __KERNEL__INCLUDE__HARDWARE__PCI_H