;; BlacklightEVO kernel/rmode.asm -- real mode subkernel
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net

bits 32
org 0x3000

section .text
global rmode_subkernel
rmode_subkernel:
	mov eax, _gdt
	mov ebx, _gdt_ptr+2
	mov [ebx], eax
	lgdt [_gdt_ptr]
	
	nop
	nop
	nop
	
	jmp KRNL_CODE_SEL:.flush
	
.flush:
	jmp _16BIT_CODE_SEL:.pmode16
	
bits 16
	
.pmode16:
	mov ax, _16BIT_DATA_SEL
	mov ss, ax
	
	mov eax, cr0
	and al, 0xFE
	mov cr0, eax
	
	xor ax, ax
	push ax
	push .flush_cs
	retf

.flush_cs:
	push cs
	pop ax
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov sp, 0x3000
	
	o32 lidt [_real_idt]

	mov eax,cr0
	or al,1
	mov cr0,eax
	jmp KRNL_CODE_SEL:.back_pmode
	
bits 32
	
.back_pmode:
	
	mov ax, KRNL_DATA_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	mov eax, "UVRM"
	mov esi, dword [0x5000]
	jmp esi
	ret


global rmode_subkernel_end

_real_idt:
	dw 1023
	dd 0
	
_gdt:
	dd 0
	dd 0

KRNL_CODE_SEL	EQU ($ - _gdt)
_gdt_krnl_cs:
	dw 0xFFFF
	dw 0			; base; gets set above
	db 0
	db 0x9A			; present, ring 0, code, non-conforming, readable
	db 0xCF			; 32-bit
	db 0

KRNL_DATA_SEL	EQU ($ - _gdt)
_gdt_krnl_ds:
	dw 0xFFFF
	dw 0			; base; gets set above
	db 0
	db 0x92			; present, ring 0, data, expand-up, writable
	db 0xCF
	db 0

_16BIT_CODE_SEL	EQU ($ - _gdt)
_gdt_16bit_cs:
	dw 0xFFFF
	dw 0			; base; gets set above
	db 0
	db 0x9A			; present, ring 0, code, non-conforming, readable
	db 0			; 16-bit
	db 0

_16BIT_DATA_SEL	EQU ($ - _gdt)
_gdt_16bit_ds:
	dw 0xFFFF
	dw 0			; base; gets set above
	db 0
	db 0x92			; present, ring 0, data, expand-up, writable
	db 0
	db 0
_gdt_end:

_gdt_ptr:
	dw _gdt_end - _gdt - 1	; GDT limit
	dd _gdt			; linear adr of GDT; gets set above

rmode_subkernel_end:
