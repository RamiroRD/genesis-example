
	Section Variables
	
	Section Code

; a0 ptr to object
; d0.w initial Phi value
Obj00Create:
	lea Obj00Update, a1
	move.l a1, ObjHandler(a0)
	move.w d0, ObjPhi(a0)
	rts
	
Obj00Update:
	move.l d4, -(sp)
	moveq #0, d4
	lea SINE_TAB,a2
	move.w ObjPhi(a0), d0
	add.w #32,d0
	move.w d0, ObjPhi(a0)
	
	; Take integer part
	lsr.w #3+1,d0			; fixed point to integer (plus a division by two)
	move.w d0,d0		; multiplying by two for LUT
	move.w d0,d1
	
	; Compute sine
	add.w d0,d0
	andi.w #$03FF,d0	; modulo 512 *2 (LUT)
	move.w (a2,d0.w),d3	; sine 
	
	; Compute cosine
	addi.w #256,d1		; pi/2 (128) (distributivity of mult. over addition)
	andi.w #$03FF,d1	; mod 512
	move.w (a2,d1.w),d4	; cosine
	
	; Expand
	moveq #9,d2 
	asr.w d2,d3
	moveq #8,d2
	asr.w d2,d4
	
; Center
	addi.w #136+(240/2)-32, d3 ; V pos
	addi.w #96+(320/2)+32-4, d4 ; H pos
		
	; Write sprite attribute table entry
	move.w #$0500, d0
	swap d0
	move.w d3, d0
	swap d0 ; first four bytes
	move.w #$85F2, d1
	swap d1
	move.w d4, d1
	bsr SprMgrAdd
	move.l (sp)+, d4
	rts