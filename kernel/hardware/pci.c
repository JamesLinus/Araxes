// BlacklightEVO kernel/hardware/pci.c -- Peripheral Component Interconnect bus
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <hardware/pci.h>

struct pci_device_info pci_devices[PCI_MAX_DEVICES];

unsigned int pci_config_read_dword(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset) {
	unsigned int t_bus = bus & 0xFF;
	unsigned int t_device = device & 0x1F;
	unsigned int t_function = function & 0x07;
	unsigned int t_offset = offset & 0xFC;
	
	unsigned int address = PCI_IO_ENABLE | (t_bus << 16) | (t_device << 11) | (t_function << 8) | t_offset;
	outd(PCI_IO_CONFIG_ADDRESS, address);
	return ind(PCI_IO_CONFIG_DATA);
}

unsigned short pci_config_read_word(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset) {
	unsigned int result = pci_config_read_dword(bus, device, function, offset);
	unsigned int t_offset = (offset & 0x02) * 8;
	return (result >> t_offset) & 0xFFFF;
}

unsigned char pci_config_read_byte(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset) {
	unsigned int result = pci_config_read_dword(bus, device, function, offset);
	unsigned int t_offset = (offset & 0x03) * 8;
	return (result >> t_offset) & 0xFF;
}

void pci_config_write_dword(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned int data) {
	unsigned int t_bus = bus & 0xFF;
	unsigned int t_device = device & 0x1F;
	unsigned int t_function = function & 0x07;
	unsigned int t_offset = offset & 0xFC;
	
	unsigned int address = PCI_IO_ENABLE | (t_bus << 16) | (t_device << 11) | (t_function << 8) | t_offset;
	outd(PCI_IO_CONFIG_ADDRESS, address);
	outd(PCI_IO_CONFIG_DATA, data);
}

void pci_config_write_word(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned short data) {
	unsigned int result = pci_config_read_dword(bus, device, function, offset);
	unsigned int t_offset = (offset & 0x02) * 8;
	result &= ~(0xFFFF << t_offset);
	result |= ((unsigned int)data << t_offset);
	pci_config_write_dword(bus, device, function, offset, data);
}

void pci_config_write_byte(unsigned int bus, unsigned int device, unsigned int function, unsigned int offset, unsigned char data) {
	unsigned int result = pci_config_read_dword(bus, device, function, offset);
	unsigned int t_offset = (offset & 0x03) * 8;
	result &= ~(0xFF << t_offset);
	result |= ((unsigned int)data << t_offset);
	pci_config_write_dword(bus, device, function, offset, data);
}

void pci_enumerate(void) {
	unsigned int bus = 0;
	unsigned int device = 0;
	unsigned int function = 0;
	
	unsigned short vendor_id = 0;
	unsigned short device_id = 0;
	unsigned char header_type = 0;
	
	int j = 0;
	
	debug_printf(LOG_INFO "Enumerating PCI devices. This might take a while.\n");
	
	for (int i = 0; i < PCI_MAX_DEVICES; i++) {
		pci_devices[i].device_handled = PCI_DEVICE_FREE;
	}
	
	for(bus = 0; bus < 256; bus++) {
		for(device = 0; device < 32; device++) {
			function = 0;
			if ((vendor_id = pci_config_read_word(bus, device, function, 0)) != 0xFFFF) {
				device_id = pci_config_read_word(bus, device, function, 2);
				header_type = pci_config_read_byte(bus, device, function, 0x0E);
				#if PCI_ENUMERATION_DEBUG
					kprintf("%02X:%02X:%d - 0x%04X:0x%04X - Header Type 0x%02X - 0x%08X\n", bus, device, function, vendor_id, device_id, header_type, pci_config_read_dword(bus, device, function, 0x08));
				#endif
				
				pci_devices[j].bus = bus;
				pci_devices[j].device = device;
				pci_devices[j].function = function;
				pci_devices[j].vendor_id = vendor_id;
				pci_devices[j].device_id = device_id;
				pci_devices[j].device_handled = PCI_DEVICE_EXISTS;
				
				if (++j == PCI_MAX_DEVICES)
					break;
				
				if (header_type & 0x80) {
					for (function = 1; function < 8; function++) {
						if ((vendor_id = pci_config_read_word(bus, device, function, 0)) != 0xFFFF) {
							device_id = pci_config_read_word(bus, device, function, 2);
							header_type = pci_config_read_byte(bus, device, function, 0x0E);
							#if PCI_ENUMERATION_DEBUG
								kprintf("%02X:%02X:%d - 0x%04X:0x%04X - Header Type 0x%02X - 0x%08X\n", bus, device, function, vendor_id, device_id, header_type, pci_config_read_dword(bus, device, function, 0x08));
							#endif
							
							pci_devices[j].bus = bus;
							pci_devices[j].device = device;
							pci_devices[j].function = function;
							pci_devices[j].vendor_id = vendor_id;
							pci_devices[j].device_id = device_id;
							pci_devices[j].device_handled = PCI_DEVICE_EXISTS;
							
							if (++j == PCI_MAX_DEVICES)
								break;
						}
					}
				}
			}
		}
		
		if (j == PCI_MAX_DEVICES) {
			debug_printf(LOG_WARNING "PCI_MAX_DEVICES hit at bus %02X device %02X function %d.\n", bus, device, function);
			break;
		}
	}
	
	debug_printf(LOG_INFO "Enumerated %d PCI devices.\n", j);
}