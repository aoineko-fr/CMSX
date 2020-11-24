;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; crt0 header for 32KB ROM
; 
; Code address: 0x4010
; Data address: 0xC000
;------------------------------------------------------------------------------
.module	crt0

.globl	_main

HIMEM = #0xFC4A
PPI_A = #0xA8

;------------------------------------------------------------------------------
.area	_HEADER (ABS)
	.org	0x4000

	; ROM header
	.db		0x41
	.db		0x42
	.dw		init
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000

;------------------------------------------------------------------------------
.area	_CODE

init:
	di

	ld		sp, (HIMEM) ; Set stack address at the top of free memory
	
	; Set Page 2 slot equal to Page 1 slot
	in		a, (PPI_A)
	and		a, #0xCF
	ld		c, a
	in		a, (PPI_A)
	and		a, #0x0C
	add		a, a
	add		a, a
	or		a, c
	out		(PPI_A), a
	
	ei
	
	call	_main ; start main() function
	rst		0
	
;------------------------------------------------------------------------------
; Ordering of segments for the linker

.area   _DATA
.area   _GSINIT
.area   _GSFINAL
