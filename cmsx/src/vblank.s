;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; Driver to re-root VDP interruption
;
; Author: Sector28, Aoineko
;------------------------------------------------------------------------------
.module	driver

.globl	_vblank

VDP_S = #0x99

;------------------------------------------------------------------------------
.area   _DRIVER

	.org	0x0038

	di
	push	af

	in		a, (VDP_S)
	and		a
	jp		p, _interrupt_end

	push	hl
	push	de
	push	bc
	exx
	ex		af, af'
	push	af
	push	hl
	push	de
	push	bc
	push	iy
	push	ix

	call    _vblank

	pop		ix
	pop		iy
	pop		bc
	pop		de
	pop		hl
	pop		af
	ex		af, af'
	exx
	pop		bc
	pop		de
	pop		hl
	
_interrupt_end:		
	pop		af

	ei
	ret