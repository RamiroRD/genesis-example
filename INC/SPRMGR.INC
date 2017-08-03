SATENTRY_SIZE equ 8
	
	Section Variables
	
SpriteAttrBuffer ds.b 640
SprMgrCount ds.w 1
	
	Section Code
	
; d0 and d1 should be first and second parts of sprite attribute table entry.
SprMgrAdd:
	lea SpriteAttrBuffer, a0
	lea SprMgrCount, a1

	moveq #0, d2
	move.w (a1), d2
	move.l d2, d3
	lsl.l #3, d3
	add.l d3, a0
	move.l d0, (a0)+
	move.l d1, (a0)+
	addq #1, d2
	move.w d2, (a1)
	rts
	
SprMgrFlush:
	lea VDPControl, a0
	lea SpriteAttrBuffer, a1
	lea SprMgrCount, a2
	lea VDPData, a3
RIterator  equr d0
RLink      equr d1
RFirst     equr d2
RCount     equr d3
	
	; link every sprite
	moveq #1, RLink
	move.w (a2), RIterator
	beq @empty
	move.w RIterator, RCount
	subq #1, RIterator
	move.l #$78000003, (a0)
@loop:
	move.l (a1)+, RFirst
	cmp.w RLink, RCount
	bne @notlast
@last:
	moveq #0, RLink
@notlast
	or.w RLink, RFirst
	move.l RFirst,(a3)
	; second part
	move.l (a1)+,(a3)
	addq #1, RLink
	dbf d0, @loop
	
	moveq #0, d0
	move.w RIterator, (a2)
@empty:
	rts