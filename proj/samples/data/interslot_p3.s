;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; 
;------------------------------------------------------------------------------

.globl _InterSlotWritePage3
.globl _InterSlotReadPage3

;------------------------------------------------------------------------------
.area   _CODE

 _InterSlotWritePage3:: ; void (u8 slot, u16 addr, u8 value)

	di
	push	ix
	ld		ix, #0
	add		ix, sp
	ld		d, 4(ix)
	ld		l, 5(ix)
	ld		h, 6(ix)
	ld		e, 7(ix)
	ld		iyh, e
	; Backup
	ld		a, (0xFFFF)
	cpl
	ld		c, a			; Save current secondary slot in C
	in		a, (0xA8)
	ld		b, a			; Save current primary slot in B

	; Switch slot
	ld		a, d
	rrca
	rrca
	and		#0x3F
	ld		e, a
	ld		a, b
	and		#0x3F
	or		e				; remplace ?? par le slot primaire (00,01,10 ou 11)
	out		(0xA8), a

	ld		a, d
	rlca
	rlca
	rlca
	rlca
	and		#0x3F
	ld		e, a
	ld		a, c
	and		#0x3F
	or		e				; remplace ?? par le slot secondaire (00,01,10 ou 11)
	ld		(0xFFFF), a
	; Write
	ld		(hl), a

	; Restore		
	ex		af, af'
	ld		a, b
	out		(0xA8), a
	ld		a, c
	ld		(0xFFFF), a
	ex		af,	af'

	pop		ix
	ei
	ret

_InterSlotReadPage3:: ; u8 InterSlotReadPage3(u8 slot, u16 addr)

	di
	push	ix
	ld		ix, #0
	add		ix, sp
	ld		d, 4(ix)
	ld		l, 5(ix)
	ld		h, 6(ix)
	; Backup
	ld		a, (0xFFFF)
	cpl
	ld		c, a			; Save current secondary slot in C
	in		a, (0xA8)
	ld		b, a			; Save current primary slot in B

	; Switch slot
	ld		a, d
	rrca
	rrca
	and		#0x3F
	ld		e, a
	ld		a, b
	and		#0x3F
	or		e				; remplace ?? par le slot primaire (00,01,10 ou 11)
	out		(0xA8), a

	ld		a, d
	rlca
	rlca
	rlca
	rlca
	and		#0x3F
	ld		e, a
	ld		a, c
	and		#0x3F
	or		e				; remplace ?? par le slot secondaire (00,01,10 ou 11)
	ld		(0xFFFF), a
	; Read
	ld		a, (hl)

	; Restore		
	ex		af, af'
	ld		a, b
	out		(0xA8), a
	ld		a, c
	ld		(0xFFFF), a
	ex		af,	af'

	pop		ix
	ei
	ret
