## Araxes Release 1 (Farron)

Araxes is an open source operating system for P6-class and newer PCs. First party content (including the operating system's kernel and the majority of the userspace) is under the ISC license.

### IRC

A public IRC channel on Freenode called #araxes is available for support, discussion, watching commits roll in, and idling. Channel rules are "keep it worksafe and relatively on-topic". If you don't have an IRC client you can use Freenode's webchat (http://webchat.freenode.net) -- please use a sensible, identifiable nick, especially if you plan to visit the channel more than once.

### Building Araxes

The basic requirements to build Araxes are:

 - **Some sort of Unix-like system.** Official builds are done on an x86-64 Linux system. Building under Cygwin or OS X is not officially supported and is untested.
 - A GCC cross-compiler and matching libgcc and binutils, targeting i686 (C required, C++ optional).
 - A recent version of NASM (2.03+ required, newer is better).
 - `make hd` and by extension `make hdqemu` require sudo, losetup and a disk image with an ext2 partition at offset 1MB and GRUB2.
 - `make iso` and by extension `make isoqemu` require GRUB2, grub-mkrescue, and its dependencies.
 - `make hdqemu` and `make isoqemu` require the above two requirements and a recent version of QEMU.

Once you have a copy of the code and the requisite tools, run `make clean` to ensure that there are no object files laying around (there shouldn't be any in the default distribution, but sometimes .gitignore misses a new file). `make all` will build the boot code for EVOfs and the kernel. `make hd` copies the contents of `isosrc/boot` to the first partition of the drive image pointed to by `HDIMAGE` in the Makefile, `make iso` creates a bootable ISO and copies to it the contents of `isosrc` in its entirety, and `make hdqemu` and `make isoqemu` execute their respective image Make rules and `qemu-system-i386` with 128 megabytes of RAM and `-vga std`.

Your hard drive image specified by `HDIMAGE` in the Makefile **must** have GRUB2 installed to an ext2/3/4 partition aligned to 1 MiB (1048576 bytes, 2048 sectors) in the image. The official kernel distribution, boot menu, and ramdisk should never exceed 90 megabytes; as a result we recommend a minimum of 100 MiB. Due to the use of loop devices (`/dev/loop0` in particular), `make hd` requires root (or sudo). `make iso`, using grub-mkrescue, does not.

If grub-mkrescue mysteriously fails, check that you have xorriso installed (grub-mkrescue likes to silently fail if it can't find xorriso). If you have xorriso installed and there's a silent failure, pop into the IRC channel and let me know. grub-mkrescue and/or xorriso dumps all information to stderr instead of stdout so we have custom code in the Makefile to handle xorriso errors.

### License

Araxes is released under the terms of the ISC license. Copyright is attributed to The Cordilon Group. The project includes source code from other projects. All applicable licenses to source code in the Araxes project can be found in the license.md file in the root of the Araxes Git repository as well as all official distributions of the operating system.
