BlacklightEVO Release 1 (EVOlution)
===================================

BlacklightEVO is a free (as in beer) hobby operating system for Pentium-class and newer x86 PCs. The upcoming release, entitled EVOlution, is under active development. The operating system itself is primarily released under a proprietary permissive license (see "License").


IRC
---

A public IRC channel on Freenode called #blacklightevo is available for support, discussion, watching commits roll in, and idling. Channel rules are "keep it worksafe and relatively on-topic". If you don't have an IRC client you can use Freenode's webchat (http://webchat.freenode.net) -- please use a sensible, identifiable nick, especially if you plan to visit the channel more than once.

Building BlacklightEVO
----------------------

The basic requirements to build BlacklightEVO are:

 - Some sort of Unix system. Building under Cygwin or OS X is not officially supported and is untested.
 - A GCC cross-compiler and matching libgcc and binutils, targeting i686 (C required, C++ optional).
 - A recent version of NASM (2.03+ required, newer is better).
 - `make hd` and by extension `make hdqemu` require sudo, losetup and a disk image with an ext2 partition at offset 1MB and GRUB2.
 - `make iso` and by extension `make isoqemu` require GRUB2, grub-mkrescue, and its dependencies.
 - `make hdqemu` and `make isoqemu` require the above two requirements and a recent version of QEMU.

Once you have a copy of the code and the requisite tools, run `make clean` to ensure that there are no object files laying around (there shouldn't be any in the default distribution, but sometimes .gitignore misses a new file). `make all` will build the boot code for EVOfs and the kernel. `make hd` copies the contents of `isosrc/boot` to the first partition of the drive image pointed to by `HDIMAGE` in the Makefile, `make iso` creates a bootable ISO and copies to it the contents of `isosrc` in its entirety, and `make hdqemu` and `make isoqemu` execute their respective image Make rules and `qemu-system-i386` with 128 megabytes of RAM and `-vga std`.

Your hard drive image specified by `HDIMAGE` in the Makefile *must* have GRUB2 installed to an ext2/3/4 partition aligned to 1 MiB (1048576 bytes, 2048 sectors) in the image. The official kernel distribution, boot menu, and ramdisk should never exceed 90 megabytes; as a result we recommend a minimum of 100 MiB. Due to the use of loop devices (`/dev/loop0` in particular), `make hd` requires root (or sudo). `make iso`, using grub-mkrescue, does not.

If grub-mkrescue mysteriously fails, pop into the IRC channel and let me know. For some reason grub-mkrescue and/or xorriso dump all information to stderr instead of stdout so we have custom code in the Makefile to handle xorriso errors. Whether or not it handles grub-mkrescue errors (from bad GRUB2 installations) I have no idea.

License
-------

### BlacklightEVO General License

As noted at the top of all source files:

    // Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
    // Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
    // Questions? Comments? Concerns? Email us: blacklight@cordilon.net

And as expanded in license.txt:

    BlacklightEVO General License
    March 13, 2013
    
    BlacklightEVO is copyright (c) 2013-2015 The Cordilon Group. The Cordilon
    Group reserve the right to change the text and conditions of license at any
    time, with or without notice. By using this software in source code and/or
    binary form, you hereby agree to the terms and conditions of this license.
    
    You are free to modify the software for your own personal use, but may not
    distribute either the original or your modified version without express
    permission from The Cordilon Group. A list of authors permitted to distribute
    their modified versions of the software can be found at the BlacklightEVO
    website.
    
    You may use snippets of code from the software in your own projects under fair
    use with attribution (please credit the snippets, or whatever may remain of
    them, to The Cordilon Group and/or from the BlacklightEVO project). You may
    freely use code from the software as a reference or as inspiration for your
    own. You may not reuse entire modules of code such as drivers without express
    permission, similar to distribution for modified versions of the software.
    
    
    If you are unsure what constitutes fair use, either use your own judgement
    (please don't steal our code -- borrowing small chunks of it is okay, as long
    as you give us a shout-out) or email blacklight@cordilon.net and ask if it's
    alright.
    
    This software comes with no explicit or implicit warranties at all. It is
    provided as is. Support is available from the project website. The Cordilon
    Group, the BlacklightEVO project, and its authors are in no way responsible or
    liable for any damages whatsoever, in any shape or form, not limited to but
    including loss of data, loss of work, or loss of physical goods. By using this
    software you admit your understanding of the lack of warranty provided with
    this software.
