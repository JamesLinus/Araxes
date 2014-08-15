;; BlacklightEVO rmode.asm -- real mode subkernel
;; Copyright (c) 2013 The Cordilon Group
;; Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net


; Okay, here's the deal. I hate this code. It's awful, it's left over from the
; worst OS code I've ever written, and it's probably going to get me burned at
; the stake one day. It wrecks GDT/IDT (something I need to work on). But it's
; a necessity.
;
; Here's how you call it:
; 1. Copy the code to 0x3000.
; 2. Disable interrupts.
; 3. Place parameters in vbe_x, vbe_y, and vbe_depth.
; 4. Call 0x3000.
; 5. Check vbe_status and vbe_framebuffer for the result.



bits 32
org 3000h

%define VGA_TYPE_GENERIC 0
%define VGA_TYPE_VBE 3

%macro dprint 1+
section .data
	%%string db %1, 0
section .text

	pusha
	mov si, %%string
	mov bx, 0007h
	call direct_print
	popa
%endmacro

kernel_stack_pointer equ 5000h
vbe_status equ 5010h
vbe_x equ 5020h
vbe_y equ 5030h
vbe_depth equ 5040h
vbe_framebuffer equ 5050h
vbe_modelist equ 5060h

vbekern_entry:
	pushad
	mov [kernel_stack_pointer], esp
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
	
	mov eax, cr0								; At this point we're copied to 1000:0000 for simplicity.
	and al, 0FEh								; However, we're still in protected mode!
	mov cr0, eax
	
	mov ax, 0000h								; Flush CS, bringing us to real mode properly.
	push ax										; Kinda, that is. We're in unreal mode.
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
	
	mov sp, 2F00h
	
	o32 lidt [_real_idt]
	
	mov dword [vbe_status], 0
	
	cmp word [vbe_x], 1
	jne .do_we_dump
	
	mov cx, word [vbe_x]
	call vbe_switch_mode
	jnc .done
	
	mov dword [vbe_status], 1
	jmp .done
	
.do_we_dump:
	cmp word [vbe_x], 0
	jne .set_a_mode
	
	cmp word [vbe_y], 0
	jne .set_a_mode
	
	cmp word [vbe_depth], 0
	jne .set_a_mode
	
.dump_modes:
	call list_vbe_modes
	
	mov dword [vbe_lfb], 5060h
	mov word [vbe_status], 5060h
	jmp .done
	
	pusha
	push es					; Save ES and set it to CS.
	push cs
	pop es
	
	mov bp, vbe_modelist
	
	mov ax, 4F00h				; VBE "Get SuperVGA Information"
	mov di, 0x8000
	mov ebx, "2EBV"
	mov dword [di], ebx
	int 10h
	pop es
	popa
	pusha
	
	
	push ds
	push cs
	pop ds
	mov si, 0x8000
	add si, 0Eh
	push ebp
	mov ebp, dword [si]
	mov si, bp
	rol ebp, 16
	mov ds, bp
	pop ebp
	
	push es
	push cs
	pop es
	
.searchloop:
	mov cx, word [si]
	mov ax, 4F01h
	mov di, 0x6000
	int 10h
	
	mov di, 0x6000
	mov ax, word [es:di]
	bt ax, 7
	jnc .skip
	
	mov [bp], cx
	mov dx, cx
	call direct_print_dw
	add bp, 2
	mov dx, word [es:di+12h]
	mov [bp], dx
	add bp, 2
	mov dx, word [es:di+14h]
	mov [bp], dx
	add bp, 2
	movzx dx, byte [es:di+19h]
	mov [bp], dx
	add bp, 2
	pusha
	mov ax, 0x0E74
	int 10h
	popa
		

.skip:
	pusha
	mov ax, 0x0E54
	int 10h
	popa
	add si, 2
	cmp word [si], 0FFFFh
	jne .searchloop
	
	mov word [bp], 0FFFFh

;.done:
	pop es
	pop ds
	popa
	
	mov eax, vbe_modelist
	mov word [vbe_status], ax
	mov dword [vbe_lfb], eax
	jmp .done
	
.set_a_mode:
	; dprint `X: `
	; mov dx, word [vbe_x]
	; call direct_print_dec
	
	; dprint `  Y: `
	; mov dx, word [vbe_y]
	; call direct_print_dec
	
	; dprint `  BPP: `
	; movzx dx, byte [vbe_depth]
	; call direct_print_dec
	
	; dprint `\n`
	
	;call list_vbe_modes
	
	;cli
	;hlt
	
	; dprint `\n`
	
	mov cx, word [vbe_x]
	mov dx, word [vbe_y]
	mov bl, byte [vbe_depth]
	call vbe_find_mode
	
	
	
	; pusha
	; dprint `  Mode: `
	; mov dx, cx
	; call direct_print_dec
	; popa
	
	cmp cx, 0
	je .done
	
	call vbe_switch_mode
	jnc .done
	
	mov dword [vbe_status], 1
	
.done:
	;mov ax, 0x0003
	;int 10h
	
	mov eax, dword [vbe_lfb]
	mov dword [vbe_framebuffer], eax
	
	;dprint `  Framebuffer: 0x`
	;mov edx, dword [vbe_framebuffer]
	;call direct_print_dd
	
	;cli
	;hlt
	
	lgdt [_gdt_ptr]								; Can never be too sure.
	mov eax,cr0
	or al,1
	mov cr0,eax
	jmp KRNL_CODE_SEL:.back_pmode
	
bits 32
	
.back_pmode:
	mov ax, KRNL_DATA_SEL
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov esp, [kernel_stack_pointer]
	
	popad
	ret
	
	cli
	hlt
	
bits 16
	
msg db "Hello, world!", 0

list_vbe_modes:						; CX=X, DX=Y, BL=bpp
	dprint `Listing all VBE modes with LFB support.\n\n`
	pusha
	push es					; Save ES and set it to CS.
	push cs
	pop es
	
	mov ax, 4F00h				; VBE "Get SuperVGA Information"
	mov di, 0x8000
	mov ebx, "2EBV"
	mov dword [di], ebx
	int 10h
	pop es
	popa
	pusha
	
	
	push ds
	push cs
	pop ds
	mov si, 0x8000
	add si, 0Eh
	mov ebp, dword [si]
	mov si, bp
	rol ebp, 16
	mov ds, bp
	
	push es
	push cs
	pop es
	
	mov ebp, vbe_modelist
	
.searchloop:
	mov cx, word [si]
	mov ax, 4F01h
	mov di, 0x6000
	int 10h
	
	mov di, 0x6000
	mov ax, word [es:di]
	bt ax, 7
	jnc .skip
	
	mov dx, cx
	mov [bp], dx
	add bp, 2
	;call direct_print_dw
	;dprint `h: `
	mov dx, word [es:di+12h]
	mov [bp], dx
	add bp, 2
	;call direct_print_dec
	;dprint `x`
	mov dx, word [es:di+14h]
	mov [bp], dx
	add bp, 2
	;call direct_print_dec
	;dprint `x`
	movzx dx, byte [es:di+19h]
	mov [bp], dx
	add bp, 2
	;call direct_print_dec
	;dprint `\n`
		

.skip:
	add si, 2
	cmp word [si], 0FFFFh
	jne .searchloop
	
	mov dx, 0FFFFh
	mov [bp], dx
	add bp, 2

.done:
	pop es
	pop ds
	popa
	ret

direct_print_db:
	pusha
	mov al, dl

	push ax
	shr al, 4
	cmp al, 10
	sbb al, 69h
	das
	mov bx, 0007h
	mov ah, 0Eh
	int 10h
	pop ax

	and al, 0Fh
	cmp al, 10
	sbb al, 69h
	das
	mov bx, 0007h
	mov ah, 0Eh
	int 10h

	popa
	ret


;;
; Writes an word to the screen in hexidecimal.
; @param DX Word to print to the screen.
;;
direct_print_dw:
	pusha
	push dx
	mov dl, dh
	call direct_print_db
	pop dx
	call direct_print_db
	popa
	ret
	
;;
; Writes a dword to the screen in hexidecimal.
; @param EDX Dword to print to the screen.
;;
direct_print_dd:
	pushad
	push edx
	shr edx, 16
	call direct_print_dw
	pop edx
	call direct_print_dw
	popad
	ret
	
direct_itoa:
	pusha
	push es
	push cs
	pop es

	mov cx, 0
	mov bx, 10				; Set BX 10, for division and mod
	mov di, .t				; Get our pointer ready

.push:
	mov dx, 0
	div bx					; Remainder in DX, quotient in AX
	inc cx                          	; Increase pop loop counter
	push dx					; Push remainder, so as to reverse order when popping
	test ax, ax				; Is quotient zero?
	jnz .push				; If not, loop again
.pop:
	pop dx					; Pop off values in reverse order, and add 48 to make them digits
	add dl, '0'				; And save them in the string, increasing the pointer each time
	mov [es:di], dl
	inc di
	dec cx
	jnz .pop

	mov byte [es:di], 0			; Zero-terminate string

	pop es
	popa
	mov si, .t				; Return location of string
	ret

	.t times 7 db 0
	
direct_print_dec:
	pusha
.printloop:
	mov ax, dx
	cmp ax, 0
	je .zero

	mov ax, dx
	call direct_itoa
	push bx
	mov bx, 0007h
	push ds
	push cs
	pop ds
	call direct_print
	pop ds
	pop bx

	popa
	ret
.zero:
	mov ah, 0Eh
	mov bx, 0007h
	mov al, '0'
	int 10h
	popa
	ret
	
direct_print:
	pusha
.loop:
	lodsb
	cmp al, 0Ah
	je .newline
	cmp al, 0
	je .done
	
.print:
	mov ah, 0Eh
	int 10h
	jmp .loop
	
.newline:
	mov ah, 0Eh
	mov al, 0Dh
	int 10h
	mov al, 0Ah
	int 10h
	jmp .loop
	
.done:
	popa
	ret
	
vbe_detect:
	push es					; Save ES and set it to CS.
	push cs
	pop es
	
	mov ax, 4F00h				; VBE "Get SuperVGA Information"
	mov di, 0x8000			; Perhaps 0x8000 should be renamed... :|
	mov ebx, "VBE2"				; We need to set offset 0 of the buffer to "VBE2" in order
	mov dword [di], ebx			; to get VBE 2.0+ info
	int 10h
	pop es
	
	cmp al, 4Fh				; If AL != 4Fh, VBE isn't implemented.
	jne .nope
	
	cmp ah, 0				; AH being nonzero means the buffer couldn't be filled for some reason.
	jne .nope
	
	mov bx, 0x8000
	mov byte [vbe_exists], 1		; So now we know we have a card that supports VBE 2.0+.
	
	mov byte [vga_type], VGA_TYPE_VBE	; Inform the generic text mode stuff that we have a vbe card.
	
	mov ax, word [cs:bx+4]			; What the fuck is THIS shit?!
	mov byte [vbe_bios_ver_major], ah	; Oh, right, BIOS version numbers.
	mov byte [vbe_bios_ver_minor], al
	
	mov ax, word [cs:bx+12h]		; So apparently you can do [CS:BX+n]...
	mov byte [vbe_memory], al		; The low and high bytes need to be split up.
	mov byte [vbe_memory_high], ah
	
	mov dword [vbe_lfb], 0			; Clear the linear framebuffer address.
	jmp .done
	
.nope:
	mov byte [vga_type], VGA_TYPE_GENERIC	; If we get here, we know we don't have a VBE 2.0+ card.
	
	mov byte [vbe_exists], 0
	mov byte [vbe_bios_ver_major], 0
	mov byte [vbe_bios_ver_minor], 0
	mov byte [vbe_memory], 0
	mov byte [vbe_memory_high], 0
	mov dword [vbe_lfb], 0
	mov word [vbe_size], 0
	mov byte [vbe_shadow_enabled], 0

.done:
	ret					; NOTE TO SELF: There is no POP ES for a reason (line 30)
	
	
vbe_find_mode:					; CX=X, DX=Y, BL=bpp
	pusha
	push es					; Save ES and set it to CS.
	push cs
	pop es
	
	mov word [cs:.x], cx
	mov word [cs:.y], dx
	mov byte [cs:.bpp], bl
	
	mov ax, 4F00h				; VBE "Get SuperVGA Information"
	mov di, 0x8000
	mov ebx, "2EBV"
	mov dword [es:di], ebx
	int 10h
	pop es
	popa
	pusha
	
	
	push ds
	push cs
	pop ds
	mov si, 0x8000
	add si, 0Eh
	mov ebp, dword [es:si]
	mov si, bp
	rol ebp, 16
	mov ds, bp
	
	push es
	push cs
	pop es
	
.searchloop:
	mov cx, word [es:si]
	mov ax, 4F01h
	mov di, 0x6000
	int 10h
	cmp ah, 0
	jne .skip
	
	mov di, 0x6000
	mov ax, word [es:di]
	bt ax, 7
	jnc .skip
	mov ax, word [cs:.x]
	cmp word [es:di+12h], ax
	jne .skip
	mov ax, word [cs:.y]
	cmp word [es:di+14h], ax
	jne .skip
	mov al, byte [cs:.bpp]
	cmp byte [es:di+19h], al
	jne .skip
	
	mov word [cs:.mode], cx
	jmp .done

.skip:
	add si, 2
	cmp word [es:si], 0FFFFh
	jne .searchloop
	
	mov word [cs:.mode], 0

.done:
	pop es
	pop ds
	popa
	mov cx, [cs:.mode]
	ret
	
	
.x	dw 0
.y	dw 0
.bpp	db 0
.mode	dw 0
	
vbe_switch_mode:
	push ds
	push es
	push cs
	push cs
	pop ds
	pop es
	
.definitely_vesa:
	or cx, 4000h				; Make sure we're requesting LFB access
	
	push cx
	mov ax, 4F01h				; Get video mode info
	mov di, 0x8000
	int 10h
	
	cmp ah, 0				; If AH == 0, we're good to go.
	je .go
	
	stc
	pop es
	pop ds
	ret
	
.go:
	mov bx, 0x8000			; Start grabbing info
	
	mov ax, word [bx+12h]			; Figure this bumblefuck out yourself.
	mov word [cs:vbe_width], ax
	
	div byte [cs:font_width]
	mov byte [cs:vga_columns], al
	mov byte [cs:vbe_text_width], al
	
	mov ax, word [bx+14h]
	mov word [cs:vbe_height], ax
	
	div byte [cs:font_height]
	mov byte [cs:vga_rows], al
	mov byte [cs:vbe_text_height], al
	
	mov al, byte [bx+19h]
	mov byte [cs:vbe_bpp], al
	
	push es
	mov ax, 40h
	mov es, ax
	
	mov ah, 0
	mov al, byte [cs:vga_columns]
	mov word [004Ah], ax
	
	mov ah, 0
	mov al, byte [cs:vga_rows]
	dec al
	mov word [0084h], ax
	
	mov al, byte [cs:vga_columns]
	mul byte [cs:vga_rows]
	shl ax, 1
	mov word [004Ch], ax
	mov word [cs:vbe_size], ax
	
	pop es
	
	mov eax, dword [bx+28h]			; Save the LFB address
	mov dword [cs:vbe_lfb], eax

	mov byte [vbe_shadow_enabled], 1
	
	pop bx
	mov ax, 4F02h				; Set the goddamn mode, finally.
	int 10h					; If your monitor implodes and destroys your innards, it's not my fault.
	
	clc					; Carry clear for success
	pop es					; Restore original segment registers
	pop ds
	ret					; ...and return!
	
vbe_exists		db 0
vbe_mode		dw 0
vbe_bios_ver_major	db 0
vbe_bios_ver_minor	db 0
vbe_memory		db 0
vbe_memory_high		db 0
vbe_lfb			dd 0
vbe_size		dw 0
vbe_shadow_enabled	db 0
vbe_bpp			db 0
vbe_cursor_x		db 0
vbe_cursor_y		db 0
vbe_doublebuffer	dd 0
vbe_text_width		db 0
vbe_text_height		db 0
vbe_width		dw 0
vbe_height		dw 0

font_width		db 8
font_height		db 16
font_chars		dw 256

vga_type db 0
vga_columns db 0
vga_rows db 0

_real_idt:
	dw 1023
	dd 0
	
_gdt:
	dd 0
	dd 0

KRNL_CODE_SEL	EQU ($ - _gdt)
_gdt_krnl_cs:
	dw 0FFFFh
	dw 0			; base; gets set above
	db 0
	db 9Ah			; present, ring 0, code, non-conforming, readable
	db 0CFh			; 32-bit
	db 0

KRNL_DATA_SEL	EQU ($ - _gdt)
_gdt_krnl_ds:
	dw 0FFFFh
	dw 0			; base; gets set above
	db 0
	db 92h			; present, ring 0, data, expand-up, writable
	db 0CFh
	db 0

_16BIT_CODE_SEL	EQU ($ - _gdt)
_gdt_16bit_cs:
	dw 0FFFFh
	dw 0			; base; gets set above
	db 0
	db 9Ah			; present, ring 0, code, non-conforming, readable
	db 0			; 16-bit
	db 0

_16BIT_DATA_SEL	EQU ($ - _gdt)
_gdt_16bit_ds:
	dw 0FFFFh
	dw 0			; base; gets set above
	db 0
	db 92h			; present, ring 0, data, expand-up, writable
	db 0
	db 0
_gdt_end:

_gdt_ptr:
	dw _gdt_end - _gdt - 1	; GDT limit
	dd _gdt			; linear adr of GDT; gets set above
