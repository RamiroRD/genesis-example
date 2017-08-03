;-----------------------------------------------------------------------;
;       File Name - SEGASTD.INC                                         ;
;-----------------------------------------------------------------------;
;File contains general macros used by both sides, such as push and pop  ;
;                                                                       ;
;      REVISION HISTORY:                                                ;
;                                                                       ;
;   name     date       comments                                        ;
;   ----    -------     -----------------------------------             ;
;   DRM     04/25/93    file created                                    ;
;   DWM     04/28/93    converted to SNASM debugger                     ;
;-----------------------------------------------------------------------;

  if ~def(SEGASTD_INC)
SEGASTD_INC equ 1

;-----------------------------------------------------------------------
; GENERAL PURPOSE MACROS
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; PUSH - Saves specified register.
;
; input:
;   thing     object to push
;
; returns:
;   nothing
;-----------------------------------------------------------------------
PUSH macro thing
     move.l \thing,-(sp)
     endm


;-----------------------------------------------------------------------
; POP - Restores specified register
;
; input:
;   thing     object to pop
;
; returns:
;   nothing
;-----------------------------------------------------------------------
POP macro thing
    move.l  (sp)+,\thing
    endm


;-----------------------------------------------------------------------
; PUSHM - Saves specified registers.
;
; input:
;   regList   list of registers to push
;
; returns:
;   nothing
;-----------------------------------------------------------------------
PUSHM macro regList
    movem.l \regList,-(sp)
    endm


;-----------------------------------------------------------------------
; POPM - Restores specified registers
;-----------------------------------------------------------------------
POPM macro regList
    movem.l (sp)+,\regList
    endm


;-----------------------------------------------------------------------
; PUSH_CREGS - Saves C registers.
;-----------------------------------------------------------------------
PUSH_CREGS macro
    PUSHM <d0-d7/a0-a6>
    endm


;-----------------------------------------------------------------------
; POP_CREGS - Restores C registers.
;-----------------------------------------------------------------------
POP_CREGS macro
    POPM <d0-d7/a0-a6>
    endm


;-----------------------------------------------------------------------
; COPY_L - Uses A0 & A1 to copy requested number of bytes from *A0 to *A1
;          COPY_L is copy for long items
;
; This macro replaces a more generic macro COPY which was in the original
;  file.  The COPY macro accepted .w .l to determin size of data being
;  moved.  This macro only does longs.
;
;
; input:
;   src       source address
;   dst       destination address
;   count     number of data elements to copy
;
; returns:
;   nothing
;-----------------------------------------------------------------------
COPY_L macro src,dst,count
    local   loop

    PUSHM <d0/a0-a1>
    movea.l \src,a0
    movea.l \dst,a1
    move.w  \count,d0
loop
    move.l (a0)+,(a1)+
    dbra    d0,loop

    POPM <d0/a0-a1>
    endm

  endif

;---------------
;
;All the Bxxxx macros that were contained in the original file were
; placed here.  This code was expanded where needed to eliminate those
; macro calls.
; The macros were:
;
; BSETX  BCLRX  BTSTX
;
;--------------

;-----------------------------------------------------------------------;
;       File Name - SEGASTD.INC                                         ;
;       End of file                                                     ;
;-----------------------------------------------------------------------;

