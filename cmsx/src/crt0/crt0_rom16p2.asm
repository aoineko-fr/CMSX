; ___________________________
; ██▀█▀██▀▀▀█▀▀█▀█  ▄▄▄ ▄▄   │   ▄▄       ▄▄   ▄▄ 
; █  ▄ █▄ ▀██▄ ▀▄█ ██   ██   │  ██ ▀ ██▄▀ ██▀ █ ██
; █  █ █▀▀ ▄█  █ █ ▀█▄█ ██▄▄ │  ▀█▄▀ ██   ▀█▄ ▀▄█▀
; ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀───────────┘
;------------------------------------------------------------------------------
; crt0 header for 8/16KB ROM program in page #2
;------------------------------------------------------------------------------
; By Guillaume 'Aoineko' Blanchard for MSX Game Library 
; (ɔ) 2022 under CC-BY-AS license
;------------------------------------------------------------------------------
; Code address: 0x8010	(right after the header)
; Data address: 0xC000
;------------------------------------------------------------------------------
.module	crt0

.include "defines.asm"
.include "macros.asm"

HIMEM = #0xFC4A

;------------------------------------------------------------------------------
.area	_HEADER (ABS)
	.org	0x8000

	; ROM header
	.db		0x41 ; A
	.db		0x42 ; B
	.dw		crt0_init
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000
	.dw		0x0000

;------------------------------------------------------------------------------
.area	_CODE

crt0_init:
	di
	; Set stack address at the top of free memory
	ld		sp, (HIMEM)
	
	; Initialize heap address
	ld		hl, #s__HEAP
	ld		(#_g_HeapStartAddress), hl

	; Initialize globals
	INIT_GLOBALS

crt0_start:
	; start main() function
	ei
	call	_main
	rst		0

;------------------------------------------------------------------------------
; Ordering of segments for the linker

;-- ROM --
.area	_HOME
.area	_CODE
.area	_RODATA
.area	_INITIALIZER 
.area   _GSINIT
.area   _GSFINAL

;-- RAM --
.area	_DATA
_g_HeapStartAddress::
	.ds 2
.area	_INITIALIZED
.area	_BSEG
.area   _BSS
.area   _HEAP