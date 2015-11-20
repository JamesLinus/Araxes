;; BlacklightEVO boot/evofs/mbr.asm -- quick and shitty MBR
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; The Blacklight project is under the terms of the ISC license. See license.md for details.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net


; Standard assumptions made here, nothing fancy

	org 0x600
	bits 16

entry:
	cli
	mov ax, 0x1000
	mov ss, ax
	mov sp, 0xB000
	xor ax, ax
	mov ds, ax
	mov es, ax
	sti
	
	mov si, 0x7C00
	mov di, 0x0600
	mov cx, 0x200
	rep movsb
	
	jmp 0:rebase
	
rebase:
	mov byte [bootdevice], dl
	
	mov ah, 0x0E
	mov al, 'M'
	int 0x10
	
	mov si, 5
	
parseMBR:
	xor ax, ax
	mov bx, 0x7BE
	mov al, byte [bx]
	bt ax, 7
	jc foundPartition
	
	add bx, 0x10
	mov al, byte [bx]
	bt ax, 7
	jc foundPartition
	
	add bx, 0x10
	mov al, byte [bx]
	bt ax, 7
	jc foundPartition
	
	add bx, 0x10
	mov al, byte [bx]
	bt ax, 7
	mov ah, 0x66				; As far as I know, no BIOS uses 0x66 for any
	jnc panic					; int 13h errors, so we'll use it for this
	
foundPartition:
	
	mov ah, 0x0E
	mov al, '.'
	int 0x10					; At this point we know a partition exists, so
	mov eax, dword [bx+8]		; we're going to grab the starting LBA and
	mov dword [diskpkt+8], eax	; shove it into an int 13/42 packet
	mov ah, 0x42
	mov dl, byte [bootdevice]
	mov si, diskpkt
	int 0x13
	jc panic
	
	mov ah, 0x0E
	mov al, '.'
	int 0x10
	
	mov dl, byte [bootdevice]
	jmp 0:0x7C00
	
panic:							; We need to save space so we only have one
	push ax						; error message, but we also dump AH after
	mov si, err
	mov ah, 0x0E
	
.ohno:
	lodsb
	or al, al
	jz .done
	int 0x10
	jmp .ohno
	
.done:							; Hic sunt dracones
	pop ax
	mov al, ah
	push ax
	shr al, 4
	cmp al, 10
	sbb al, 0x69
	das
	mov ah, 0x0E
	int 0x10
	
	pop ax
	and al, 0x0F
	cmp al, 10
	sbb al, 0x69
	das
	mov ah, 0x0E
	int 0x10
	cli
	hlt
	
crudStorage:
	bootdevice db 0
	err db "Error in MBR, halting", 13, 10, 0
	diskpkt	db 0x10				; Packet size
			db 0				; Reserved
			dw 1				; Number of sectors to copy
			dd 0x00007C00		; Buffer to copy to
			dq 0				; LBA of starting sector
	db 0xAA
	
padsig:
	times 510-($-$$) db 0
	db 0x55, 0xAA
