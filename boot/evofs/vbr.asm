;; BlacklightEVO boot/evofs/vbr.asm -- Volume Boot Record for EVOfs drives
;; Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
;; Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
;; Questions? Comments? Concerns? Email us: blacklight@cordilon.net


; The main point of this tiny-ass program is to get the second stage
; bootloader out of its protected block, test to make sure it hasn't been
; corrupted (fail if it has), and transfer control to it.
;
; We start at 0x0000:0x7C00 in real mode with no verifiably good stack and
; the boot drive identifier in DL.
;
; NOTE: This VBR makes some assumptions. Namely, that the hard drive will
; actually respond within five attempts when reading the MBR and that the
; BIOS supports the int 13h extensions enough to read the second stage
; loader into memory. Oh, and it assumes there's a 386 or better being used,
; since I can't imagine anyone who knows what BlacklightEVO is would attempt
; to run it on anything less.
;
; -- TODO -- actually check the file system info block

	org 0x7C00
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
	
	mov byte [bootdevice], dl
	
	mov ah, 0x0E
	mov al, 'V'
	int 0x10
	
	mov si, 5
loadMBR:						; We have no idea where the MBR loaded itself
	;mov ax, 0x0201				; to and there's no polite way to ask, so we
	;mov cx, 0x0001				; have to do it again
	;mov dh, 0
	mov ah, 0x42
	mov dl, byte [bootdevice]
	mov si, diskpkt
	int 0x13
	jnc parseMBR
	
	;dec si
	;jz loadMBR
	jmp panic
	
parseMBR:
	xor ax, ax
	mov bx, 0x81BE
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
	
foundPartition:					; At this point we know a partition exists, so
	push bx						; we're going to grab the starting LBA, add 1
	mov eax, dword [bx+8]		; to it (we want LBA 1, not LBA 0), and shove
	inc eax						; it into an int 13/42 packet
	mov dword [diskpkt+8], eax
	mov ah, 0x42
	mov dl, byte [bootdevice]
	mov si, diskpkt
	int 0x13
	jc panic
	
	pop bx						; We also inform stage2 what partition we've
	sub bx, 0x81BE				; booted from
	shr bl, 4
	mov byte [bootpart], bl
	
	;mov edx, 0xC413E5F7			; Checksum the 60 sectors we just loaded
	;mov cx, 0x7800
	;mov si, 0x8000
	
;checksumLoop:
	;xor eax, eax
	;lodsb
	;add edx, eax
	;ror edx, 1
	;loop checksumLoop
	
	;mov eax, dword [entry+0x15C]
	;cmp eax, edx
	;je .pass
	
	;mov ah, 0x67				; OH NO THE CHECKSUM FAILED
	;jmp panic					; Who knows why, but bail anyways
	
;.pass:
	mov dl, byte [bootdevice]
	mov bl, byte [bootpart]
	jmp 0:0x8000
	
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
	bootpart db 0
	err db "Error in VBR, halting", 13, 10, 0
	diskpkt	db 0x10				; Packet size
			db 0				; Reserved
			dw 60				; Number of sectors to copy
			dd 0x00008000		; Buffer to copy to
			dq 0				; LBA of starting sector
	db 0xAA
	
padsig:
	times 510-($-$$) db 0
	db 0x55, 0xAA
