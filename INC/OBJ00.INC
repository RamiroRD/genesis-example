
	Section Variables
	
	Section Code

; a0 ptr to object
; d0.w initial Phi value
Obj00Create:
	lea Obj00Update, a1
	move.l a1, ObjHandler(a0)
	move.w d0, ObjPhi(a0)
	moveq #0, d0
	move.w d0, ObjState(a0)
	move.w #8*(128+320/2), ObjPosX(a0)
	move.w #8*(128-30), ObjPosY(a0)
	moveq #0, d0
	move.w d0, ObjVelX(a0)
	move.w d0, ObjVelY(a0)
	rts

; a0 ptr to object
Obj00Update:
	bsr @CheckForStateChange
	move.w ObjState(a0), d0
	lsl.l #2, d0
	lea @JumpTable, a1
	add.w d0, a1
	jmp (a1)
@JumpTable:
	bra @State00
	bra @State01
	bra @State02
	
@State00:
	move.l d4, -(sp)
	moveq #0, d4
	lea SINE_TAB,a2
	move.w ObjPhi(a0), d0
	add.w #15,d0
	move.w d0, ObjPhi(a0)
	
	; Take integer part
	lsr.w #3+1,d0			; fixed point to integer (plus a division by two)
	move.w d0,d1
	
	; Compute sine
	add.w d0,d0
	andi.w #$03FE,d0	; modulo 512 *2 (LUT)
	move.w (a2,d0.w),d3	; sine 
	
	; Compute cosine
	addi.w #256,d1		; pi/2 (128) (distributivity of mult. over addition)
	andi.w #$03FE,d1	; mod 512
	move.w (a2,d1.w),d4	; cosine
	
	; Expand
	moveq #9,d2 
	asr.w d2,d3
	moveq #8,d2
	asr.w d2,d4
	
; Center
	addi.w #136+(240/2)-32, d3 ; V pos
	addi.w #96+(320/2)+32-4, d4 ; H pos
	lsl.l #3, d3
	move.w d3, ObjPosY(a0)
	asr.l #3, d3
	lsl.l #3, d3
	move.w d4, ObjPosX(a0)
	asr.l #3, d3
		
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
	
@State01:
@State02:
	move.l ObjPosX(a0), d1
	move.w d1, d0
	swap d1
	move.l ObjVelX(a0), d2
	move.w d2, d3
	swap d2
PosY equr d0
PosX equr d1
VelX equr d2
VelY equr d3
	
	cmp.w #300*8, PosY
	ble @DoNotBounce
@Bounce:
	move.w #300*8, PosY
	muls.w #6, VelY
	asr.w #3, VelY
	neg.w VelY
@DoNotBounce:
	add.w VelX, PosX
	add.w VelY, PosY
	addq.w #1, VelY
	
	move.w PosX, ObjPosX(a0)
	move.w PosY, ObjPosY(a0)
	move.w VelX, ObjVelX(a0)
	move.w VelY, ObjVelY(a0)
	
	lsr.w #3, PosX
	lsr.w #3, PosY
; Sprite Attribute Table entry
	swap d0
	move.w #$0500, d0
	swap d1
	move.w #$85F2, d1
	swap d1
	bsr SprMgrAdd
	rts

@CheckForStateChange:
	move.b Data1, d0
	btst #0, d0
	bne @NotPressed
@Pressed:
	move.w #1, ObjState(a0)
@NotPressed	
	rts
	