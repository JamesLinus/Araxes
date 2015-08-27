## BlacklightEVO Makefile -- exactly what it says on the tin
## Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
## Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
## Questions? Comments? Concerns? Email us: blacklight@cordilon.net


## Okay, so this Makefile is probably the worst thing I have ever written.
## It's more shell script than Makefile, but it creates the prettiest output
## when the build goes perfectly without any warnings.

## Custom error handling has been written for make iso because for some
## incredibly stupid reason, grub-mkrescue sends all general output to stderr,
## making it impossible to actually separate real errors from "hi I make ISO
## files :)". I can only imagine that the people at GNU were huffing duster
## when they made the decision to ignore the "err" in "stderr". To work around
## this, we redirect stderr for grub-mkrescue to iso.log, grep it for any sign
## of failure from xorriso, and crap out if that's the case.

## There's also some custom handling for kernel/rmode.asm, which is assembled
## as a 16-bit flat binary, included as a binary blob in kernel/entry.asm, and
## loaded to physical address 0x3000 by the kernel if it feels it needs to
## drop back to real mode to fill in blank spots left by the bootloader.

CC = i686-elf-gcc
AS = i686-elf-as
LD = i686-elf-ld
NASM = nasm

## This is pretty much because my cross-toolchain is broken and rebuilding it
## doesn't seem to fix it. Unless you happen to be the only other Kazinsal on
## the internet, you'll want to change this.
LIBGCC_DIR = /home/kazinsal/opt/cross/lib/gcc/i686-elf/4.8.2/ 

KERNSOURCES_C := kernel/main.c kernel/global.c kernel/mm.c kernel/vga.c kernel/gdt.c kernel/idt.c kernel/printf.c kernel/fb_font.c kernel/hardware/timer.c kernel/hardware/uart.c kernel/fs/mbr.c
KERNSOURCES_ASM := kernel/entry.asm kernel/isr.asm
KERNOBJECTS := $(KERNSOURCES_C:.c=.o) kernel/rmode.o $(KERNSOURCES_ASM:.asm=.o)
KERNOBJECTS_LINK := $(KERNSOURCES_C:.c=.o) $(KERNSOURCES_ASM:.asm=.o)
KERNELF = evo-i686.elf

KERNCFLAGS = -c -Ikernel/include -DKERNEL_VERSION_MAJOR=$(KERNEL_VERSION_MAJOR) -DKERNEL_VERSION_MINOR=$(KERNEL_VERSION_MINOR) -DKERNEL_VERSION_PATCH=$(KERNEL_VERSION_PATCH) -DKERNEL_VERSION_DEBUG=$(KERNEL_VERSION_DEBUG) -ffreestanding -std=gnu99 -O0 -Wall -Wextra
KERNLDFLAGS = -L$(LIBGCC_DIR) -T kernel/linker.ld -o $(KERNELF)
KERNNASMFLAGS = -felf

KERNEL_VERSION_MAJOR=0
KERNEL_VERSION_MINOR=0
KERNEL_VERSION_PATCH=0
KERNEL_VERSION_DEBUG=23

HDIMAGE = evo-6G.img
CDIMAGE = evo.iso

.PHONY: all alliso clean clean-kernel clean-boot kernel objects hd hdqemu iso isoqemu boot tools help
.SUFFIXES: .c .asm

help:
	@echo " -- Diplaying help                                (make help)"
	@echo
	@echo " -- Available make targets:"
	@echo "     - all:          'make tools boot kernel'"
	@echo "     - tools:        Builds build assistance tools"
	@echo "     - boot:         Builds EVOfs boot files"
	@echo "     - kernel:       Builds BlacklightEVO kernel"
	@echo
	@echo "     - hd:           all + Updates hard disk image boot partition"
	@echo "     - hdqemu:       hd + executes QEMU"
	@echo "     - iso:          all + Creates LiveCD image
	@echo "     - isoqemu:      iso + executes QEMU"
	@echo
	@echo "     - clean:        'make clean-tools clean-boot clean-kernel'"
	@echo "     - clean-tools:  Removes build assistance tool binaries"
	@echo "     - clean-boot:   Removes EVOfs boot file binaries"
	@echo "     - clean-kernel: Removes BlacklightEVO kernel binaries"

all: tools boot kernel

tools:
	@echo " -- Building build assistance tools               (make tools)"
	@echo "     - debugver"
	@gcc -o tools/debugver tools/debugver.c
	@chmod a+x tools/debugver

hd: all
	@echo " -- Updating hard disk image boot partition       (make hd)"
	@echo "     - $(KERNELF) -> isosrc/boot"
	@cp $(KERNELF) isosrc/boot
	@mkdir -p mnt
	@sudo losetup /dev/loop0 $(HDIMAGE) -o 1048576
	@sudo mount /dev/loop0 mnt
	@echo "     - isosrc/boot/$(KERNELF)"
	@sudo cp isosrc/boot/$(KERNELF) mnt/boot
	@echo "     - isosrc/boot/grub/*.cfg"
	@sudo cp isosrc/boot/grub/*.cfg mnt/boot/grub
	@echo "     - isosrc/boot/grub/*.png"
	@sudo cp isosrc/boot/grub/*.png mnt/boot/grub
	@sudo umount mnt
	@sudo losetup -d /dev/loop0
	
hdqemu: hd
	@echo " -- Executing QEMU                                (make hdqemu)"
	@echo -n "     - "
	qemu-system-i386 -m 128 -hda $(HDIMAGE) -vga std

iso: all
	@echo " -- Building LiveCD image                         (make iso)"
	@echo "     - $(KERNELF) -> isosrc/boot"
	@cp $(KERNELF) isosrc/boot
	@echo "     - grub-mkrescue -o $(CDIMAGE) isosrc"
	@grub-mkrescue -o $(CDIMAGE) isosrc 2> iso.log; if grep "FAILURE" iso.log; then rm iso.log; exit 1; else rm iso.log; fi;
	
isoqemu: iso
	@echo " -- Executing QEMU                                (make isoqemu)"
	@echo -n "     - "
	qemu-system-i386 -m 128 -cdrom $(CDIMAGE) -vga std

objects:
	@echo $(KERNOBJECTS)

clean: clean-tools clean-boot clean-kernel

clean-tools:
	@echo " -- Cleaning build assistance tools               (make clean-tools)"
	@rm -f tools/debugver

clean-kernel:
	@echo " -- Cleaning kernel objects                       (make clean-kernel)"
	@rm -f $(KERNOBJECTS) $(KERNELF) 2>/dev/null

clean-boot:
	@echo " -- Cleaning boot objects                         (make clean-boot)"
	@rm -f boot/evofs/mbr boot/evofs/vbr boot/evofs/stage2 2>/dev/null

## We change the GRUB2 configs here for GRUB2 based bootloaders, updating the title text.
pre-kernel:
	@echo " -- Building kernel                               (make kernel)"
	@sed -i "s/^title-text:.*/title-text: \"BlacklightEVO Development Build $(KERNEL_VERSION_MAJOR).$(KERNEL_VERSION_MINOR).$(KERNEL_VERSION_PATCH)`tools/debugver $(KERNEL_VERSION_DEBUG)`\"/" isosrc/boot/grub/grub-theme.cfg
##@sed -i "s/^menuentry \"BlacklightEVO.*/menuentry  \"BlacklightEVO $(KERNEL_VERSION_MAJOR).$(KERNEL_VERSION_MINOR).$(KERNEL_VERSION_PATCH)`tools/debugver $(KERNEL_VERSION_DEBUG)`\" {/" isosrc/boot/grub/grub.cfg

kernel: tools pre-kernel $(KERNELF)

## Boot code nasm, including the worst (best?) bash one-liner I've ever written
boot:
	@echo " -- Building EVOfs boot files                     (make boot)"
	@echo "     - boot/evofs/mbr.asm"
	@$(NASM) boot/evofs/mbr.asm
	@echo "     - boot/evofs/vbr.asm"
	@$(NASM) boot/evofs/vbr.asm
	@echo "     - boot/evofs/stage2.asm"
	@$(NASM) boot/evofs/stage2.asm && if [ `wc -c boot/evofs/stage2 | cut -d' ' -f1` -gt 32070 ]; then echo >&2 "Error building stage2: resulting binary size `wc -c boot/evofs/stage2 | cut -d' ' -f1` > 32070"; false; else truncate -s 30720 boot/evofs/stage2; fi

$(KERNELF): $(KERNOBJECTS)
	@echo -n "     - Linking $@..."
	@$(LD) $(KERNLDFLAGS) $(KERNOBJECTS_LINK) -lgcc && echo " Done!" || echo

kernel/rmode.bin:
	@echo "     - kernel/rmode.asm"
	@$(NASM) -o kernel/rmode.bin kernel/rmode.asm

.asm.o:
	@echo "     - $<"
	@if [ "$<" = "kernel/rmode.asm" ]; then $(NASM) -fbin -o $@ $<; rm -f kernel/entry.o; else $(NASM) $(KERNNASMFLAGS) -o $@ $<; fi

.c.o:
	@echo "     - $<"
	@$(CC) $(KERNCFLAGS) -o $@ $<

