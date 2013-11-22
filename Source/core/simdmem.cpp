// BlacklightEVO core\simdmem.cpp -- SIMD mem* functions (MMX, SSE, SSE2, etc.)
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\mm.h"

void simd_memcpy_mmx(void* dest, void* src, size_t num) {
	//void* fpu_state = malloc(108);
	_asm {
		//fnsave [fpu_state]
		mov esi, src
		mov edi, dest
		mov ecx, num

    mov eax,ecx
    shr ecx,6
    mov ebx,ecx
    shl ebx,6
    sub eax,ebx

	cmp ecx,0
	je done

align 16
             
    lloop:

        movq mm0,[esi]
        movq mm1,[esi+8]
        movq mm2,[esi+16]
        movq mm3,[esi+24]
        movq mm4,[esi+32]
        movq mm5,[esi+40]
        movq mm6,[esi+48]
        movq mm7,[esi+56]
        movq [edi],mm0
        movq [edi+8],mm1
        movq [edi+16],mm2
        movq [edi+24],mm3
        movq [edi+32],mm4
        movq [edi+40],mm5
        movq [edi+48],mm6
        movq [edi+56],mm7
        add esi,8*8
        add edi,8*8
        dec ecx
	jnz lloop

	done:		
    mov ecx,eax
    rep movsb

    emms
		//frstor [fpu_state]
	}
	//free(fpu_state);
}