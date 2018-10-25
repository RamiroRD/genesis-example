ELEM_SIZE     equ  32
MEMPOOL_CAP   equ  128
NA            equ  $FFFF


	Section Variables
; These should only be CHANGED from this file.
MemoryPoolCount ds.w 1 
; These are relative to the start of MemoryPoolElements
MemoryPoolFirst ds.w 1 
MemoryPoolLast  ds.w 1
MemoryPoolFirstFree ds.w 1

MemoryPoolElements ds.b 4096
	
; Element structure offsets
ElemNext      equ  0
ElemNextFree  equ  2
ElemContents  equ  4

	Section Code
; All subroutines in this file follow the convention in which d0-d3 and a0-a3 are the only registers
; that can be modified without restoring their original value.

MemoryPoolInit:
	moveq #0, d0
	move.w #NA, d1
	move.w d0, MemoryPoolCount
	move.w d1, MemoryPoolFirst
	move.w d1, MemoryPoolLast
	lea MemoryPoolElements, a0
	move.w d0, MemoryPoolFirstFree
; Initialize free list
	move.l #128-1, d3
	moveq.l #ELEM_SIZE, d0 ; Next free field of first element should be ELEM_SIZE, then it increases by ELEM_SIZE
	addi.l #ElemNextFree, a0 ; Accessor to NextFree element, to be increased by ELEM_SIZE in every iteration
@init:
	move.w d0, (a0)
	add.l #ELEM_SIZE, d0
	addi.l #ELEM_SIZE, a0
	dbf d3, @init
	; NextFree of last element should point to nowhere (NA)
	move.w d1, (a0)
	rts

;
; a0 ends up being the address of the allocated ELEMENT
;
MemoryPoolAlloc:
	move.w #MEMPOOL_CAP, d3
	move.w MemoryPoolCount, d0
	cmp.w d0, d3
	beq .full
	
	move.w #NA, d1
	lea MemoryPoolFirstFree, a1
	move.w (a1), d2 ; semi-ptr to new element
	move.w MemoryPoolLast, d3 ; We need the old last
	move.w d2, MemoryPoolLast
	lea MemoryPoolElements, a0
	move.l a0, a2 ; for .notfirst
	add.w d2, a0 ; a0 is now ptr to new element
	move.w d1, ElemNext(a0)
	move.w ElemNextFree(a0), (a1)
	
	addq #1, d0
	move.w d0, MemoryPoolCount
	cmpi.w #1, d0
	bne .notfirst
.first:
	move.w d2, MemoryPoolFirst
	rts
.notfirst:
	add.l d3, a2
	move.w d2, ElemNext(a2)
	rts
.full:
	move.w #NA,d0
	ext d0
	move.l d0,a0
	rts ; return a0
	
MemoryPoolFree:
	rts
