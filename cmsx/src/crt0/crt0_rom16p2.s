;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; crt0 header for 16KB ROM
; 
; Code address: 0x8010
; Data address: 0xC000
;------------------------------------------------------------------------------
.module	crt0

.globl	_main

HIMEM = #0xFC4A

;------------------------------------------------------------------------------
.area	_HEADER (ABS)
	.org	0x8000

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
	ei
	
	call	_main ; start main() function
	rst		0

;------------------------------------------------------------------------------
; Ordering of segments for the linker

.area   _DATA
.area   _GSINIT
.area   _GSFINAL
