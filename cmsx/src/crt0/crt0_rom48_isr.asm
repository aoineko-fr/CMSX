;_____________________________________________________________________________
;   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
;  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
;  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
;_____________________________________________________________________________
; crt0 header for 48KB ROM with ISR replacement
; 
; Code address: 0x4000
; Data address: 0xC000
;------------------------------------------------------------------------------
.module	crt0

.globl	_main
.globl	_VDP_InterruptHandler
.globl  l__INITIALIZER
.globl  s__INITIALIZED
.globl  s__INITIALIZER
.globl  s__HEAP

HIMEM  = #0xFC4A
ROMVER = #0x002B
MSXVER = #0x002D
PPI_A  = #0xA8
VDP_S  = #0x99

;------------------------------------------------------------------------------
.area	_HEADER (ABS)

	.org	0x0000

;------------------------------------------------------------------------------
.area   _DRIVER (ABS)

	.org	0x0038

_interrupt_start::

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

	call    _VDP_InterruptHandler

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
	reti


;------------------------------------------------------------------------------
.area	_CODE

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

init:
	di
	; Set stack address at the top of free memory
	ld		sp, (HIMEM)

	; Backup Page 0 (Main-ROM) information
	ld		a, (ROMVER)
	ld		(#_g_VersionROM), a
	ld		a, (MSXVER)
	ld		(#_g_VersionMSX), a

	; Set Page 0 & 2 slot equal to Page 1 slot
	in		a, (PPI_A)				; Get primary slots info [P3|P2|P1|P0]
	ld		b, a					; Backup full slots info
	and		a, #0xCC				; Mask P0 & P2 slots [P3|00|P1|00]
	ld		c, a					; Backup P1 & P3 slots

	ld		a, b					; Retreive primary slots info again
	and		a, #0x0C				; Mask all but P1 slots
	add		a, a					; 
	add		a, a					; P1<<2
	or		a, c					; [P3|P1<<2|P1|00]
	ld		c, a					; Backup [P3|P1<<2|P1|00]

	ld		a, b					; Retreive primary slots info again
	and		a, #0x0C				; Mask all but P2 slots
	rrca
	rrca
	and		a, #0x3F				; P1>>2
	or		a, c					; [P3|P2|P1|P1>>2]
	
	out		(PPI_A), a
	
	; Initialize heap address
	ld		hl, #s__HEAP
	ld		(#_g_HeapStartAddress), hl

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
_g_VersionROM::
	.ds 1
_g_VersionMSX::
	.ds 1

.area	_INITIALIZED
.area	_BSEG
.area   _BSS
.area   _HEAP
