
StartUp:
		tst.l	($A10008).l		;Test Port A control
		bne	PortA_Ok
PortA_Ok:
		bne	SkipSetup

		move.b	($A10001).l,d0		;version
		andi.b	#$F,d0
		beq	SkipSecurity			
		move.l	#'SEGA',($A14000).l

SkipSecurity:   

		move.w	($C00004).l,d0		;test if VDP works
		
		move.l #UserStack,a6
		move a6,usp
		
;---------------------
; Registros VDP
;---------------------
		lea	(VDPSetupArray),a0
		lea VDPControl, a1
		move.w	#(VDPSetupArrayEnd-VDPSetupArray)/2-1,d1	
VDPSetupLoop:
		move.w (a0)+, (a1)
		dbf	d1, VDPSetupLoop
		
		bsr ClearVSRAM
		
; Clear VRAM 	
		; This uses VRAM FILL
		
		move.l #$81748F01, (a1)	; byte increment and DMA enable
		move.l #$94FF93FF, (a1)	; 
		move.l #$8780, (a1)		; VRAM FILL
		move.l #$40000080, (a1)  	; Destination = $0000
		move.b #$00, (a1)			; Byte to fill with

; ClearRAM

		lea	($FFFF0000).l,a0
		move.w	#$3fff,d1
		
ClearRAM:
		move.l	#0,(a0)+
		dbf	d1,ClearRAM
		
		lea VDPControl, a0
@waitfordma:
		move.b (a0),d0
		btst #1,d0
		beq @waitfordma
		move.w #$8F02,(a0)

		move.l	#$C0000000, (a0)
		move.w	#63,d1
		
ClearCRAM:
		move.w	#0,VDPData
		dbf	d1,ClearCRAM
				

;---------------------
; PSG
;---------------------	

		move.b	#$9F,($C00011).l
		move.b	#$BF,($C00011).l
		move.b	#$DF,($C00011).l
		move.b	#$FF,($C00011).l
		
		
		move.w	#0,($A11200).l
		
;--------------------
SkipSetup:
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
