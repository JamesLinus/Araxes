// BlacklightEVO kernel/hardware/pci.c -- Peripheral Component Interconnect bus
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <hardware/pci.h>

struct pci_device_info pci_devices[PCI_MAX_DEVICES];

// Reads a 32-bit value from a PCI device's configuration space.
uint32_t pci_config_read_dword(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset) {
	uint32_t t_bus = bus & 0xFF;
	uint32_t t_device = device & 0x1F;
	uint32_t t_function = function & 0x07;
	uint32_t t_offset = offset & 0xFC;
	
	uint32_t address = PCI_IO_ENABLE | (t_bus << 16) | (t_device << 11) | (t_function << 8) | t_offset;
	outd(PCI_IO_CONFIG_ADDRESS, address);
	return ind(PCI_IO_CONFIG_DATA);
}

// Reads a 16-bit value from a PCI device's configuration space.
uint16_t pci_config_read_word(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset) {
	uint32_t result = pci_config_read_dword(bus, device, function, offset);
	uint32_t t_offset = (offset & 0x02) * 8;
	return (result >> t_offset) & 0xFFFF;
}

// Reads an 8-bit value from a PCI device's configuration space.
uint8_t pci_config_read_byte(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset) {
	uint32_t result = pci_config_read_dword(bus, device, function, offset);
	uint32_t t_offset = (offset & 0x03) * 8;
	return (result >> t_offset) & 0xFF;
}

// Writes a 32-bit value to a PCI device's configuration space.
void pci_config_write_dword(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint32_t data) {
	uint32_t t_bus = bus & 0xFF;
	uint32_t t_device = device & 0x1F;
	uint32_t t_function = function & 0x07;
	uint32_t t_offset = offset & 0xFC;
	
	uint32_t address = PCI_IO_ENABLE | (t_bus << 16) | (t_device << 11) | (t_function << 8) | t_offset;
	outd(PCI_IO_CONFIG_ADDRESS, address);
	outd(PCI_IO_CONFIG_DATA, data);
}

// Writes a 16-bit value to a PCI device's configuration space.
void pci_config_write_word(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint16_t data) {
	uint32_t result = pci_config_read_dword(bus, device, function, offset);
	uint32_t t_offset = (offset & 0x02) * 8;
	result &= ~(0xFFFF << t_offset);
	result |= ((uint32_t)data << t_offset);
	pci_config_write_dword(bus, device, function, offset, data);
}

// Writes an 8-bit value to a PCI device's configuration space.
void pci_config_write_byte(uint32_t bus, uint32_t device, uint32_t function, uint32_t offset, uint8_t data) {
	uint32_t result = pci_config_read_dword(bus, device, function, offset);
	uint32_t t_offset = (offset & 0x03) * 8;
	result &= ~(0xFF << t_offset);
	result |= ((uint32_t)data << t_offset);
	pci_config_write_dword(bus, device, function, offset, data);
}

// Enumerates all the PCI devices on the system that we can find.
void pci_enumerate(void) {
	uint32_t bus = 0;
	uint32_t device = 0;
	uint32_t function = 0;
	
	uint16_t vendor_id = 0;
	uint16_t device_id = 0;
	uint8_t header_type = 0;
	
	int j = 0;
	
	#if PCI_ENUMERATION_DEBUG
		debug_printf(LOG_INFO "Enumerating PCI devices. This might take a while.\n");
	#endif
	
	// Clear out the list of PCI devices in case it hasn't been done yet.
	for (int i = 0; i < PCI_MAX_DEVICES; i++) {
		pci_devices[i].device_handled = PCI_DEVICE_FREE;
	}
	
	// The main enumeration loop. We do a brute force scan on each bus for devices, and each
	// device for functions. We log each function we find as a found PCI device in the PCI
	// device structure list.
	for(bus = 0; bus < 256; bus++) {
		for(device = 0; device < 32; device++) {
			function = 0;
			if ((vendor_id = pci_config_read_word(bus, device, function, 0)) != 0xFFFF) {
				device_id = pci_config_read_word(bus, device, function, 2);
				header_type = pci_config_read_byte(bus, device, function, 0x0E);
				#if PCI_ENUMERATION_DEBUG
					debug_printf(LOG_INFO "%02X:%02X:%d - 0x%04X:0x%04X - Header Type 0x%02X - 0x%08X\n", bus, device, function, vendor_id, device_id, header_type, pci_config_read_dword(bus, device, function, 0x08));
				#endif
				
				pci_devices[j].bus = bus;
				pci_devices[j].device = device;
				pci_devices[j].function = function;
				pci_devices[j].vendor_id = vendor_id;
				pci_devices[j].device_id = device_id;
				pci_devices[j].device_handled = PCI_DEVICE_EXISTS;
				
				if (++j == PCI_MAX_DEVICES)
					break;
				
				// If bit 7 is set in the header type, we are dealing with a
				// multifunction device. We need to iterate through the 7 other
				// functions to see if they exist. Multifunction PCI devices are
				// allowed to have discontiguous functions, much to the chagrin
				// of anyone writing a PCI device enumerator.
				if (header_type & 0x80) {
					for (function = 1; function < 8; function++) {
						if ((vendor_id = pci_config_read_word(bus, device, function, 0)) != 0xFFFF) {
							device_id = pci_config_read_word(bus, device, function, 2);
							header_type = pci_config_read_byte(bus, device, function, 0x0E);
							#if PCI_ENUMERATION_DEBUG
								debug_printf(LOG_INFO "%02X:%02X:%d - 0x%04X:0x%04X - Header Type 0x%02X - 0x%08X\n", bus, device, function, vendor_id, device_id, header_type, pci_config_read_dword(bus, device, function, 0x08));
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
		
		// If we run out of PCI device structures to fill, log it.
		if (j == PCI_MAX_DEVICES) {
			debug_printf(LOG_WARNING "PCI_MAX_DEVICES hit at bus %02X device %02X function %d.\n", bus, device, function);
			break;
		}
	}
	
	#if PCI_ENUMERATION_DEBUG
		debug_printf(LOG_INFO "Enumerated %d PCI devices.\n", j);
	#endif
}