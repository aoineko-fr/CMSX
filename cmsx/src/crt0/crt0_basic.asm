;_____________________________________________________________________________
;   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
;  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
;  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
;_____________________________________________________________________________
; crt0 header for Basic binary
; 
; Credit: Konamiman 1/2018
; https://github.com/Konamiman/MSX/blob/master/SRC/SDCC/crt0_msxbasic.asm
; 
; Code address: 0x8007 (right after the header)
; Data address: 0      (right after code)
;------------------------------------------------------------------------------
.module crt0

.globl	_main
.globl  l__INITIALIZER
.globl  s__INITIALIZED
.globl  s__INITIALIZER
.globl  s__HEAP

HIMEM = #0xFC4A

;------------------------------------------------------------------------------
; BASIC binary header
.area _HEADER (ABS)
	.org    0x8000

	; Binary program header
	.db 	0xFE		; ID byte
	.dw 	crt0_init	; Start address
	.dw		crt0_end	; End address
	.dw 	crt0_init	; Execution address

;------------------------------------------------------------------------------
.area	_HOME
.area	_CODE
crt0_init:
	di
	; Set stack address at the top of free memory
	ld		sp, (HIMEM)

	; Initialize globals and jump to main()
    ld		bc, #l__INITIALIZER
	ld		a, b
	or		a, c
	jp		z, crt0_start
	ld		de, #s__INITIALIZED
	ld		hl, #s__INITIALIZER
	ldir

crt0_start:
	; start main() function
	ei
	jp		_main

;------------------------------------------------------------------------------
; Ordering other segments for the linker
.area	_RODATA
.area	_INITIALIZER
.area   _GSINIT
.area   _GSFINAL
.area	_DATA
_g_HeapStartAddress::
	.dw		s__HEAP

.area	_INITIALIZED
.area	_BSEG
.area   _BSS
.area   _HEAP

crt0_end:
