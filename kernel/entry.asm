;; BlacklightEVO kernel/entry.asm -- kernel entry point and some asm procedures
;; Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
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


;section .rodata
global nasm_version_string
nasm_version_string:
	db "NASM ", __NASM_VER__, 0
