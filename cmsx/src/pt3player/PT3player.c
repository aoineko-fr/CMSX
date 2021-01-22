/* =============================================================================
   SDCC Vortex Tracker II PT3 player for MSX

   Version: 1.1.4 (08/01/2021)
   Architecture: MSX
   Format: C Object (SDCC .rel)
   Programming language: C and Z80 assembler

   Authors:
    - Vortex Tracker II v1.0 PT3 player for ZX Spectrum by S.V.Bulba 
      <vorobey@mail.khstu.ru> http://bulba.at.kz
    - (09-Jan-05) Adapted to MSX by Alfonso D. C. aka Dioniso 
      <dioniso072@yahoo.es>
    - Arrangements for MSX ROM: MSXKun/Paxanga soft > 
      http://paxangasoft.retroinvaders.com/
    - asMSX version: SapphiRe > http://www.z80st.es/
    - Adapted to SDCC: mvac7/303bcn > <mvac7303b@gmail.com>

   Description:
     Adaptation of the Vortex Tracker II PT3 Player for MSX to be used in 
     software development in C (SDCC). 
     
   History of versions:
    - 1.1.4 (08/01/2021)>PT3_Init and Bug #11 in loop
    - 1.1.3 (05/01/2021) PT3state, PT3_Loop, PT3_Pause and PT3_Resume
    - 1.1.2 (04/01/2021) assignment of frequency table memory address to NoteTable 
    - 1.1 (28/05/2019) Adaptation to SDCC of asMSX version by SapphiRe.
    - 1.0 (21/10/2016) Adaptation to SDCC of the ROM version by Kun.

In this replayer:

Dioniso version:
 - No version detection (just for Vortex Tracker II and PT3.5).
 - No frequency table decompression (default is number 2). 
   Coger tabla segun quiera, en fichero aparte
 - No volume table decompression (Vortex Tracker II/PT3.5 volume table used).


msxKun version:
 - Usable desde ROM (solo tiene en RAM area de trabajo, lo minimo posible).

SapphiRe version:
 This version of the replayer uses a fixed volume and note table, if you need a 
 different note table you can copy it from TABLES.TXT file, distributed with the
 original PT3 distribution. This version also allows the use of PT3 commands.

 PLAY and PSG WRITE routines seperated to allow independent calls


mvac7 version:
 Adaptation to C (SDCC) of the SapphiRe version.

============================================================================= */


#include "PT3player.h"

//VARS:
u8  ChanA[29]; // CHNPRM_Size
u8  ChanB[29];
u8  ChanC[29];			

u8  DelyCnt;
u16 CurESld;		
u8  CurEDel;

//Ns_Base_AddToNs:
u8  Ns_Base;		
u8  AddToNs;		

//AYREGS::
u8  PT3_Regs[14];
u16 EnvBase;
u8  VAR0END[240];

u8  PT3_State;			// Before called PT3_SETUP
//	7	6	5	4	3	2	1	0	
//	│			│			└────── Play Enable
//	│			└────────────────── Loop Enable
//	└────────────────────────────── Loop Reached
         
u16 PT3_ModAddr;		// Song data address
u16 PT3_CrPsPtr;		// Cursor position in pattern
u16 PT3_SamPtrs;		// Sample info?
u16 PT3_OrnPtrs;		// Ornament pattern

u16 PT3_PDSP;			// pilasave
u16 PT3_CSP;			// pilsave2
u16 PT3_PSP;			// pilsave3
  
u8  PT3_PrNote;			//
u16 PT3_PrSlide;		//
  
u16 PT3_AdInPtA;		// Play data pattern
u16 PT3_AdInPtB;		// Play data
u16 PT3_AdInPtC;		// Play data
  
u16 PT3_LPosPtr;		// Position Ptr?
u16 PT3_PatsPtr;		// Pat Ptr

u8  PT3_Delay;			// Delay
u8  PT3_AddToEn;		// Envelope data (No scale as it does not use Envs??)
u8  PT3_Env_Del;		// Envelope data (idem)
u16 PT3_ESldAdd;		// Envelope data (idem)

const void* PT3_NoteTable;    // note table memory address

#if (PT3_EXTRA)

u16 PT3_SrtCrPsPtr;  	// Cursor position in pattern at start

#endif // (PT3_EXTRA)


	

// ================================== REPLAYER =================================



/* =============================================================================
 PT3_Init
 Description: Initialize the Player
 Input:       -
 Output:      -
============================================================================= */
void PT3_Init()
{
// Create Volume Table for Vortex Tracker II/PT3.5
// (c) Ivan Roshin, adapted by SapphiRe ---
__asm
	ld		HL, #0x11
	ld		D, H
	ld		E, H
	ld		IX, #_VAR0END  ;_VT_ + 16
	ld		B, #15
INITV1:	
	push	HL
	add		HL, DE
	ex		DE, HL
	sbc		HL, HL
	ld		C, B 
	ld		B, #16
INITV2:	
	ld		A, L
	rla
	ld		A, H
	adc		A, #0
	ld		(IX), A
	inc		IX
	add		HL, DE
	djnz	INITV2
	pop		HL
	ld		A, E
	cp		#0x77
	jr		NZ, INITV3
	inc		E
INITV3:	
	ld		B, C
	djnz	INITV1


CLEAR_REGS: 
	xor		A  
	ld		(#_PT3_State), A
	ld		HL, #_PT3_Regs
	ld		DE, #_PT3_Regs + 1
	ld		BC, #13
	ld		(HL), A
	ldir  

__endasm;
}





/* =============================================================================
 PT3_Mute
 Description: Silence the PSG
 Input:       -
 Output:      -
============================================================================= */
void PT3_Mute() __naked
{
__asm
MUTE:	
	xor		A
	ld		(#_PT3_Regs + PSG_REG_AMP_A), A
	ld		(#_PT3_Regs + PSG_REG_AMP_B), A
	ld		(#_PT3_Regs + PSG_REG_AMP_C), A
	
	jp		_PT3_UpdatePSG                ;ROUT_A0
  
__endasm;
}
// ----------------------------------------------------------------------------- END PT3_Mute



/* =============================================================================
 PT3_Pause
 Description: Pause song playback
 Input:       -
 Output:      -
============================================================================= */
void PT3_Pause() __naked
{
__asm
	ld		HL, #_PT3_State       
	res		1, (HL)

	jp		MUTE
__endasm;
}
// ----------------------------------------------------------------------------- <<< END PT3_Pause



/* =============================================================================
 PT3_Resume
 Description: Resume song playback
 Input:       -
 Output:      -
============================================================================= */  	
void PT3_Resume()
{
__asm
	ld		HL, #_PT3_State       
	set		1, (HL)      ;PLAYER ON
__endasm;
}
// ----------------------------------------------------------------------------- <<< END PT3_Resume




/* =============================================================================
 PT3_Loop
 Description: Change state of loop
 Input:       - 0=off ; 1=on  (false = 0, true = 1)
 Output:      -
============================================================================= */
void PT3_SetLoop(u8 loop) __z88dk_fastcall
{
loop;
__asm

	ld		A, L			; Fastcall parameter
	ld		HL, #_PT3_State
	or		A
	jr		NZ, LoopON
	res		4, (HL)			; not loop
	ret
  
LoopON:  
	set		4, (HL)			; loop

__endasm;
}

/* -----------------------------------------------------------------------------
PT3_InitSong
(u16) Song data address
(char) Loop - 0=off ; 1=on  (false = 0, true = 1));
----------------------------------------------------------------------------- */
void PT3_InitSong(const void* songAddr) __z88dk_fastcall
{
	songAddr;
__asm
	//push	IX
									// HL = songAddr
#if (PT3_SKIP_HEADER)
	ld		DE, #-100
	add		HL, DE
#endif

	ld		(#_PT3_ModAddr), HL		// _PT3_ModAddr = songAddr
	ex		DE, HL					// DE = songAddr

	ld		HL, #100
	add		HL, DE
	ld		A, (HL)
	ld		(#_PT3_Delay), A		// _PT3_Delay = [songAddr + 100]

	ld		HL, #105
	add		HL, DE
	ld		(#_PT3_SamPtrs), HL		// _PT3_SamPtrs = songAddr + 105

	ld		HL, #169
	add		HL, DE
	ld		(#_PT3_OrnPtrs), HL		// _PT3_OrnPtrs = songAddr + 169

	ld		HL, #200
	add		HL, DE
	ld		(#_PT3_CrPsPtr), HL		// _PT3_CrPsPtr = songAddr + 200
#if (PT3_EXTRA)
	ld		(#_PT3_SrtCrPsPtr), HL	// Backup startup value
#endif

	ld		IX, (#_PT3_ModAddr)		// IX = songAddr

	ld		B, #0
	ld		C, 102(IX)				// BC = [songAddr + 102]
	ld		HL, #201
	add		HL, DE
	add		HL, BC
	ld		(#_PT3_LPosPtr), HL		// _PT3_LPosPtr = songAddr + 201 + BC

	ld		B, 104(IX)
	ld		C, 103(IX)				// BC = [songAddr + 104 | songAddr + 103]
	ex		DE, HL					// HL = songAddr
	add		HL, BC
	ld		(#_PT3_PatsPtr), HL		// _PT3_PatsPtr  = songAddr + BC

; --- INITIALIZE PT3 VARIABLES -------------------------------------------------  
	xor		A	
	ld		HL, #_ChanA				; VARS
	ld		(HL), A
	ld		DE, #_ChanA + 1			; VARS+1
	ld		BC, #_VAR0END - _ChanA -1    ;_PT3_Regs - _ChanA -1
	ldir

	inc		A
	ld		(#_DelyCnt), A
	ld		HL, #0xF001				; H - CHNPRM_Volume, L - CHNPRM_NtSkCn
	ld		(#_ChanA + CHNPRM_NtSkCn),HL
	ld		(#_ChanB + CHNPRM_NtSkCn),HL
	ld		(#_ChanC + CHNPRM_NtSkCn),HL

	ld		HL, #EMPTYSAMORN
	ld		(#_PT3_AdInPtA), HL		; ptr to zero  ; # chg
	ld		(#_ChanA + CHNPRM_OrnPtr), HL ;ornament 0 is "0,1,0"
	ld		(#_ChanB + CHNPRM_OrnPtr), HL ;in all versions from
	ld		(#_ChanC + CHNPRM_OrnPtr), HL ;3.xx to 3.6x and VTII

	ld		(#_ChanA + CHNPRM_SamPtr), HL ;S1 There is no default
	ld		(#_ChanB + CHNPRM_SamPtr), HL ;S2 sample in PT3, so, you
	ld		(#_ChanC + CHNPRM_SamPtr), HL ;S3 can comment S1,2,3; see
	; also EMPTYSAMORN comment

// Autoplay
#if (PT3_AUTOPLAY)
	xor		A
	ld		HL, #_PT3_State
	ld		(HL), A
	SET		1, (HL)      ;PLAYER ON  
#endif

	//pop		IX

__endasm;
}
// ----------------------------------------------------------------------------- END playerINIT

  




/* -----------------------------------------------------------------------------
PT3PlayAY
Play Song. 
Send data to AY registers
Execute on each interruption of VBLANK
----------------------------------------------------------------------------- */
void PT3_UpdatePSG()
{
__asm  

	// Update mixer register wanted value with I/O 2-bits from the current mixer register value
	ld		A, (#_PT3_Regs + PSG_REG_MIXER)
	and		#0b00111111
	ld		B, A
	ld		A, #PSG_REG_MIXER
	out		(#PSG_PORT_REG), A
	in		A, (#PSG_PORT_READ)  
	and		#0b11000000
	or		B
	ld		(#_PT3_Regs + PSG_REG_MIXER), A

	// Registers value copy loop (528 T-States)
	ld		HL, #_PT3_Regs	// 11	Data to copy to PSG registers
	ld		C, #PSG_PORT_WRITE		// 8	Setup outi register
	xor		A						// 5	Initialize register number
	// R#0-12
	.rept 13
		out		(PSG_PORT_REG), A	// 12	port_reg <- reg_num
		outi						// 18	port_data <- data[i++]
		inc		A					// 5
	.endm
	// R#13
	out		(PSG_PORT_REG), A		// 12	port_reg <- reg_num
	ld		A, (HL)					// 8
	and		A						// 5
	ret		M						// 12	don't copy R#13 if value is negative
	out		(PSG_PORT_WRITE), A		// 12	port_data <- data[i]

__endasm;
}
// ----------------------------------------------------------------------------- END PT3PlayAY





/* -----------------------------------------------------------------------------
PT3Decode
Decode a frame from PT3 song
----------------------------------------------------------------------------- */
void PT3_Decode() __naked
{
__asm   
 
	ld		HL, #_PT3_State							// Check bit #1 of PT3_State to know if music is playing
	bit		1, (HL)
	ret		Z

	xor		A
	ld		(#_PT3_AddToEn), A						// PT3_AddToEn = 0
	ld		(#_PT3_Regs + PSG_REG_MIXER), A			// PSG_REG_MIXER = 0
	dec		A
	ld		(#_PT3_Regs + PSG_REG_SHAPE), A			// PSG_REG_SHAPE = 0xFF

	LD   HL,#_DelyCnt
	DEC  (HL)
	JP   NZ,PL2

	LD   HL,#_ChanA + CHNPRM_NtSkCn
	DEC  (HL)
	JR   NZ,PL1B

	ld	 BC,(#_PT3_AdInPtA)
	LD   A,(BC)
	AND  A
	JR   NZ,PL1A

	LD   D,A
	LD   (#_Ns_Base),A
	LD   HL,(#_PT3_CrPsPtr)
	INC  HL
	LD   A,(HL)
	INC  A
	JR   NZ,PLNLP

	CALL CHECKLP

	ld   HL,(#_PT3_LPosPtr)
	LD   A,(HL)
	INC  A
  
PLNLP:	
	LD   (#_PT3_CrPsPtr),HL
	DEC  A
	ADD  A,A
	LD   E,A
	RL   D
	ld   HL,(#_PT3_PatsPtr)
	ADD  HL,DE
	LD   DE,(#_PT3_ModAddr)
	ld   (#_PT3_PSP),SP
	LD   SP,HL
	POP  HL
	ADD  HL,DE
	LD   B,H
	LD   C,L
	POP  HL
	ADD  HL,DE
	LD   (#_PT3_AdInPtB),HL
	POP  HL
	ADD  HL,DE
	LD   (#_PT3_AdInPtC),HL
	ld   SP,(#_PT3_PSP)
  
PL1A:	
	LD   IX,#_ChanA + 12
	CALL PTDECOD
	LD   (#_PT3_AdInPtA),BC
  
PL1B:	
	LD   HL,#_ChanB + CHNPRM_NtSkCn
	DEC  (HL)
	JR   NZ,PL1C
	LD   IX,#_ChanB + 12
	ld   BC,(#_PT3_AdInPtB)
	CALL PTDECOD
	LD   (#_PT3_AdInPtB),BC

PL1C:	
	LD   HL,#_ChanC + CHNPRM_NtSkCn
	DEC  (HL)
	JR   NZ,PL1D
	LD   IX,#_ChanC + 12
	ld   BC,(#_PT3_AdInPtC)
	CALL PTDECOD
	LD   (#_PT3_AdInPtC),BC

PL1D:	
	ld   A,(#_PT3_Delay)
	ld   (#_DelyCnt),A

PL2:	
	LD   IX,#_ChanA
	LD   HL,(#_PT3_Regs + PSG_REG_TONE_A)
	CALL CHREGS
	LD   (#_PT3_Regs + PSG_REG_TONE_A),HL			// Set Tone A
	LD   A,(#_PT3_Regs + PSG_REG_AMP_C)
	LD   (#_PT3_Regs + PSG_REG_AMP_A),A				// Set Volume A
	LD   IX,#_ChanB
	LD   HL,(#_PT3_Regs + PSG_REG_TONE_B)
	CALL CHREGS
	LD   (#_PT3_Regs + PSG_REG_TONE_B),HL			// Set Tone B
	LD   A,(#_PT3_Regs + PSG_REG_AMP_C)
	LD   (#_PT3_Regs + PSG_REG_AMP_B),A				// Set Volume B
	LD   IX,#_ChanC
	LD   HL,(#_PT3_Regs + PSG_REG_TONE_C)
	CALL CHREGS
	LD   (#_PT3_Regs + PSG_REG_TONE_C),HL			// Set Tone C

	LD   HL,(#_Ns_Base)    ;Ns_Base_AddToNs
	LD   A,H
	ADD  A,L
	LD   (#_PT3_Regs + PSG_REG_NOISE),A				// Set Noise

	ld   A,(#_PT3_AddToEn)
	LD   E,A
	ADD  A,A
	SBC  A,A
	LD   D,A
	LD   HL,(#_EnvBase)
	ADD  HL,DE
	LD   DE,(#_CurESld)
	ADD  HL,DE
	LD  (#_PT3_Regs + PSG_REG_ENV),HL

	XOR  A
	LD   HL,#_CurEDel
	OR   (HL)

	RET  Z
	DEC  (HL)
	RET  NZ
	LD   A,(#_PT3_Env_Del)
	LD   (HL),A
	LD   HL,(#_PT3_ESldAdd)
	ADD  HL,DE
	LD   (#_CurESld),HL

  
;  LD   HL,#_PT3_State
;  BIT  1,(HL)   ; pause mode
;  JP   Z,MUTE
  
	RET


;Check Loop
CHECKLP:	
	LD   HL,#_PT3_State
	;SET  7,(HL)   ;loop control
	BIT  4,(HL)   ;loop bit 
	RET  NZ
  
;=0 - No loop

;remove the lock if finished the song. Bug #11
	RES  1,(HL) ;set pause mode

	POP  HL
	LD   HL,#_DelyCnt
	INC  (HL)
;  LD   HL,#_ChanA + CHNPRM_NtSkCn
;  INC  (HL)
    
	JP   MUTE



PD_OrSm:	
	LD   -12 + CHNPRM_Env_En(IX),#0
	CALL SETORN
	LD   A,(BC)
	INC  BC
	RRCA

PD_SAM:	
	ADD  A,A
PD_SAM_:	
	LD   E,A
	LD   D,#0
	ld	 HL,(#_PT3_SamPtrs)
	ADD  HL,DE
	LD   E,(HL)
	INC  HL
	LD   D,(HL)
	ld	 HL,(#_PT3_ModAddr)
	ADD  HL,DE
	LD   -12 + CHNPRM_SamPtr(IX),L
	LD   -12 + CHNPRM_SamPtr + 1(IX),H
	JR   PD_LOOP

PD_VOL:	
	RLCA
	RLCA
	RLCA
	RLCA
	LD   -12 + CHNPRM_Volume(IX),A
	JR   PD_LP2
	
PD_EOff:	
	LD   -12 + CHNPRM_Env_En(IX),A
	LD   -12 + CHNPRM_PsInOr(IX),A
	JR   PD_LP2

PD_SorE:	
	DEC  A
	JR   NZ,PD_ENV
	LD   A,(BC)
	INC  BC
	LD   -12 + CHNPRM_NNtSkp(IX),A
	JR   PD_LP2

PD_ENV:	
	CALL SETENV
	JR   PD_LP2

PD_ORN:	
	CALL SETORN
	JR   PD_LOOP
       
PD_ESAM:	
	LD   -12 + CHNPRM_Env_En(IX),A
	LD   -12 + CHNPRM_PsInOr(IX),A
	CALL NZ,SETENV
	LD   A,(BC)
	INC  BC
	JR   PD_SAM_

PTDECOD: 
	LD   A,-12 + CHNPRM_Note(IX)
	LD   (#_PT3_PrNote),A           ;LD   (#PrNote + 1),A
	LD   L,CHNPRM_CrTnSl-12(IX)
	LD   H,CHNPRM_CrTnSl + 1-12(IX)
	LD  (#_PT3_PrSlide),HL

PD_LOOP:	
	ld   DE,#0x2010
PD_LP2:	
	ld   A,(BC)
	inc  BC
	ADD  A,E
	JR   C,PD_OrSm
	ADD  A,D
	JR   Z,PD_FIN
	JR   C,PD_SAM
	ADD  A,E
	JR   Z,PD_REL
	JR   C,PD_VOL
	ADD  A,E
	JR   Z,PD_EOff
	JR   C,PD_SorE
	ADD  A,#96
	JR   C,PD_NOTE
	ADD  A,E
	JR   C,PD_ORN
	ADD  A,D
	JR   C,PD_NOIS
	ADD  A,E
	JR   C,PD_ESAM
	ADD  A,A
	LD   E,A

	LD   HL,#(SPCCOMS + 0xDF20)  ;LD HL,((SPCCOMS + $DF20) % 65536)
;	PUSH DE
;	LD   DE,#0xDF20
;	LD   HL,#SPCCOMS	
;	ADD  HL,DE
;	POP  DE	
  
	ADD  HL,DE
	LD   E,(HL)
	INC  HL
	LD   D,(HL)
	PUSH DE

	JR   PD_LOOP

PD_NOIS:	
	LD   (#_Ns_Base),A
	JR   PD_LP2

PD_REL:	
	RES  0,-12 + CHNPRM_Flags(IX)
	JR   PD_RES
	
PD_NOTE:	
	ld   -12 + CHNPRM_Note(IX),A
	SET  0,-12 + CHNPRM_Flags(IX)
	XOR  A

PD_RES:	
	LD   (#_PT3_PDSP),SP
	LD   SP,IX
	LD   H,A
	LD   L,A
	PUSH HL
	PUSH HL
	PUSH HL
	PUSH HL
	PUSH HL
	PUSH HL
	LD   SP,(#_PT3_PDSP)

PD_FIN:	
	ld   A,-12 + CHNPRM_NNtSkp(IX)
	ld   -12 + CHNPRM_NtSkCn(IX),A
	ret

C_PORTM: 
	RES  2,-12 + CHNPRM_Flags(IX)
	LD   A,(BC)
	INC  BC
  
;SKIP PRECALCULATED TONE DELTA (BECAUSE
;CANNOT BE RIGHT AFTER PT3 COMPILATION)
	INC  BC
	INC  BC
	LD   -12 + CHNPRM_TnSlDl(IX),A
	LD   -12 + CHNPRM_TSlCnt(IX),A
	LD   DE,(#_PT3_NoteTable)
	LD   A,-12 + CHNPRM_Note(IX)
	LD   -12 + CHNPRM_SlToNt(IX),A
	ADD  A,A
	LD   L,A
	LD   H,#0
	ADD  HL,DE
	LD   A,(HL)
	INC  HL
	LD   H,(HL)
	LD   L,A
	PUSH HL
	LD   A,(#_PT3_PrNote)            ;<--- LD   A,#0x3E
	LD   -12 + CHNPRM_Note(IX),A
	ADD  A,A
	LD   L,A
	LD   H,#0
	ADD  HL,DE
	LD   E,(HL)
	INC  HL
	LD   D,(HL)
	POP  HL
	SBC  HL,DE
	LD   -12 + CHNPRM_TnDelt(IX),L
	LD   -12 + CHNPRM_TnDelt + 1(IX),H
	LD   DE,(#_PT3_PrSlide)             ;<--- change to Kun version
	LD   -12 + CHNPRM_CrTnSl(IX),E       ;<---
	LD   -12 + CHNPRM_CrTnSl + 1(IX),D     ;<---
	LD   A,(BC) ;SIGNED TONE STEP
	INC  BC
	EX   AF,AF
	LD   A,(BC)
	INC  BC
	AND  A
	JR   Z,NOSIG
	EX   DE,HL
NOSIG:	
	SBC  HL,DE
	JP   P,SET_STP
	CPL
	EX   AF,AF
	NEG
	EX   AF,AF
SET_STP:	
	LD   -12 + CHNPRM_TSlStp + 1(IX),A
	EX   AF,AF
	ld   -12 + CHNPRM_TSlStp(IX),A
	ld   -12 + CHNPRM_COnOff(IX),#0
	ret

C_GLISS:	
	SET  2,-12 + CHNPRM_Flags(IX)
	LD   A,(BC)
	INC  BC
	LD  -12 + CHNPRM_TnSlDl(IX),A
	LD  -12 + CHNPRM_TSlCnt(IX),A
	LD   A,(BC)
	INC  BC
	EX   AF,AF
	LD   A,(BC)
	INC  BC
	JR   SET_STP

C_SMPOS:	
	LD   A,(BC)
	INC  BC
	LD   -12 + CHNPRM_PsInSm(IX),A
	RET

C_ORPOS:	
	LD   A,(BC)
	INC  BC
	LD   -12 + CHNPRM_PsInOr(IX),A
	RET

C_VIBRT:
	LD   A,(BC)
	INC  BC
	LD   -12 + CHNPRM_OnOffD(IX),A
	LD   -12 + CHNPRM_COnOff(IX),A
	LD   A,(BC)
	INC  BC
	LD   -12 + CHNPRM_OffOnD(IX),A
	XOR  A
	LD   -12 + CHNPRM_TSlCnt(IX),A
	LD   -12 + CHNPRM_CrTnSl(IX),A
	LD   -12 + CHNPRM_CrTnSl + 1(IX),A
	RET

C_ENGLS:
	LD   A,(BC)
	INC  BC
	LD   (#_PT3_Env_Del),A
	LD   (#_CurEDel),A
	LD   A,(BC)
	INC  BC
	LD   L,A
	LD   A,(BC)
	INC  BC
	LD   H,A
	LD   (#_PT3_ESldAdd),HL
	RET

C_DELAY:
	LD   A,(BC)
	INC  BC
	LD   (#_PT3_Delay),A
	RET
	
SETENV:
	LD   -12 + CHNPRM_Env_En(IX),E
	LD   (#_PT3_Regs + PSG_REG_SHAPE),A
	LD   A,(BC)
	INC  BC
	LD   H,A
	LD   A,(BC)
	INC  BC
	LD   L,A
	LD   (#_EnvBase),HL
	XOR  A
	LD   -12 + CHNPRM_PsInOr(IX),A
	LD   (#_CurEDel),A
	LD   H,A
	LD   L,A
	LD   (#_CurESld),HL
  
C_NOP:	
	RET

SETORN:	
	ADD  A,A
	LD   E,A
	LD   D,#0
	LD   -12 + CHNPRM_PsInOr(IX),D
	ld	 HL,(#_PT3_OrnPtrs) 
	ADD  HL,DE
	LD   E,(HL)
	INC  HL
	LD   D,(HL)
	ld   HL,(#_PT3_ModAddr) 
	ADD  HL,DE
	LD   -12 + CHNPRM_OrnPtr(IX),L
	LD   -12 + CHNPRM_OrnPtr + 1(IX),H
	RET





;-------------------------------------------------------------------------------
;ALL 16 ADDRESSES TO PROTECT FROM BROKEN PT3 MODULES 

SPCCOMS: 
.dw C_NOP			  ; ## COMPROBAR Q NO SEA AUTOMODIF
.dw C_GLISS			; (parece que no lo toca nada)
.dw C_PORTM
.dw C_SMPOS
.dw C_ORPOS
.dw C_VIBRT
.dw C_NOP
.dw C_NOP
.dw C_ENGLS
.dw C_DELAY
.dw C_NOP
.dw C_NOP
.dw C_NOP
.dw C_NOP
.dw C_NOP
.dw C_NOP


CHREGS:	
	XOR  A
	LD   (#_PT3_Regs + PSG_REG_AMP_C),A				// Set Volume C
	BIT   0,CHNPRM_Flags(IX)
	PUSH  HL
	JP    Z,CH_EXIT
	ld	 (#_PT3_CSP),sp
	LD   L,CHNPRM_OrnPtr(IX)
	LD   H,CHNPRM_OrnPtr + 1(IX)
	LD   SP,HL
	POP  DE
	LD   H,A
	LD   A,CHNPRM_PsInOr(IX)
	LD   L,A
	ADD  HL,SP
	INC  A
	CP   D
	JR   C,CH_ORPS
	LD   A,E
CH_ORPS:	
	LD   CHNPRM_PsInOr(IX),A
	LD   A,CHNPRM_Note(IX)
	ADD  A,(HL)
	JP   P,CH_NTP
	XOR  A
CH_NTP:	
	CP   #96
	JR   C,CH_NOK
	LD   A,#95
CH_NOK:	
	ADD  A,A
	EX   AF,AF
	LD   L,CHNPRM_SamPtr(IX)
	LD   H,CHNPRM_SamPtr + 1(IX)
	LD   SP,HL
	POP  DE
	LD   H,#0
	LD   A,CHNPRM_PsInSm(IX)
	LD   B,A
	ADD  A,A
	ADD  A,A
	LD   L,A
	ADD  HL,SP
	LD   SP,HL
	LD   A,B
	INC  A
	CP   D
	JR   C,CH_SMPS
	LD   A,E
CH_SMPS:	
	LD   CHNPRM_PsInSm(IX),A
	POP  BC
	POP  HL
	LD   E,CHNPRM_TnAcc(IX)
	LD   D,CHNPRM_TnAcc + 1(IX)
	ADD  HL,DE
	BIT  6,B
	JR   Z,CH_NOAC
	LD   CHNPRM_TnAcc(IX),L
	LD   CHNPRM_TnAcc + 1(IX),H
CH_NOAC: 
	EX   DE,HL
	EX   AF,AF
	LD   L,A
	LD   H,#0
	LD   SP,(#_PT3_NoteTable)
	ADD  HL,SP
	LD   SP,HL
	POP  HL
	ADD  HL,DE
	LD   E,CHNPRM_CrTnSl(IX)
	LD   D,CHNPRM_CrTnSl + 1(IX)
	ADD  HL,DE
	ld	 SP,(#_PT3_CSP)
	EX   (SP),HL
	XOR  A
	OR   CHNPRM_TSlCnt(IX)
	JR   Z,CH_AMP
	DEC  CHNPRM_TSlCnt(IX)
	JR   NZ,CH_AMP
	LD   A,CHNPRM_TnSlDl(IX)
	LD   CHNPRM_TSlCnt(IX),A
	LD   L,CHNPRM_TSlStp(IX)
	LD   H,CHNPRM_TSlStp + 1(IX)
	LD   A,H
	ADD  HL,DE
	LD   CHNPRM_CrTnSl(IX),L
	LD   CHNPRM_CrTnSl + 1(IX),H
	BIT  2,CHNPRM_Flags(IX)
	JR   NZ,CH_AMP
	LD   E,CHNPRM_TnDelt(IX)
	LD   D,CHNPRM_TnDelt + 1(IX)
	AND  A
	JR   Z,CH_STPP
	EX   DE,HL
CH_STPP: 
	SBC  HL,DE
	JP   M,CH_AMP
	LD   A,CHNPRM_SlToNt(IX)
	LD   CHNPRM_Note(IX),A
	XOR  A
	LD   CHNPRM_TSlCnt(IX),A
	LD   CHNPRM_CrTnSl(IX),A
	LD   CHNPRM_CrTnSl + 1(IX),A
CH_AMP:	
	LD   A,CHNPRM_CrAmSl(IX)
	BIT  7,C
	JR   Z,CH_NOAM
	BIT  6,C
	JR   Z,CH_AMIN
	CP   #15
	JR   Z,CH_NOAM
	INC  A
	JR   CH_SVAM
CH_AMIN:	
	CP   #-15
	JR   Z,CH_NOAM
	DEC  A
CH_SVAM:	
	LD   CHNPRM_CrAmSl(IX),A
CH_NOAM:	
	LD   L,A
	LD   A,B
	AND  #15
	ADD  A,L
	JP   P,CH_APOS
	XOR  A
CH_APOS:	
	CP   #16
	JR   C,CH_VOL
	LD   A,#15
CH_VOL:	
	OR   CHNPRM_Volume(IX)
	LD   L,A
	LD   H,#0
	LD   DE,#_PT3_Regs  ;_VT_
	ADD  HL,DE
	LD   A,(HL)
CH_ENV:	
	BIT  0,C
	JR   NZ,CH_NOEN
	OR   CHNPRM_Env_En(IX)
CH_NOEN:	
	LD   (#_PT3_Regs + PSG_REG_AMP_C),A				// Set volume C
	BIT  7,B
	LD   A,C
	JR   Z,NO_ENSL
	RLA
	RLA
	SRA  A
	SRA  A
	SRA  A
	ADD  A,CHNPRM_CrEnSl(IX) ;SEE COMMENT BELOW
	BIT  5,B
	JR   Z,NO_ENAC
	LD   CHNPRM_CrEnSl(IX),A
NO_ENAC:	
	ld	 HL,#_PT3_AddToEn 
	ADD  A,(HL) ;BUG IN PT3 - NEED WORD HERE.
	   ;FIX IT IN NEXT VERSION?
	LD   (HL),A
	JR   CH_MIX
NO_ENSL: 
	RRA
	ADD  A,CHNPRM_CrNsSl(IX)
	LD   (#_AddToNs),A
	BIT  5,B
	JR   Z,CH_MIX
	LD   CHNPRM_CrNsSl(IX),A
CH_MIX:	
	LD   A,B
	RRA
	AND  #0x48
CH_EXIT:	
	LD   HL,#_PT3_Regs + PSG_REG_MIXER
	OR   (HL)
	RRCA
	LD   (HL),A
	POP  HL
	XOR  A
	OR   CHNPRM_COnOff(IX)
	RET  Z
	DEC  CHNPRM_COnOff(IX)
	RET  NZ
	XOR  CHNPRM_Flags(IX)
	LD   CHNPRM_Flags(IX),A
	RRA
	LD   A,CHNPRM_OnOffD(IX)
	JR   C,CH_ONDL
	LD   A,CHNPRM_OffOnD(IX)
CH_ONDL:	
	LD   CHNPRM_COnOff(IX),A
	RET



;------------------------------------------------------------------------------- DATAS

EMPTYSAMORN: 
  .db 0,1,0,0x90 
;delete $90 if you dont need default sample  ; # pongo el 0 aqui

__endasm;
}
// ----------------------------------------------------------------------------- END

