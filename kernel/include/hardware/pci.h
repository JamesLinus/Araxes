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
	uint32_t bus;
	uint32_t device;
	uint32_t function;
	uint16_t vendor_id;
	uint16_t device_id;
	
	uint32_t device_handled;
};

extern struct pci_device_info pci_devices[PCI_MAX_DEVICES];

uint32_t pci_config_read_dword(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset);
uint16_t pci_config_read_word(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset);
uint8_t pci_config_read_byte(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset);

void pci_config_write_dword(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint32_t data);
void pci_config_write_word(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint16_t data);
void pci_config_write_byte(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint8_t data);

void pci_enumerate(void);

#endif  // __KERNEL__INCLUDE__HARDWARE__PCI_H