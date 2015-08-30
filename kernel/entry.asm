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
	times 16384 db 0
 
; Entry point begins here
section .text
global _entry
_entry:
	mov esp, entry_stack+16384			; Set up a kernel stack.
	
	mov esi, rmode_subkernel			; Copy the real mode subkernel to low memory.
	mov edi, RMGLOBAL_ENTRY
	mov ecx, rmode_subkernel_end-rmode_subkernel
	rep movsb
	
	cmp eax, 0x2BADB002					; If we were multibooted, we need to rmode_call().
	jne .rmode_check
	
	mov dword [RMGLOBAL_EBX], ebx		; Pass our current EBX and EAX to the subkernel.
	mov dword [RMGLOBAL_EAX], eax
	
	mov dword [RMGLOBAL_PCICFG], 0		; Zeroed in case the BIOS doesn't zero low memory.
	
	call RMGLOBAL_ENTRY					; Invoke the real mode subkernel.
	
.rmode_check:
	cmp eax, 'UVRM'						; Did the real mode subkernel return?
	je .kernel_go						; Alternatively, were we loaded via EVOboot?
	
	push eax							; If neither of those are the case, display a WTF message.
	mov eax, 'VGA3'						; Invoke the real mode subkernel to set a VGA text mode.
	call RMGLOBAL_ENTRY
	pop eax
	
	mov byte [0xB8000], 'W'				; Truly the pinnacle of debugging.
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
	
.wtfloop:								; This loop dumps EDX as hexadecimal.
	rol edx, 4							; Fetch the next digit into AL.
	mov al, dl
	and al, 0x0F						; Zero-extend the low nibble to the whole byte.
	cmp al, 10							; CF = (AL < 10)
	sbb al, 0x69						; AL = AL - (0x69 + CF)
	das									; ... "Decimal Adjust after Subtraction"
	mov byte [edi], al					; We now have the digit in ASCII in video memory.
	inc edi
	mov byte [edi], 0x4F				; White-on-red to inform the user this is a fatal error.
	inc edi
	loop .wtfloop
	
	jmp .hang							; We can't proceed further. Hang.
	
.kernel_go:
	push dword [RMGLOBAL_PCICFG]		; unsigned int pcicfg
	push dword [RMGLOBAL_EAX]			; unsigned int old_magic
	push dword [RMGLOBAL_EBX]			; multiboot_info_t* multiboot
	push eax							; unsigned int magic
	
	extern kernel_main					; Jump into the kernel's main function.
	call kernel_main
	
.hang:									; We shouldn't ever get here, but just
	cli									; in case we do somehow, lock up.
	hlt
	jmp .hang
	

global gdt_reload
extern gdtr
gdt_reload:
	lgdt [gdtr]							; Load the GDT pointer.
	
	mov ax, 0x18						; Load the kernel data selectors.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	jmp 0x10:.flush						; Load the kernel code selector.

.flush:
	ret


global idt_reload
extern idtr
idt_reload:
	lidt [idtr]							; Load the IDT pointer.
	ret
	

global rmode_call
extern gdt
rmode_call:
	push ebp
	mov ebp, esp
	
	cli									; Interrupts remain off while we're in real mode.
	
	mov eax, dword [ebp+8]				; unsigned int magic
	call RMGLOBAL_ENTRY					; Invoke the real mode subkernel.
	
	push eax							; Push the return value onto the stack just in case.
	
	call gdt_reload						; Load the kernel GDT.
	call idt_reload						; Load the protected mode IDT.
	
	mov eax, dword [gdt+0x34]			; Clear the TSS descriptor's busy flag.
	and ah, 0xFD
	mov dword [gdt+0x34], eax
	
	mov ax, 0x33						; Load the Task Register.
	ltr ax
	
	mov eax, cr0						; Re-enable paging.
	or eax, 0x80000000
	mov cr0, eax
	
	pop eax								; Retrieve the return value.
	sti									; We can safely turn interrupts back on now.
	
	mov esp, ebp
	pop ebp
	ret

section .data
rmode_subkernel:
	incbin "kernel/rmode.o"
rmode_subkernel_end:

section .rodata
global nasm_version_string
nasm_version_string:
	db "NASM ", __NASM_VER__, 0
