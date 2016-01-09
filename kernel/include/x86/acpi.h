// BlacklightEVO kernel/include/x86/acpi.h -- Advanced Configuration and Power Interface
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __KERNEL__INCLUDE__X86__ACPI_H
#define __KERNEL__INCLUDE__X86__ACPI_H

extern void* acpi_rsdp;

void* acpi_get_rsdp(void);

#endif	// __KERNEL__INCLUDE__X86__ACPI_H
