SystemStack EQU $FFFFFE00
UserStack EQU $FFFFFFFF

		Section Code

Vectors:dc.l SystemStack, StartUp, BusError, AddressError
		dc.l IllegalInstruction, DivByZero, CHKInstruction,	TrapV
		dc.l PrivilegeViolation, Trace, Line1010, Line1111
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l HBlank,	Error,		VBlank,	Error
		dc.l Trap00,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
		dc.l Error,	Error,		Error,	Error
Header:		dc.b 'SEGA MEGA DRIVE ' ; Console name
		dc.b '                ' ; Copyright/Date
DomesticName:	dc.b 'SEGA GENESIS TEST                               ' ; Domestic Name
		dc.b 'SEGA GENESIS TEST                               ' ; International Name
		dc.b 'GM 00000000-01'   ; Version
Checksum:	dc.w $1337
					; Checksum
		dc.b 'JUE             ' ; I/O Support
RomStartLoc:	dc.l 0			; ROM Start
RomEndLoc:	dc.l RomEnd
					; ROM End
RamStartLoc:	dc.l $FF0000		; RAM Start
RamEndLoc:	dc.l $FFFFFF		; RAM End

		dc.b $20,$20,$20,$20	; 'RA',$F8,$20 if SRAM = on
		
SramStart:	dc.l $20202020		; $200000 if SRAM = on
SramEnd:	dc.l $20202020		; $20xxxx if SRAM = on
		
		dc.b '                                              ' ; Notes
		dc.b '      '
		dc.b 'JUE             ' ; Country
EndOfHeader:

		Section Variables
VCounter ds.w 1
MCounter ds.w 1
Phi      ds.w 1
		
;---------------------
; Code start
;---------------------

		Section Code

		INCLUDE INC/MAINCPU.INC
		INCLUDE INC/STARTUP.INC	
		INCLUDE INC/ERR_HNDL.INC
		INCLUDE INC/COMMON.INC		
		INCLUDE INC/MEMPOOL.INC
		INCLUDE INC/SPRMGR.INC
		INCLUDE INC/OBJECT.INC
		INCLUDE INC/OBJ00.INC
		
Init:
		move.l #(VRAM_DATA_END-VRAM_DATA)/2,d0
		moveq #0,d1
		moveq #1,d2
		move.l #VRAM_DATA,d4
		bsr DMA
		
		move.l #$6000,d0
		moveq #0,d1
		moveq #1,d2
		move.l #VRAM_01,d4
		bsr DMA
		
		move.l #2048,d0
		move.w #$E000,d1
		moveq #1,d2
		move.l #PLANE_B,d4
		bsr DMA
		
	
		move.l #64,d0
		moveq #0,d1
		moveq #3,d2
		move.l #CRAM_01,d4
		bsr DMA
		
		moveq #0, d0
		move.w d0, MCounter
		move.w d0, Phi
		addq #1, d0
		move.w d0, VCounter

		bsr MemoryPoolInit
		moveq #32, d7
		move.w #(1024), d6
		moveq #0, d5
@CreateObjects:
		bsr MemoryPoolAlloc
		addq #ElemContents, a0
		move.w d5, d0
		add.w d6, d5
		bsr Obj00Create	
		dbf d7, @CreateObjects
		
@EnableInterrupts
		move #$0000,sr ; Switch to user mode and unmask interrupts
		
;-----------------------
; Loop principal
;-----------------------
@idle:
		bra @idle

;---------------------
; Horizontal Blank
;---------------------
HBlank:
		
		rte
				
;---------------------
; Vertical Blank
;---------------------
VBlank:
		movem.l d0-a6,-(a7)
		
		move.w VCounter,d7
		addq.w #1,d7
		move.w d7,VCounter

		; Write horizontal scroll for Plane B
		move.l #$7C020003,VDPControl
		neg.w d7
		move.w d7,VDPData ; H pos
		
		; Write vertical scroll for Plane B
		move.l #$40020010,VDPControl
		move.w #8*10,d4
		move.w d4, VDPData ;V pos
		
		bsr UpdateObjects
		bsr SprMgrFlush
@skip:
		movem.l (a7)+, d0-a6
		rte
;---------------------

;---------------------
VRAM_01:
		INCBIN INC/VRAM.RAM
		even
VRAM_01_END:
CRAM_01:
		INCBIN INC/CRAM.RAM
		even
CRAM_01_END:
PLANE_B:
		INCBIN INC/PLANEB.RAM
		even
PLANE_B_END:
		INCLUDE INC/SINE_TAB.INC
		even
		INCLUDE INC/CRAM_DAT.INC
		even
		INCLUDE INC/VRAM_DAT.INC
		even
ASCII:
		INCLUDE INC/ASCII.INC
		even


RomEnd:
