;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; crt0 header for MSX-DOS program
; 
; Credit: Konamiman & Avelino, 11/2004
; https://github.com/Konamiman/MSX/blob/master/SRC/SDCC/crt0_msxbasic.asm
; 
; Code address: 0x0108
; Data address: (after code)
;------------------------------------------------------------------------------
.globl	_main
.globl  l__INITIALIZER
.globl  s__INITIALIZED
.globl  s__INITIALIZER

;------------------------------------------------------------------------------
.area	_HEADER (ABS)

	.org	0x0100 ; MSX-DOS .com program start address

init:

	; Initialize globals and jump to "main"
	call	gsinit
	jp		__pre_main

	;--- Program code and data (global vars) start here
	;* Place data after program code, and data init code after data

;------------------------------------------------------------------------------
.area	_CODE

__pre_main:
	push	de
	ld		de, #_HEAP_start
	ld		(_heap_top),de
	pop		de
	jp		_main

;------------------------------------------------------------------------------
.area	_DATA

_heap_top::
	.dw 0

;------------------------------------------------------------------------------
.area	_GSINIT

gsinit::
	ld		bc, #l__INITIALIZER
	ld		a, b
	or		a, c
	jp		z, gsinext
	ld		de, #s__INITIALIZED
	ld		hl, #s__INITIALIZER
	ldir
	
gsinext:

;------------------------------------------------------------------------------
.area	_GSFINAL

	ret

;* These doesn't seem to be necessary... (?)

;.area  _OVERLAY
;.area	_HOME
;.area  _BSS

;------------------------------------------------------------------------------
.area	_HEAP

_HEAP_start::