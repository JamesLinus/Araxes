;; BlacklightEVO boot/evofs/stage2.asm -- Second stage bootloader for EVOfs drives
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net


; We start at 0x0000:0x8000 in real mode with the boot drive identifier in DL,
; the partition we booted from in BL, and a working stack. That's about it.
;
; We can assume that since we've gotten here, the VBR loaded us just fine, we
; were booted from an EVOfs partition, and we're on at least a 386. We need to
; figure out what we're actually running on though, so that's one of the first
; things we do. BlacklightEVO won't run on any less than a Pentium.
;
; -- TODO -- actually check the file system info block

	org 0x8000
	bits 16
	
%macro print16 1+
	section .data
		%%string db %1,0
	section .text

	push si
	mov si,%%string
	call biosPrint
	pop si
%endmacro
	
entry:
	xor ax, ax
	mov ds, ax
	mov fs, ax
	mov gs, ax
	print16 `BlacklightEVO stage2\n`

detectPentium:
	pushfd
	pop ecx
	mov eax, ecx
	
	xor eax, 0x200000			; Flip ID bit (CPUID exist check) in EFLAGS
	push eax
	popfd
	pushfd
	pop eax
	
	push ecx
	popfd
	
	xor eax, ecx 
	jne .good
	
.fatal:
	print16 `FATAL: BlacklightEVO requires a Pentium or higher.`
	cli
.fatalloop:
	hlt
	jmp .fatalloop

.good:
	xor eax, eax
	inc eax
	cpuid
	and ah, 0x0F
	cmp ah, 5
	jb .fatal
	
	print16 `CPUID detect success.\n`
	
	print16 `FATAL: This stage2 actually does nothing right now. Use GRUB2 instead.`
	cli
	jmp .fatalloop


biosPrint:
	pusha
	mov ah, 0x0E
	
.pch:
	lodsb
	or al, al
	jz .done
	cmp al, 10
	je .newline
	int 0x10
	jmp .pch
	
.newline:
	mov al, 13
	int 0x10
	mov al, 10
	int 0x10
	jmp .pch
	
.done:
	popa
	ret
	
