## BlacklightEVO Makefile -- exactly what it says on the tin
## Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
## Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
## Questions? Comments? Concerns? Email us: blacklight@cordilon.net

CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
NASM = nasm

KERNSOURCES_C := kernel/main.c kernel/global.c kernel/mm.c kernel/vga.c kernel/gdt.c kernel/idt.c kernel/printf.c kernel/fb_font.c
KERNSOURCES_ASM := kernel/entry.asm kernel/isr.asm
KERNOBJECTS := $(KERNSOURCES_C:.c=.o) $(KERNSOURCES_ASM:.asm=.o)
KERNELF = kernel-i686.elf

KERNCFLAGS = -c -Ikernel/include -ffreestanding -std=gnu99 -O2 -Wall -Wextra
KERNLDFLAGS = -T kernel/linker.ld -o $(KERNELF)
KERNNASMFLAGS = -felf

HDIMAGE = evo-5G.img

.PHONY: all alliso clean kernel objects hdqemu
.SUFFIXES: .c .asm

all: kernel

hdqemu: kernel
	sudo losetup /dev/loop0 evo-5G.img -o 1048576
	sudo mount /dev/loop0 /mnt
	sudo cp $(KERNELF) /mnt/boot
	sudo umount /mnt
	sudo losetup -d /dev/loop0
	qemu-system-i386 -m 128 -hda evo-5G.img

iso:
	cp $(KERNELF) isosrc/boot
	grub-mkrescue -o evo.iso isosrc

objects:
	echo $(KERNOBJECTS)

clean:
	rm $(KERNOBJECTS) $(KERNELF)

kernel: $(KERNELF)

$(KERNELF): $(KERNOBJECTS)
	$(LD) $(KERNLDFLAGS) $(KERNOBJECTS)

.asm.o:
	$(NASM) $(KERNNASMFLAGS) -o $@ $<

.c.o:
	$(CC) $(KERNCFLAGS) -o $@ $<

