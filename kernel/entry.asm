;; BlacklightEVO kernel/entry.asm -- kernel entry point and some asm procedures
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net

; Constants
MULTIBOOT_FLAGS equ 1<<0 | 1<<1				; mb_align | mb_info
MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
 
; Multiboot header
section .multiboot
align 4
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM
 
; Stack for the entry point and its code
section .entry_stack
align 4
entry_stack:
	times 8192 db 0
 
; Entry point begins here
section .text
global _entry
_entry:
	mov esp, entry_stack+8192
	mov dword [0x5000], _entry
	
	cmp eax, 0x2BADB002
	jne .rmode_check
	
	mov dword [0x5004], ebx
	mov dword [0x5008], eax
	mov esi, rmode_subkernel
	mov edi, 0x3000
	mov ecx, rmode_subkernel_end-rmode_subkernel
	rep movsb
	
	mov eax, 0x3000
	jmp eax
	
.rmode_check:
	cmp eax, "UVRM"
	je .kernel_go
	
	mov byte [0xB8000], 'W'		; Truly the pinnacle of debugging.
	mov byte [0xB8002], 'T'
	mov byte [0xB8004], 'F'
	mov byte [0xB8006], '?'
	
	mov byte [0xB80A7], al
	mov byte [0xB80A5], ah
	shr eax, 16
	mov byte [0xB80A3], al
	mov byte [0xB80A1], ah
	
	mov byte [0xB80A6], 0xDE
	mov byte [0xB80A4], 0xDE
	mov byte [0xB80A2], 0xDE
	mov byte [0xB80A0], 0xDE
	jmp .hang
	
.kernel_go:
	mov ecx, dword [0x5008]
	mov ebx, dword [0x5004]
	push ecx
	push ebx
	push eax
	; Jump into the kernel
	extern kernel_main
	call kernel_main
 
	; We shouldn't ever get here, but just in case we do somehow, lock up
.hang:
	cli
	hlt
	jmp .hang
	

extern gdtr
global gdt_reload
gdt_reload:
	lgdt [gdtr]
	
	mov ax, 0x18
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	jmp 0x10:.flush

.flush:
	ret

global idt_reload
extern idtr
idt_reload:
	lidt [idtr]
	ret
	

section .data
rmode_subkernel:
	incbin "kernel/rmode.bin"
rmode_subkernel_end:

;section .rodata
global nasm_version_string
nasm_version_string:
	db "NASM ", __NASM_VER__, 0
