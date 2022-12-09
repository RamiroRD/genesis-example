
StartUp:
		move #$2700,sr 	
		tst.l	($A10008).l		;Test Port A control
		move.b	($A10001).l,d0		;version
		andi.b	#$F,d0
		beq	@notmss			
		move.l	#'SEGA',($A14000).l
@notmss:   
		move.l #UserStack,a6
		move a6,usp

		lea VDPControl, a6
		lea VDPData, a5
@waitfordma:
		move.w (a6), d0
		btst #1, d0
		bne.s @waitfordma
		
SetupVDP:
; Load VDP regs
		lea	VDPSetupArray, a0
		move.w	#(VDPSetupArrayEnd-VDPSetupArray)/4-1,d1	
@regs:
		move.l (a0)+, (a6)
		dbf	d1, @regs
		
; Clear VSRAM
		bsr ClearVSRAM
		
; Clear VRAM 	
		move.l #$8F018174, (a6)	; byte increment and DMA enable
		move.l #$94FF93FF, (a6)
		move.l #$40000080, (a6)  	; Destination = $0000
		
		move.w #0, $FFFF0000
		move.b $FFFF0000, (a5)			; Byte to fill with
		
; ClearRAM

		lea	($FFFF0000).l,a0
		moveq #0, d2
		move.w #$3fff,d1
@ram:
		move.l d2, (a0)+
		dbf	d1, @ram
		
; Wait for VRAM fill to finish
@waitfordma:
		move.w (a6), d0
		btst #1, d0
		bne.s @waitfordma
		move.l #$8F028164,(a6)

; Clear CRAM
		move.l	#$C0000000, (a6)
		move.w	#63,d1
@cram:
		move.w	d2, (a5)
		dbf	d1, @cram
		
; Reset PSG

		move.b	#$9F,($C00011).l
		move.b	#$BF,($C00011).l
		move.b	#$DF,($C00011).l
		move.b	#$FF,($C00011).l		
		move.w	#0,($A11200).l
		
		bra Init
	
VDPSetupArray:
		dc.w	$8006	;
		dc.w	$8164	;Genesis display = 1, DMA = 0, VBlank = 1, display = 1
		dc.w	$8230	;Scroll A - $C000
		dc.w	$8338	;Window   - $E000
		dc.w	$8407	;Scroll B - $E000
		dc.w	$857c	;Sprites  - $F800
		dc.w	$8600	;Unused
		dc.w	$8700	;Backdrop color - $00
		dc.w	$8800	;Unused
		dc.w	$8900	;Unused
		dc.w	$8A00	;H Interrupt register
		dc.w	$8B00	;Full screen scroll, no external interrupts
		dc.w	$8C81	;40 cells display
		dc.w	$8D3F	;H Scroll - $FC00
		dc.w	$8E00	;Unused
		dc.w	$8F02	;VDP auto increment
		dc.w	$9001	;64 cells scroll
		dc.w	$9100	;Window H position
		dc.w	$9200	;Window V position
		dc.w	$93FF	;DMA stuff (off)
		dc.w	$94FF	;DMA stuff (off)
		dc.w	$9500	;DMA stuff (off)
		dc.w	$9600	;DMA stuff (off)
		dc.w	$9780	;DMA stuff (off)
VDPSetupArrayEnd:
