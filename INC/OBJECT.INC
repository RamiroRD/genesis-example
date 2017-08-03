				rsreset
ObjHandler		rs.l 1
ObjState		rs.w 1
ObjPosX 		rs.w 1
ObjPosY 		rs.w 1
ObjVelX 		rs.w 1
ObjVelY 		rs.w 1
ObjAccX 		rs.w 1
ObjAccY 		rs.w 1
ObjPhi 			rs.w 1
ObjTimer1		rs.w 1
ObjTimer2		rs.w 1
ObjUser1		rs.w 1
ObjUser2		rs.w 1
ObjUser3		rs.w 1

	Section Variables

	Section Code

; Assume this uses ALL registers
UpdateObjects:
	lea MemoryPoolFirst, a4
	lea MemoryPoolElements, a5
@loop:
	move.w (a4), d4
	cmp.w #NA, d4
	beq @finish ; empty list
	move.l a5, a4
	add.w d4, a4 
	move.l ElemContents+ObjHandler(a4), a6
	movea.l a4, a0
	addq #ElemContents, a0
	jsr (a6)
	bra @loop
@finish:
	rts
	