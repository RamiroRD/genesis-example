
	Section Variables
	
	Section Code

CPosX00	equ (50+128)*8
CPosX01	equ (320-50+128)*8
CPosY	equ (240/2+128)*8
CInitialTimerVal equ 100

; a0 ptr to object
Obj01Create:
	lea Obj01Update, a1
	move.l a1, ObjHandler(a0)
	moveq #1, d0
	move.w d0, ObjState(a0)
	move.w #CPosX00, ObjPosX(a0)
	move.w #CPosY, ObjPosY(a0)
	moveq #0, d0
	move.w d0, ObjVelX(a0)
	move.w d0, ObjVelY(a0)
	move.w #CInitialTimerVal, ObjTimer1(a0)
	rts

TickTimer:
	move.w ObjTimer1(a0), d0
	subq #1, d0
	bne @DontChange
	move.w ObjState(a0), d1
	eor.w #1, d1
	move.w d1, ObjState(a0)
	move.w #CInitialTimerVal, d0
@DontChange:
	move.w d0, ObjTimer1(a0)
	rts
; a0 ptr to object
Obj01Update:
	bsr TickTimer
	move.w ObjState(a0), d0
	lsl.l #2, d0
	lea @JumpTable, a1
	add.w d0, a1
	jmp (a1)
@JumpTable:
	bra @State00
	bra @State01
	dc.w $FFFF
@State00:
	move.w #CPosX00, d3
	bra.s AccelerateTowards
@State01:
	move.w #CPosX01, d3
	bra.s AccelerateTowards
	nop
AccelerateTowards:
; Compute velocity
	move.w ObjPosX(a0), d0 ; low is Y, high is X
	sub.w d0, d3 ;  refX - posX
	asr.w #3, d3 ; gain
	move.w d3, ObjVelX(a0)
; Integrate position
	add.w d3, d0
	move.w d0, ObjPosX(a0) ; store posX
; Sprite Attribute Table entry
	move.w d0, d1
	move.w #CPosY/8, d0
	swap d0
	move.w #$0500, d0
	swap d1
	move.w #$85F2, d1
	swap d1
	lsr.w #3, d1
	bsr SprMgrAdd ; will return for us
	rts
