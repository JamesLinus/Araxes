;; BlacklightEVO kernel/rmode.asm -- real mode subkernel
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; The Blacklight project is under the terms of the ISC license. See license.md for details.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net

RMGLOBAL_ESP		equ 0x5000
RMGLOBAL_EBX		equ 0x5004
RMGLOBAL_EAX		equ 0x5008
RMGLOBAL_PCICFG		equ 0x500C
RMGLOBAL_VIDEO_WIDTH	equ 0x5010
RMGLOBAL_VIDEO_HEIGHT	equ 0x5012
RMGLOBAL_VIDEO_DEPTH	equ 0x5014
RMGLOBAL_VIDEO_MODE	equ 0x5016
RMGLOBAL_RETVAL		equ 0x5018

RMGLOBAL_VBE_BUFFER	equ 0x5200

bits 32
org 0x3000

section .text

;; ===========================================================================
;; rmode_subkernel - drops to real mode, performs a function, returns to pmode
;; Pass a magic number in EAX. Returns magic number 'UVRM' in EAX.
;; Clobbers GDTR, IDTR. Disables interrupts and paging.
;; Reload GDTR, IDTR, IF and paging before continuing!
;; ===========================================================================
rmode_subkernel:
	cli						; Ensure interrupts are disabled.
	
	mov dword [RMGLOBAL_ESP], esp			; Save the previous stack pointer.
	mov edx, eax					; ...and the magic number.
	
	mov eax, cr0					; Disable paging.
	and eax, 0x7FFFFFFF
	mov cr0, eax
	
	mov eax, _gdt					; Load a new GDT.
	mov ebx, _gdt_ptr+2
	mov [ebx], eax
	lgdt [_gdt_ptr]
	
	nop
	nop
	nop
	
	jmp KRNL_CODE_SEL:.flush			; Ensure we're in a flat 32-bit CS.
	
.flush:
	jmp _16BIT_CODE_SEL:.pmode16			; Move to 16-bit protected mode.
	
bits 16
	
.pmode16:
	mov ax, _16BIT_DATA_SEL				; Load a flat 16-bit data segment.
	mov ss, ax					; We need a sane stack for the next bit.
	mov sp, 0x3000
	
	mov eax, cr0					; Disable protected mode.
	and al, 0xFE
	mov cr0, eax
	
	xor ax, ax					; Far jump into a 16-bit real mode CS.
	push ax
	push .flush_cs
	retf

.flush_cs:
	push cs						; CS = DS = ES = FS = GS = SS = 0
	pop ax
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov sp, 0x3000					; Start the stack right below us.
	
	o32 lidt [_real_idt]				; Load an IDT that points to the IVT.
	
.parse_magic:						; switch(magic)
	cmp edx, 'VGA3'
	je vga_set_mode3
	
	cmp edx, 'VBE0'
	je vbe_do_4F00
	
	cmp edx, 'VBE1'
	je vbe_do_4F01
	
	cmp edx, 'EDID'
	je vbe_do_4F15
	
	cmp edx, 'DOWN'
	je apm_shutdown
	
	cmp edx, 0x2BADB002
	je multiboot_fixup
	
	mov dword [RMGLOBAL_VBE_BUFFER], 0xFFFFFFFF
	jmp return_pmode				; Just return on a bad magic number.


;; ===========================================================================
;; vga_set_mode3 - set the screen to VGA mode 3 (80x25 colour text mode)
;; Called with magic number 'VGA3'
;; ===========================================================================
vga_set_mode3:
	mov ax, 0x0003					; Do the actual mode set.
	int 0x10
	
	mov ax, 0x1002					; Set all the EGA palette registers at once.
	mov dx, text_palette				; We do this to turn brown into dark yellow.
	xor bx, bx					; Brown is historically accurate, but looks
	int 0x10					; out of place in the ANSI colour code set.
	
	jmp return_pmode


;; ===========================================================================
;; vbe_do_4F00 - calls VBE function 00h and places the result in a buffer
;; Called with magic number 'VBE0'
;; ===========================================================================
vbe_do_4F00:
	mov di, RMGLOBAL_VBE_BUFFER			; Clear the buffer of whatever was last in it.
	mov cx, 0x200
	mov al, 0
	rep stosb
	
	mov eax, "VBE2"					; Place {'V', 'B', 'E', '2'} in the buffer.
	mov dword [RMGLOBAL_VBE_BUFFER], eax
	
	mov eax, 0x4F00					; VBE Function 00h: Get SuperVGA Information
	mov edi, RMGLOBAL_VBE_BUFFER
	int 10h
	
	movzx eax, ax
	mov dword [RMGLOBAL_RETVAL], eax
	jmp return_pmode


;; ===========================================================================
;; vbe_do_4F01 - calls VBE function 01h and places the result in a buffer
;; Called with magic number 'VBE1'
;; ===========================================================================
vbe_do_4F01:
	mov di, RMGLOBAL_VBE_BUFFER			; Clear the buffer of whatever was last in it.
	mov cx, 0x200
	mov al, 0
	rep stosb
	
	mov eax, 0x4F01					; VBE Function 01h: Get SuperVGA Mode Information
	mov cx, word [RMGLOBAL_VIDEO_MODE]
	movzx ecx, cx
	mov edi, RMGLOBAL_VBE_BUFFER
	int 10h
	
	movzx eax, ax
	mov dword [RMGLOBAL_RETVAL], eax
	jmp return_pmode


;; ===========================================================================
;; vbe_do_4F02 - calls VBE function 02h and places the result in a buffer
;; Called with magic number 'VBE2'
;; ===========================================================================
vbe_do_4F02:
	mov di, RMGLOBAL_VBE_BUFFER			; Clear the buffer of whatever was last in it.
	mov cx, 0x200
	mov al, 0
	rep stosb
	
	mov eax, 0x4F02					; VBE Function 02h: Set SuperVGA Mode
	mov bx, word [RMGLOBAL_VIDEO_MODE]
	movzx ebx, bx
	int 10h
	
	movzx eax, ax
	mov dword [RMGLOBAL_RETVAL], eax
	jmp return_pmode


;; ===========================================================================
;; vbe_do_4F15 - calls VBE function 15h and places the result in a buffer
;; Called with magic number 'EDID'
;; ===========================================================================
vbe_do_4F15:
	mov di, RMGLOBAL_VBE_BUFFER			; Clear the buffer of whatever was last in it.
	mov cx, 0x200
	mov al, 0
	rep stosb
	
	mov ax, 0x4F15					; VBE Function 15h: Data Display Channel
	mov bl, 1					; Subfunction 01h: Read EDID
	xor cx, cx
	mov dx, cx
	mov di, RMGLOBAL_VBE_BUFFER
	int 10h
	
	movzx eax, ax
	mov dword [RMGLOBAL_RETVAL], eax
	jmp return_pmode


;; ===========================================================================
;; apm_shutdown - connect to Advanced Power Management and power the PC off
;; Called with magic number 'DOWN'
;; ===========================================================================
apm_shutdown:
	mov ax, 0x5300					; Check if there's an APM BIOS.
	mov bx, 0x0000
	int 0x15
	jc .fail					; Failure? Return to caller.
	
	cmp al, 2					; We need APM 1.2+ to power down.
	jb .fail
	
	mov ax, 0x5301					; Connect real-mode interface.
	mov bx, 0x0000
	int 0x15
	jc .fail
	
	mov ax, 0x530E					; Set the BIOS compatibility version.
	mov bx, 0x0000
	mov cx, 0x0102					; Request the BIOS uses APM 1.2.
	int 0x15
	jc .fail
	
	mov ax, 0x5307					; Set a device's APM power state.
	mov bx, 0x0001					; Device ID 0001: all APM devices.
	mov cx, 0x0003					; Power state 0003: off.
	int 0x15
	
.fail:
	movzx eax, ax
	mov dword [RMGLOBAL_RETVAL], eax
	jmp return_pmode				; If the power down failed, return.


;; ===========================================================================
;; multiboot_fixup - fills in some blanks that GRUB doesn't
;; Called with magic number 0x2BADB002
;; ===========================================================================
multiboot_fixup:

get_pci_mechanism:
	mov ax, 0xB101
	xor edi, edi
	int 0x1A
	cmp ah, 0
	je .pciget
	
	mov dword [RMGLOBAL_PCICFG], 0xFFFFFFFF
	jmp .done
	
.pciget:
	mov word [RMGLOBAL_PCICFG], ax
	mov byte [RMGLOBAL_PCICFG+2], cl
	mov byte [RMGLOBAL_PCICFG+3], dl

.done:
	mov dword [RMGLOBAL_RETVAL], 'UVRM'
	jmp vga_set_mode3


;; ===========================================================================
;; return_pmode - enables 32-bit protected mode and returns to the caller
;; ===========================================================================
return_pmode:
	mov eax,cr0
	or al,1
	mov cr0,eax
	jmp KRNL_CODE_SEL:back_pmode
	
bits 32
	
back_pmode:
	mov ax, KRNL_DATA_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	mov eax, dword [RMGLOBAL_RETVAL]
	mov esp, dword [RMGLOBAL_ESP]
	ret


;; ===========================================================================
;; text_palette - an EGA/VGA text mode colour palette
;; ===========================================================================
text_palette:
	db 000000b
	db 000001b
	db 000010b
	db 000011b
	db 000100b
	db 000101b
	db 000110b
	db 000111b
	db 111000b
	db 111001b
	db 111010b
	db 111011b
	db 111100b
	db 111101b
	db 111110b
	db 111111b
	db 000000b

;; ===========================================================================
;; _real_idt, _gdt - real mode IDT, mixed mode GDT
;; ===========================================================================
_real_idt:
	dw 1023
	dd 0
	
_gdt:
	dd 0
	dd 0

KRNL_CODE_SEL	EQU ($ - _gdt)
_gdt_krnl_cs:
	dw 0xFFFF
	dw 0
	db 0
	db 0x9A
	db 0xCF
	db 0

KRNL_DATA_SEL	EQU ($ - _gdt)
_gdt_krnl_ds:
	dw 0xFFFF
	dw 0
	db 0
	db 0x92
	db 0xCF
	db 0

_16BIT_CODE_SEL	EQU ($ - _gdt)
_gdt_16bit_cs:
	dw 0xFFFF
	dw 0
	db 0
	db 0x9A
	db 0
	db 0

_16BIT_DATA_SEL	EQU ($ - _gdt)
_gdt_16bit_ds:
	dw 0xFFFF
	dw 0
	db 0
	db 0x92
	db 0
	db 0
_gdt_end:

_gdt_ptr:
	dw _gdt_end - _gdt - 1
	dd _gdt

rmode_subkernel_end:
