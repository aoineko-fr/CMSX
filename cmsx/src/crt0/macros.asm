;______________________________________________________________________________
;   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
;  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
;  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
;______________________________________________________________________________
.module	crt0

;==============================================================================
; HELPER
;==============================================================================

;------------------------------------------------------------------------------
; Initialize globals
.macro INIT_GLOBALS
    ld		bc, #l__INITIALIZER
	ld		a, b
	or		a, c
	jp		z, skip_globals	
	ld		de, #s__INITIALIZED
	ld		hl, #s__INITIALIZER
	ldir
skip_globals:
.endm

;------------------------------------------------------------------------------
; Set Page 2 at the same slot than the Page 1 one
.macro SET_PAGE2_SLOT_FROM_PAGE1
	; Set Pages 1, 2 & 3 slot equal to Page 1 subslot
	in		a, (PPI_A)				; A=[P3|P2|P1|P0] Get primary slots info 
	ld		b, a					; B=[P3|P2|P1|P0] Backup original primary slots info
	and		a, #0b00001100			; A=[00|00|P1|00] Mask all pages slots but P1 

	ld		c, a					; C=[00|00|P1|00] Backup P1
	add		a, a					;                 P1<<1
	add		a, a					; A=[00|P1|00|00] P1<<1
	ld		d, a					; D=[00|P1|00|00] Backup P1<<2
	or		a, c					; A=[00|P1|P1|00] Merge P1 in P2
	ld		c, a					; C=[00|P1|P1|00]

	ld		a, d					; D=[00|P1|00|00] Backup P1<<2
	add		a, a					;                 P1<<1
	add		a, a					; A=[P1|00|00|00] P1<<1
	or		a, c					; A=[P1|P1|P1|00] Merge P1 in P2 & P3
	ld		c, a					; C=[P1|P1|P1|00]
	
	ld		a, b					; A=[P3|P2|P1|P0] Restore primary slots info 
	and		a, #0b00000011			; A=[00|00|00|P0] Mask all pages slots but P0 
	or		a, c					; A=[P1|P1|P1|P0] Merge original P0 and P1 in all other page

	out		(PPI_A), a				; Set primary slots info
	ld		e, a					; E=[P1|P1|P1|P0] Backup new slots

	; Set Page 2 subslot equal to Page 1 subslot
	ld		a, (SLTSL)				; A=[S3|S2|S1|S0] Read secondary slots register of selected primary slot
	cpl								;                 Reverses the bits
	and		a, #0b00001100			; A=[00|00|S1|00] Mask all pages slots but P1 

	ld		c, a					; C=[00|00|S1|00] Backup P1
	add		a, a					;                 P1<<1
	add		a, a					; A=[00|S1|00|00] P1<<1
	ld		d, a					; D=[00|S1|00|00] Backup P1<<2
	or		a, c					; A=[00|S1|S1|00] Merge P1 in P2
	ld		c, a					; C=[00|S1|S1|00]

	ld		a, d					; D=[00|S1|00|00] Backup P1<<2
	add		a, a					;                 P1<<1
	add		a, a					; A=[S1|00|00|00] P1<<1
	or		a, c					; A=[S1|S1|S1|00] Merge P1 in P2 & P3
	ld		c, a					; C=[S1|S1|S1|00]
	
	ld		a, b					; A=[S3|S2|S1|S0] Restore primary slots info 
	and		a, #0b00000011			; A=[00|00|00|P0] Mask all pages slots but P0 
	or		a, c					; A=[S1|S1|S1|S0] Merge original P0 and P1 in all other page
	
	ld		(SLTSL), a				; 

	; Restore initial Page 3 slot
	ld		a, b					; A=[P3|P2|P1|P0] Restore initiale slots
	and		a, #0b11000000			; A=[P3|00|00|00]
	ld		b, a					; B=[P3|00|00|00]

	ld		a, e					; A=[P1|P1|P1|P0]
	and		a, #0b00111111			; A=[00|P1|P1|P0]
	or		a, b					; A=[P3|P1|P1|P0]

	out		(PPI_A), a
.endm



;==============================================================================
; ROM MAPPER
;==============================================================================

;------------------------------------------------------------------------------
.ifeq ROM_MAPPER-ROM_PLAIN
	.macro INIT_MAPPER
	.endm
.endif
;------------------------------------------------------------------------------
.ifeq ROM_MAPPER-ROM_ASCII8
	BANK0_ADDR = #0x6000
	BANK1_ADDR = #0x6800
	BANK2_ADDR = #0x7000
	BANK3_ADDR = #0x7800

	.macro INIT_MAPPER
		xor		a
		ld		(BANK0_ADDR), a ; Segment 0 in Bank 0
		inc		a
		ld		(BANK1_ADDR), a ; Segment 1 in Bank 1
		inc		a
		ld		(BANK2_ADDR), a ; Segment 2 in Bank 2
		inc		a
		ld		(BANK3_ADDR), a ; Segment 3 in Bank 3
	.endm
.endif
;------------------------------------------------------------------------------
.ifeq ROM_MAPPER-ROM_ASCII16
	BANK0_ADDR = #0x6000
	BANK1_ADDR = #0x77FF

	.macro INIT_MAPPER
		xor		a
		ld		(BANK0_ADDR), a ; Segment 0 in Bank 0
		inc		a
		ld		(BANK1_ADDR), a ; Segment 1 in Bank 1
	.endm
.endif
;------------------------------------------------------------------------------
.ifeq ROM_MAPPER-ROM_KONAMI
	BANK1_ADDR = #0x6000
	BANK2_ADDR = #0x8000
	BANK3_ADDR = #0xA000

	.macro INIT_MAPPER
		xor		a
		inc		a
		ld		(BANK1_ADDR), a ; Segment 1 in Bank 1
		inc		a
		ld		(BANK2_ADDR), a ; Segment 2 in Bank 2
		inc		a
		ld		(BANK3_ADDR), a ; Segment 3 in Bank 3
	.endm
.endif
;------------------------------------------------------------------------------
.ifeq ROM_MAPPER-ROM_KONAMI_SCC
	BANK0_ADDR = #0x5000
	BANK1_ADDR = #0x7000
	BANK2_ADDR = #0x9000
	BANK3_ADDR = #0xB000

	.macro INIT_MAPPER
		xor		a
		ld		(BANK0_ADDR), a ; Segment 0 in Bank 0
		inc		a
		ld		(BANK1_ADDR), a ; Segment 1 in Bank 1
		inc		a
		ld		(BANK2_ADDR), a ; Segment 2 in Bank 2
		inc		a
		ld		(BANK3_ADDR), a ; Segment 3 in Bank 3
	.endm
.endif
