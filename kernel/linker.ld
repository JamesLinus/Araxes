/* BlacklightEVO kernel/linker.ld -- kernel entry point
 * Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
 * Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
 * Questions? Comments? Concerns? Email us: blacklight@cordilon.net */

ENTRY(_entry)

SECTIONS
{
	. = 1M;

	.text BLOCK(4K): ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K): ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K): ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K): ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.entry_stack)
	}
	
	.unused_section BLOCK(4K): ALIGN (4K)
	{
		kernel_end = . ;
	}
}
