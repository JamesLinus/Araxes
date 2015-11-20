;; BlacklightEVO kernel/isr.asm -- interrupt blood seal
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; The Blacklight project is under the terms of the ISC license. See license.md for details.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net

; Macros

%macro spawn_isr 1
	global int_isr_%1
	int_isr_%1:
		cli
		push byte 0
		push byte %1
		jmp idt_isr_stub
%endmacro

%macro spawn_isr_error 1
	global int_isr_%1
	int_isr_%1:
		cli
		push byte %1
		jmp idt_isr_stub
%endmacro

%macro spawn_isr_irq 2
	global int_irq_%1
	int_irq_%1:
		cli
		push byte 0
		push byte %2
		jmp idt_isr_stub
%endmacro

extern idt_isr_handler
idt_isr_stub:
	pusha
	
	push ds
	push es
	push fs
	push gs
	
	mov ax, 0x18
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	
	mov eax, idt_isr_handler
	call eax
	
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	
	popa
	add esp, 8
	iret


int_exception_set:
	spawn_isr 0
	spawn_isr 1
	spawn_isr 2
	spawn_isr 3
	spawn_isr 4
	spawn_isr 5
	spawn_isr 6
	spawn_isr 7
	spawn_isr_error 8
	spawn_isr 9
	spawn_isr_error 10
	spawn_isr_error 11
	spawn_isr_error 12
	spawn_isr_error 13
	spawn_isr_error 14
	spawn_isr 15
	spawn_isr 16
	spawn_isr 17
	spawn_isr 18
	spawn_isr 19
	spawn_isr 20
	spawn_isr 21
	spawn_isr 22
	spawn_isr 23
	spawn_isr 24
	spawn_isr 25
	spawn_isr 26
	spawn_isr 27
	spawn_isr 28
	spawn_isr 29
	spawn_isr 30
	spawn_isr 31

int_irq_set:
	spawn_isr_irq 0, 32
	spawn_isr_irq 1, 33
	spawn_isr_irq 2, 34
	spawn_isr_irq 3, 35
	spawn_isr_irq 4, 36
	spawn_isr_irq 5, 37
	spawn_isr_irq 6, 38
	spawn_isr_irq 7, 39
	spawn_isr_irq 8, 40
	spawn_isr_irq 9, 41
	spawn_isr_irq 10, 42
	spawn_isr_irq 11, 43
	spawn_isr_irq 12, 44
	spawn_isr_irq 13, 45
	spawn_isr_irq 14, 46
	spawn_isr_irq 15, 47
