;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; crt0 header for MSX-DOS program
; 
; Code address: 0x0100
; Data address: (after code)
;------------------------------------------------------------------------------
.module	crt0

.globl	_main
.globl  l__INITIALIZER
.globl  s__INITIALIZED
.globl  s__INITIALIZER
.globl  s__HEAP

HIMEM = #0xFC4A

;------------------------------------------------------------------------------
.area	_HEADER (ABS)
	.org	0x0100 ; MSX-DOS .com program start address

;------------------------------------------------------------------------------
.area	_CODE

init:
	; Set stack address at the top of free memory
	di
	ld		sp, (HIMEM)
	
	; Initialize globals
    ld		bc, #l__INITIALIZER
	ld		a, b
	or		a, c
	jp		z, start
	ld		de, #s__INITIALIZED
	ld		hl, #s__INITIALIZER
	ldir

start:
	; start main() function
	ei
	call	_main
	rst		0

;------------------------------------------------------------------------------
.area	_DATA

_g_HeapStartAddress::
	.dw		s__HEAP

;------------------------------------------------------------------------------
; Ordering of segments for the linker

.area	_HOME
.area	_CODE
.area	_INITIALIZER 
.area   _GSINIT
.area   _GSFINAL
.area	_DATA
.area	_INITIALIZED
.area	_BSEG
.area   _BSS
.area   _HEAP