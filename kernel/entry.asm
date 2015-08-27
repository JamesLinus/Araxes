;; BlacklightEVO kernel/entry.asm -- kernel entry point and some asm procedures
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net

; Constants
MULTIBOOT_FLAGS equ 1<<0 | 1<<1				; mb_align | mb_info
MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

RMGLOBAL_ENTRY	equ 0x3000

RMGLOBAL_ESP	equ 0x5000
RMGLOBAL_EBX	equ 0x5004
RMGLOBAL_EAX	equ 0x5008
RMGLOBAL_PCICFG	equ 0x500C
 
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
	
	mov esi, rmode_subkernel
	mov edi, RMGLOBAL_ENTRY
	mov ecx, rmode_subkernel_end-rmode_subkernel
	rep movsb
	
	cmp eax, 0x2BADB002
	jne .rmode_check
	
	mov dword [RMGLOBAL_EBX], ebx
	mov dword [RMGLOBAL_EAX], eax
	
	mov dword [RMGLOBAL_PCICFG], 0
	
	call RMGLOBAL_ENTRY
	
.rmode_check:
	cmp eax, 'UVRM'
	je .kernel_go
	
	push eax
	mov eax, 'VGA3'
	call RMGLOBAL_ENTRY
	pop eax
	
	mov byte [0xB8000], 'W'		; Truly the pinnacle of debugging.
	mov byte [0xB8001], 0x4F
	mov byte [0xB8002], 'T'
	mov byte [0xB8003], 0x4F
	mov byte [0xB8004], 'F'
	mov byte [0xB8005], 0x4F
	mov byte [0xB8006], '?'
	mov byte [0xB8007], 0x4F
	mov byte [0xB8008], ' '
	mov byte [0xB8009], 0x4F
	mov byte [0xB800A], ' '
	mov byte [0xB800B], 0x4F
	mov byte [0xB800C], '0'
	mov byte [0xB800D], 0x4F
	mov byte [0xB800E], 'x'
	mov byte [0xB800F], 0x4F
	
	mov edi, 0xB8010
	mov ecx, 8
	mov edx, eax
	
.wtfloop:
	rol edx, 4
	mov al, dl
	and al, 0x0F
	cmp al, 10
	sbb al, 0x69
	das
	mov byte [edi], al
	inc edi
	mov byte [edi], 0x4F
	inc edi
	loop .wtfloop
	
	jmp .hang
	
.kernel_go:
	mov ecx, dword [RMGLOBAL_EAX]
	mov ebx, dword [RMGLOBAL_EBX]
	mov edx, dword [RMGLOBAL_PCICFG]
	push edx
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
	

global gdt_reload
extern gdtr
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
	

global rmode_call
extern gdt
rmode_call:
	push ebp
	mov ebp, esp
	
	cli
	mov eax, dword [ebp+8]
	
	call RMGLOBAL_ENTRY
	push eax
	
	call gdt_reload
	call idt_reload
	
	mov eax, dword [gdt+0x34]			; Clear the TSS descriptor's busy flag.
	and ah, 0xFD
	mov dword [gdt+0x34], eax
	
	mov ax, 0x33
	ltr ax
	
	call idt_reload
	
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	pop eax
	sti
	
	mov esp, ebp
	pop ebp
	ret

section .data
rmode_subkernel:
	incbin "kernel/rmode.o"
rmode_subkernel_end:

;section .rodata
global nasm_version_string
nasm_version_string:
	db "NASM ", __NASM_VER__, 0
