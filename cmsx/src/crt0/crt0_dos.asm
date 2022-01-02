; ___________________________
; ██▀█▀██▀▀▀█▀▀█▀█  ▄▄▄ ▄▄   │   ▄▄       ▄▄   ▄▄ 
; █  ▄ █▄ ▀██▄ ▀▄█ ██   ██   │  ██ ▀ ██▄▀ ██▀ █ ██
; █  █ █▀▀ ▄█  █ █ ▀█▄█ ██▄▄ │  ▀█▄▀ ██   ▀█▄ ▀▄█▀
; ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀───────────┘
;------------------------------------------------------------------------------
; crt0 header for MSX-DOS program
;------------------------------------------------------------------------------
; By Guillaume 'Aoineko' Blanchard for MSX Game Library 
; (ɔ) 2022 under CC-BY-AS license
; 
; Based on work from 'Konamiman' and 'Avelino' (11/2004)
;  https://github.com/Konamiman/MSX/blob/master/SRC/SDCC/crt0-msxdos/crt0msx_msxdos.asm
;------------------------------------------------------------------------------
; Code address: 0x0100
; Data address: 0		(right after code)
;------------------------------------------------------------------------------
.module	crt0

.include "defines.asm"
.include "macros.asm"

HIMEM = #0xFC4A

;------------------------------------------------------------------------------
.area	_HEADER (ABS)
	.org	0x0100 ; MSX-DOS .com program start address

_g_FirstAddr::
_g_HeaderAddr::

;------------------------------------------------------------------------------
.area	_CODE

crt0_init:
	di
	; Set stack address at the top of free memory
	ld		sp, (HIMEM)
	
	; Initialize globals
	INIT_GLOBALS

crt0_start:
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
_g_LastAddr::
.area	_DATA
_g_HeapStartAddress::
	.dw		s__HEAP

.area	_INITIALIZED
.area	_BSEG
.area   _BSS
.area   _HEAP