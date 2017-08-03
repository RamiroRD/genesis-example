;---------------------
; ClearScreen
; Clear sprites, scroll layers, horizontal scrolling.
; This uses VRAM FILL
;---------------------
ClearScreen:
		lea VDPControl,a0
		move.l #$81748F01, (a0) ; byte increment and DMA enable
		move.l #$94409300, (a0) ; $400 bytes from $C000 up to end of VRAM
		move.l #$8780,(a0)		; VRAM FILL
		move.l #$40000083,(a0)  ; Destination = $C000
		move.b #$00,(a1)
isFillOver?:
		move.w (a0),d0
		btst #1,d0
		bne isFillOver?
		move.w #$8F02,(a0)
		bra ClearVSRAM ; this will return for us

		

;---------------------
; ClearVSRAM
; Clears whole VSRAM
;---------------------
ClearVSRAM:
		moveq #39,d1
		move.l	#$40000080,(a0)
ClearVSRAM?:
		move.w	#0,(a1)
		dbf d1,ClearVSRAM?
		rts
		
;----------------------------------------------
; DMA 68K -> VRAM(VRAM,CRAM,VSRAM) (does not save regs)
; d0.l = number of words to move 
; d1.l = VRAM addr
; d2.l = destination type:
; d4.l = source addr
; VRAM $01
; CRAM $03
; VSRAM $05
DMA:		  
;*******Prepares length*******
	lsl.l #8,d0
	lsr.w #8,d0
	or.l #$94009300,d0
;*******Prepares source and DMA type*******
	lsr.l #1,d4
	move.l d4,d3
	lsl.l #8,d3
	and.l #$00FFFF00,d3
	lsr.w #8,d3    
	or.l #$96009500,d3
	swap d4
	or.w #$9700,d4
;*******Prepares destination******* 
	lsl.l #2,d1
	lsr.w #2,d1
	swap d1
	ror.l #2,d2
	lsl.w #4,d2
	or.l d1,d2 
	bset #7,d2
;*****Actual DMA******
	lea VDPControl,a0
	move.l #$81748F02, (a0) ; set increment by 2 and set DMA bit. 
	move.l d0, (a0) ; sets length
	move.l d3, (a0) ; sets low and mid source bytes
	move.w d4, (a0) ; sets highest source byte
	move.l d2, -(sp) ; DMA command (VDP memory type is here)
	move.l (sp),(a0)
	addq.l #4,sp
	move.w #$8164, (a0)
	rts