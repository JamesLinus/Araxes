// BlacklightEVO hardware\pci.cpp -- Peripheral Component Interconnect interface
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\console.h"
#include "core\mm.h"
#include "hardware\pci.h"
#include "hardware\pcicodes.h"

unsigned char pci_read_config_byte(pci_bdf* bdf, unsigned char reg) {
	outd(PCI_REG_ADDRESS, 0x80000000L | ((unsigned long)bdf->bus << 16) | ((unsigned)bdf->device << 11) | ((unsigned)bdf->function << 8) | (reg & ~3));
	return inb(PCI_REG_DATA + (reg & 3));
}

unsigned short pci_read_config_word(pci_bdf* bdf, unsigned char reg) {
	outd(PCI_REG_ADDRESS, 0x80000000L | ((unsigned long)bdf->bus << 16) | ((unsigned)bdf->device << 11) | ((unsigned)bdf->function << 8) | (reg & ~3));
	return inw(PCI_REG_DATA + (reg & 2));
}

unsigned int pci_read_config_dword(pci_bdf* bdf, unsigned char reg) {
	outd(PCI_REG_ADDRESS, 0x80000000L | ((unsigned long)bdf->bus << 16) | ((unsigned)bdf->device << 11) | ((unsigned)bdf->function << 8) | (reg & ~3));
	return ind(PCI_REG_DATA);
}

void pci_write_config_byte(pci_bdf* bdf, unsigned char reg, unsigned char val) {
	outd(PCI_REG_ADDRESS, 0x80000000L | ((unsigned long)bdf->bus << 16) | ((unsigned)bdf->device << 11) | ((unsigned)bdf->function << 8) | (reg & ~3));
	outb(PCI_REG_DATA + (reg & 3), val);
}

void pci_write_config_word(pci_bdf* bdf, unsigned char reg, unsigned short val) {
	outd(PCI_REG_ADDRESS, 0x80000000L | ((unsigned long)bdf->bus << 16) | ((unsigned)bdf->device << 11) | ((unsigned)bdf->function << 8) | (reg & ~3));
	outw(PCI_REG_DATA + (reg & 2), val);
}

void pci_write_config_dword(pci_bdf* bdf, unsigned char reg, unsigned int val) {
	outd(PCI_REG_ADDRESS, 0x80000000L | ((unsigned long)bdf->bus << 16) | ((unsigned)bdf->device << 11) | ((unsigned)bdf->function << 8) | (reg & ~3));
	outd(PCI_REG_DATA, val);
}

int pci_detect(void) {
	outd(PCI_REG_ADDRESS, 0x80000000L);
	if(ind(PCI_REG_ADDRESS) != 0x80000000L)
		return -1;
	return 0;
}

char* pci_get_vendor(unsigned short id) {
	for (int i = 0; i < pci_vendor_info_length; i++) {
		if (pci_vendor_list[i].id == id)
			return pci_vendor_list[i].name;
	}
	return "Unknown Vendor";
}

char* pci_get_device(unsigned short vendor, unsigned short id) {
	for (int i = 0; i < pci_device_info_length; i++) {
		if (pci_device_list[i].vendor == vendor && pci_device_list[i].id == id)
			return pci_device_list[i].name;
	}
	return "Unknown Device";
}

static int pci_iterate(pci_bdf *pci)
{
	unsigned char hdr_type = 0x80;

/* if first function of this device, check if multi-function device
(otherwise fn==0 is the _only_ function of this device) */
	if(pci->function == 0)
	{
		hdr_type = pci_read_config_byte(pci, 0x0E);
	}
/* increment iterators
fn (function) is the least significant, bus is the most significant */
	pci->function++;
	if(pci->function >= 8 || (hdr_type & 0x80) == 0)
	{
		pci->function = 0;
		pci->device++;
		if(pci->device >= 32)
		{
			pci->device = 0;
			pci->bus++;
//			if(pci->bus > g_last_pci_bus)
			if(pci->bus > 7)
				return 1; /* done */
		}
	}
	return 0;
}
/*****************************************************************************
*****************************************************************************/
bool pci_exists(unsigned short vendor, unsigned short device) {
	pci_bdf* pci = pci_get_bdf(vendor, device);
	if (!pci) {
		free(pci);
		return false;
	} else {
		free(pci);
		return true;
	}
}

pci_bdf* pci_get_bdf(unsigned short vendor, unsigned short device)
{
	pci_bdf* pci = (pci_bdf*)malloc(sizeof(pci_bdf));

	int err = 0;

/* check for PCI BIOS */
	if(pci_detect()) {
		free(pci);
		return 0;
	}
/* display numeric ID of all PCI devices detected */
	memset(pci, 0, sizeof(pci_bdf));
	do
	{
		unsigned long id;

/* 00=PCI_VENDOR_ID */
		id = pci_read_config_dword(pci, 0x00);
		if(err)
		{
			printf("Error 0x%02X reading PCI config\n", err);
			free(pci);
			return 0;
		}
/* anything there? */
		if(id != 0xFFFFFFFFL)
		{
			if (((id & 0xFFFF) == vendor) && (id >> 16 == device))
				return pci;
			//printf("bus %u, dev %2u, fn %u: "
			//	"%04X:%04X - %s %s\n", pci.bus,
			//	pci.device, pci.function, id & 0xFFFF, id >> 16, pci_get_vendor(id & 0xFFFF), pci_get_device(id & 0xFFFF, id >> 16));
			//pci_print_name(id);
		}
	} while(!pci_iterate(pci));

	free(pci);
	return 0;
}

void pci_dump(void)
{
	pci_bdf* pci = (pci_bdf*)malloc(sizeof(pci_bdf));

	int err = 0;

/* check for PCI BIOS */
	if(pci_detect()) {
		free(pci);
		return;
	}
/* display numeric ID of all PCI devices detected */
	memset(pci, 0, sizeof(pci_bdf));
	do
	{
		unsigned long id;

/* 00=PCI_VENDOR_ID */
		id = pci_read_config_dword(pci, 0x00);
		if(err)
		{
			printf("Error 0x%02X reading PCI config\n", err);
			free(pci);
			return;
		}
/* anything there? */
		if(id != 0xFFFFFFFFL)
		{
			printf("bus %u, dev %2u, fn %u: %04X:%04X - %s %s\n", pci->bus,
				pci->device, pci->function, id & 0xFFFF, id >> 16, pci_get_vendor(id & 0xFFFF), pci_get_device(id & 0xFFFF, id >> 16));
			//if (id == 0x80867000 || id == 0x70008086)
				//break;
		}
	} while(!pci_iterate(pci));

	free(pci);
}