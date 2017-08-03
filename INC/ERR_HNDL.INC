; None of this vectors are supposed to be able to recover the program
; Therefore SSP will be reset here so we can make subroutine calls
; The program MIGHT be able to continue if the interrupt happened during user exec

SSPBackupAddress EQU $FFFF2000
LeftMargin EQU 3
UpperMargin EQU 3
FirstColumn EQU LeftMargin
SecondColumn EQU FirstColumn+19


BusError:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #BusErrorStr,-(sp)
		bra HandleError
AddressError:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #AddressErrorStr,-(sp)
		bra HandleError
IllegalInstruction:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #IllegalInstructionStr,-(sp)
		bra HandleError
DivByZero:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #DivByZeroStr,-(sp)
		bra HandleError
CHKInstruction:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #CHKInstructionStr,-(sp)
		bra HandleError
TrapV:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #TrapVStr,-(sp)
		bra HandleError
PrivilegeViolation:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #PrivilegeViolationStr,-(sp)
		bra.s HandleError
Trace:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #TraceStr,-(sp)
		bra HandleError
Line1010:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #Line1010Str,-(sp)
		bra HandleError
Line1111:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #Line1111Str,-(sp)
		bra HandleError
Trap00: 
		move #$2700,sr
		nop
		bra.s Trap00
		rte
Error:
		move.l sp,SSPBackupAddress
		lea SystemStack,sp
		move.l #ErrorStr,-(sp)
		bra HandleError
		
HandleError:
		move.w #$2700,sr
		movem.l d0-a6,-(sp)
		
		bsr PrepareErrorScreen
		
		movea.l (15*4)(sp),a2	; Error message address
		moveq #LeftMargin,d6
		moveq #UpperMargin,d7
		bsr PrintMessage
		
		bsr PrintRegisters
		
		moveq #0,d0
Idle:	
		
		lea $FF0000,a0
		move.b #'$',(a0)+
		move.l d0,-(sp)
		bsr Int2Str
		move.l (sp)+,d0
		lea $FF0000,a2
		moveq #SecondColumn,d6
		moveq #UpperMargin,d7
		
		addq.l #1,d0
		move.l d0,-(sp)
		bsr PrintMessage
		move.l (sp)+,d0
		bra.s Idle

		
;---------------------
; PrintRegisters
;---------------------
PrintRegisters:
		; Print regs
		lea.l RegistersStr,a2	; Error message address
		moveq #LeftMargin,d6
		moveq #UpperMargin+2,d7
		bsr PrintMessage
		
		; Convert USP value to string
		lea ($FF0000),a0
		move.l #'USP ',(a0)+ 
		move.w #'= ',(a0)+ 
		move.b #'$',(a0)+
		move usp,a1
		move.l a1,d0
		bsr Int2Str ; append number
		
		; Print USP
		lea ($FF0000),a2
		moveq #FirstColumn,d6
		moveq #UpperMargin+4,d7
		bsr PrintMessage	
		
		;
		; NESTED LOOP ahead
		;
		moveq #2-1,d2		; 2 sets of registers
		moveq #4,d3			; displacement register (for sp)
RegPrtOuterLoop:
		moveq #8-1,d1		; 8 registers or 7 for second loop
		subq #1,d2			; horrible hack to handle second loop
		add.l d2,d1 
		addq #1,d2
		moveq #UpperMargin+6,d7
		moveq #48,d4		; ASCII zero
RegPrtInnerLoop:
			; first data regs and then addr regs

			
			lea ($FF0000),a0
			move.b #' ',(a0)+ 
			tst d2
			beq isAddr
			move.b #'D',(a0)+
			moveq #FirstColumn,d6
			bra.s Ugh
isAddr:
			move.b #'A',(a0)+
			moveq #SecondColumn,d6
Ugh:
			move.b d4,(a0)+	
			move.b #' ',(a0)+
			move.w #'= ',(a0)+ 
			move.b #'$',(a0)+ 
			move.l 0(d3,sp),d0  ; 4 because of the call to this subroutine
			movem.l d0-d7,-(sp)
			bsr Int2Str ; append number
			
			; Print reg
			lea ($FF0000),a2
			
			bsr PrintMessage
			movem.l (sp)+,d0-d7
			
			addq #4,d3
			addq #1,d4
			addq #2,d7
			dbf d1,RegPrtInnerLoop
		dbf d2,RegPrtOuterLoop
		
		; Convert old SSP value to string
		lea ($FF0000),a0
		move.l #'SSP ',(a0)+ 
		move.w #'= ',(a0)+ 
		move.b #'$',(a0)+
		move.l SSPBackupAddress,d0
		bsr Int2Str ; append number
		
		; Print SSP
		lea ($FF0000),a2
		moveq #SecondColumn,d6
		bsr PrintMessage
		
		rts
		
;---------------------
; PrintScreen
; a2 = Address of null terminated error message
; d6.w = column number (first is 0)
; d7.w = row number (first is 0)
;---------------------
PrintMessage:
		lea VDPControl,a0
		lea VDPData,a1
		move.l #($C000),d0 ; Address to write name patterns
		mulu.w #64,d7
		add.w d7,d6
		add.w d6,d6
		add.w d6,d0
		
		; Prepare command word
		lsl.l #2,d0
		lsr.w #2,d0
		ori.w #$4000,d0
		swap d0
		move.l d0,(a0)
		moveq #0,d1

Print:
		; Fetch ASCII character
		move.b (a2)+,d1
		tst.b d1
		beq DonePrinting
		sub.b #$20,d1
		move.w d1,(a1)
		bra.s Print
		
DonePrinting:
		move.w #$8F02,(a0)
		
		rts

;---------------------
; Int2Str
; a0 = Address of null terminated string (hex)
; d0.l = Number to convert
;---------------------
Int2Str:
		moveq #0,d1
		moveq #7,d2
		
TakeDigit:
		rol.l #4,d0
		move d0,d1
		andi.w #$000F,d1
		cmpi.w #10,d1
		blt	IsDec
		addq.w #7,d1
IsDec:
		add.w #48,d1
		move.b d1,(a0)+
		dbf d2,TakeDigit
		
Int2StrDone:
		move.b #0,(a0)+
		rts

;---------------------
; PrepareErrorScreen
;---------------------		
PrepareErrorScreen:

		lea VDPControl,a0		
		lea VDPData,a1	
		lea	VDPSetupArray,a2
		
		
		move.w	#(VDPSetupArrayEnd-VDPSetupArray)/2-1,d1	
ReloadVDPRegs:
		move.w	(a2)+,(a0)
		dbf	d1,ReloadVDPRegs
		

		
		bsr ClearScreen
		
		
		; Load ASCII characters
		move.l #1621,d0		; length in words
		move.l #$0,d1		; VRAM addr
		move.l #$1,d2		; VDP mem type (VRAM)
		lea ASCII,a0		; source addr
		move.l a0,d4
		bsr DMA
		
		; Set message screen palette
		move.l #$C0000000,(a0)
		move.l #$0E000EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		move.l #$0EEE0EEE,(a1)
		
		rts
		

;---------------------
; Error strings.
; (Zero terminated)
;---------------------
RegistersStr:
	dc.b 'REGISTERS',$0
	even
BusErrorStr:
	dc.b 'BUS ERROR',$0
	even
AddressErrorStr:
	dc.b 'ADDRESS ERROR',$0
	even
IllegalInstructionStr:
	dc.b 'ILLEGAL INSTRUCTION',$0
	even
DivByZeroStr:
	dc.b 'DIVISION BY ZERO',$0
	even
CHKInstructionStr:
	dc.b 'CHK INSTRUCTION',$0
	even
TrapVStr:
	dc.b 'TRAPV INSTRUCTION',$0
	even
PrivilegeViolationStr:
	dc.b 'PRIVILEGE VIOLATION',$0
	even
TraceStr:
	dc.b 'TRACE', $0
	even
Line1010Str:
	dc.b 'LINE 1010 EMULATOR', $0
	even
Line1111Str:
	dc.b 'LINE 1111 EMULATOR', $0
	even
Trap00Str:
	dc.b 'TRAP 00', $0
	even
ErrorStr:
	dc.b 'UNKNOWN ERROR',$0
	even