;------------------------------------------------------------------------------
;  █▀▀ █▀▄▀█ █▀ ▀▄▀
;  █▄▄ █ ▀ █ ▄█ █ █ v0.2
;------------------------------------------------------------------------------
; crt0 header for Basic binary
; 
; Credit: Konamiman 1/2018
; https://github.com/Konamiman/MSX/blob/master/SRC/SDCC/crt0_msxbasic.asm
; 
; Code address: 0x8020
; Data address: (after code)
;------------------------------------------------------------------------------
.module crt0

.globl	_main
.globl  l__INITIALIZER
.globl  s__INITIALIZED
.globl  s__INITIALIZER
.globl  s__HEAP

HIMEM = #0xFC4A

;------------------------------------------------------------------------------
.area _HEADER (ABS)
	.org    0x8000

	; Binary program header
	.db 	0xFE	; ID byte
	.dw 	init	; Start address
	.dw		end		; End address
	.dw 	init	; Execution address

init:
	; Set stack address at the top of free memory
	di
	ld		sp, (HIMEM)

	; Initialize globals and jump to main()
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
	jp		_main

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

end:
