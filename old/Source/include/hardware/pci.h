// BlacklightEVO hardware\pci.h -- Peripheral Component Interconnect interface
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __HARDWARE__PCI_H
#define __HARDWARE__PCI_H

#include "global.h"

#define PCI_REG_ADDRESS	0xCF8
#define PCI_REG_DATA	0xCFC

#define PCI_BAR0	0x10
#define PCI_BAR1	0x14
#define PCI_BAR2	0x18
#define PCI_BAR3	0x1C
#define PCI_BAR4	0x20
#define PCI_BAR5	0x24

typedef struct {
	unsigned char bus, device, function;
} pci_bdf;

unsigned char pci_read_config_byte(pci_bdf* bdf, unsigned char reg);
unsigned short pci_read_config_word(pci_bdf* bdf, unsigned char reg);
unsigned int pci_read_config_dword(pci_bdf* bdf, unsigned char reg);
void pci_write_config_byte(pci_bdf* bdf, unsigned char reg, unsigned char val);
void pci_write_config_word(pci_bdf* bdf, unsigned char reg, unsigned short val);
void pci_write_config_dword(pci_bdf* bdf, unsigned char reg, unsigned int val);

int pci_detect(void);

bool pci_exists(unsigned short vendor, unsigned short device);
pci_bdf* pci_get_bdf(unsigned short vendor, unsigned short device);

void pci_dump(void);

#endif	// __HARDWARE__PCI_H