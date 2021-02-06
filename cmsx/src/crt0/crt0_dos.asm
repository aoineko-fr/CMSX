;_____________________________________________________________________________
;   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
;  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
;  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
;_____________________________________________________________________________
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
	di
	; Set stack address at the top of free memory
	ld		sp, (HIMEM)
	
	; Initialize globals
    ld		bc, #l__INITIALIZER
	ld		a, b
	or		a, c
	jp		z, start
	ld		de, #s__INITIALIZED
	ld		hl, #s__INITIALIZER
	ldir

	; Initialize heap address
	ld		hl, #s__HEAP
	ld		(#_g_HeapStartAddress), hl

start:
	; start main() function
	ei
	call	_main
	; exit program and return value from L register
	ld		c, #0x62
	ld		b, l
	call	5				; Try the DOS-2 termination function (_TERM)
	ld		c, #0x00
	jp		5				; Otherwise, try the DOS-1 termination function (0)

;------------------------------------------------------------------------------
; Ordering of segments for the linker

.area	_HOME
.area	_CODE
.area	_RODATA
.area	_INITIALIZER 
.area   _GSINIT
.area   _GSFINAL
.area	_DATA
_g_HeapStartAddress::
	.ds 2

.area	_INITIALIZED
.area	_BSEG
.area   _BSS
.area   _HEAP