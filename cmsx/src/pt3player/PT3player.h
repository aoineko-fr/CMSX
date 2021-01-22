//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
//   SDCC Vortex Tracker II PT3 player for MSX
//
//   Version: 1.3 (20/01/2021)
//   Architecture: MSX
//   Format: C Object (SDCC .rel)
//   Programming language: C and Z80 assembler
//
//   Authors:
//    - Vortex Tracker II v1.0 PT3 player for ZX Spectrum by S.V.Bulba 
//      <vorobey@mail.khstu.ru> http://bulba.at.kz
//    - (09-Jan-05) Adapted to MSX by Alfonso D. C. aka Dioniso 
//      <dioniso072@yahoo.es>
//    - Arrangements for MSX ROM: MSXKun/Paxanga soft > 
//      http://paxangasoft.retroinvaders.com/
//    - asMSX version: SapphiRe > http://www.z80st.es/
//    - Adapted to SDCC: mvac7/303bcn > <mvac7303b@gmail.com>
//    - Optimized and adapted for CMASX: Guillaume 'Aoineko' Blanchard <aoineko@free.fr>
//
//   Description:
//     Adaptation of the Vortex Tracker II PT3 Player for MSX to be used in 
//     software development in C (SDCC). 
//     
//   History of versions:
//    - 1.2 (04/01/2021) Assignment of frequency table memory address to NoteTable
//    - 1.1 (28/05/2019) Adaptation to SDCC of asMSX version by SapphiRe.
//    - 1.0 (21/10/2016) Adaptation to SDCC of the ROM version by Kun.
//
//-----------------------------------------------------------------------------
//  
//   Dioniso version:
//    - No version detection (just for Vortex Tracker II and PT3.5).
//    - No frequency table decompression (default is number 2). 
//      Take table as desired, in separate file
//    - No volume table decompression (Vortex Tracker II/PT3.5 volume table used).
//   
//   msxKun version:
//    - Usable from ROM (only has in RAM working area, as little as possible).
//   
//   SapphiRe version:
//    - This version of the replayer uses a fixed volume and note table, if you need a 
//      different note table you can copy it from TABLES.TXT file, distributed with the
//      original PT3 distribution. This version also allows the use of PT3 commands.
//    - PLAY and PSG WRITE routines seperated to allow independent calls
//   
//   mvac7 version:
//    - Adaptation to C (SDCC) of the SapphiRe version.
//
//   Aoineko version:
//    - Adaptation for CMSX library
//    - Optimize PT3_AYPlay (528 T-States instead of 906) and rename it to PT3_UpdatePSG
//    - Optimize PT3_InitSong (quicker and now support data withou the 100 bytes header)


//-----------------------------------------------------------------------------
#pragma once

#include "core.h"
#include "psg.h"

#define PT3_SKIP_HEADER	1
#define PT3_AUTOPLAY	0
#define PT3_EXTRA		1

// Constants

// #define AY0index 0xA0
// #define AY0write 0xA1
// #define AY0read  0xA2


//ChannelsVars
//struc	CHNPRM
//reset group
#define CHNPRM_PsInOr 0	 //RESB 1
#define CHNPRM_PsInSm 1	 //RESB 1
#define CHNPRM_CrAmSl 2	 //RESB 1
#define CHNPRM_CrNsSl 3	 //RESB 1
#define CHNPRM_CrEnSl 4	 //RESB 1
#define CHNPRM_TSlCnt 5	 //RESB 1
#define CHNPRM_CrTnSl 6	 //RESW 1
#define CHNPRM_TnAcc  8	 //RESW 1
#define CHNPRM_COnOff 10 //RESB 1
//reset group

#define CHNPRM_OnOffD 11 //RESB 1

//IX for PTDECOD here (+12)
#define CHNPRM_OffOnD 12 //RESB 1
#define CHNPRM_OrnPtr 13 //RESW 1
#define CHNPRM_SamPtr 15 //RESW 1
#define CHNPRM_NNtSkp 17 //RESB 1
#define CHNPRM_Note   18 //RESB 1
#define CHNPRM_SlToNt 19 //RESB 1
#define CHNPRM_Env_En 20 //RESB 1
#define CHNPRM_Flags  21 //RESB 1

//Enabled - 0,SimpleGliss - 2
#define CHNPRM_TnSlDl 22 //RESB 1
#define CHNPRM_TSlStp 23 //RESW 1
#define CHNPRM_TnDelt 25 //RESW 1
#define CHNPRM_NtSkCn 27 //RESB 1
#define CHNPRM_Volume 28 //RESB 1
#define CHNPRM_Size   29 //RESB 1
// endstruc

//- struc AR -
//#define AR_TonA  0	//RESW 1
//#define AR_TonB  2	//RESW 1
//#define AR_TonC  4	//RESW 1
//#define AR_Noise 6	//RESB 1
// #define AR_Mixer 7	//RESB 1
#define AR_AmplA 8	//RESB 1
#define AR_AmplB 9	//RESB 1
#define AR_AmplC 10	//RESB 1
#define AR_Env   11	//RESW 1
#define AR_EnvTp 13	//RESB 1
//endstruc



//VARS:
extern u8 ChanA[29]; //CHNPRM_Size
extern u8 ChanB[29];
extern u8 ChanC[29];			


extern u8 DelyCnt;
extern u16 CurESld;		
extern u8 CurEDel;


//Ns_Base_AddToNs:
extern u8 Ns_Base;		
extern u8 AddToNs;		


extern u8 PT3_Regs[14];
extern u16 EnvBase;
extern u8 VAR0END[240];


/*            
Switches: 1=ON; 0=OFF
- BIT 0 = ?
- BIT 1 = PLAYER ON/OFF
- BIT 2 = ?
- BIT 3 = ?
- BIT 4 = LOOP ON/OFF
- BIT 7 = set each time, when loop point is passed 
*/
extern u8 PT3_State;		// Before called PT3_SETUP

#define PT3_STATE_PLAY (1<<1)
#define PT3_STATE_LOOP (1<<4)

/* --- Workarea --- (points to RAM that was previously in self-modifying code)
 -The status byte in SETUP should be something like this (CH enable/disable is not yet)
|EP|0|0|0|CH3|CH2|CH1|LP|

LP: Loop enable/disable. A 1 if we want the track to play only once. 
EP: End point. A 1 each time the topic ends. 
CH1-CH3: Channel enable/disable. A 1 if we don't want the channel to ring. (STILL NOT GOING!!)
*/
//extern char PT3_SETUP;   set bit0 to 1, if you want to play without looping
//				           bit7 is set each time, when loop point is passed          

extern u16 PT3_ModAddr;		// Song data address
extern u16 PT3_CrPsPtr;		// Cursor position in pattern
extern u16 PT3_SamPtrs;		// Sample info?
extern u16 PT3_OrnPtrs;		// Ornament pattern

extern u16 PT3_PDSP;		// pilasave
extern u16 PT3_CSP;			// pilsave2
extern u16 PT3_PSP;			// pilsave3
									   
extern u8  PT3_PrNote;		//
extern u16 PT3_PrSlide;		//
  
extern u16 PT3_AdInPtA;		// play data pattern
extern u16 PT3_AdInPtB;		// play data
extern u16 PT3_AdInPtC;		// play data
  
extern u16 PT3_LPosPtr;		// Position Ptr?
extern u16 PT3_PatsPtr;		// Pat Ptr

extern u8  PT3_Delay;		// delay
extern u8  PT3_AddToEn;		// Envelope data (No cal ya que no usa Envs??)
extern u8  PT3_Env_Del;		// Envelope data (idem)
extern u16 PT3_ESldAdd;		// Envelope data (idem)

extern const void* PT3_NoteTable;   // Note table memory address

#if (PT3_EXTRA)

extern u16 PT3_SrtCrPsPtr;	// Cursor position in pattern at start

#endif // (PT3_EXTRA)


/* =============================================================================
 PT3_Init
 Description: Initialize the Player
 Input:       -
 Output:      -
============================================================================= */
void PT3_Init();



/* =============================================================================
 PT3_Mute
 Description: Silence the PSG
 Input:       -
 Output:      -
============================================================================= */
void PT3_Mute();



/* =============================================================================
 PT3_Loop
 Description: Change state of loop
 Input:       - 0=off ; 1=on  (false = 0, true = 1)
 Output:      -
============================================================================= */
void PT3_SetLoop(u8 loop) __z88dk_fastcall; 



/* =============================================================================
 PT3_Pause
 Description: Pause song playback
 Input:       -
 Output:      -
============================================================================= */
void PT3_Pause();



/* =============================================================================
 PT3_Resume
 Description: Resume song playback
 Input:       -
 Output:      -
============================================================================= */  	
void PT3_Resume();



/* -----------------------------------------------------------------------------
PT3_InitSong
(u16) Song data address. Subtract 100 if you delete the header of the PT3 file.
(char) Loop - 0=off ; 1=on  (false = 0, true = 1));
----------------------------------------------------------------------------- */
void PT3_InitSong(const void* songADDR) __z88dk_fastcall;



/* -----------------------------------------------------------------------------
PT3PlayAY
Play Song. 
Send data to AY registers
Execute on each interruption of VBLANK
----------------------------------------------------------------------------- */
void PT3_UpdatePSG();



/* -----------------------------------------------------------------------------
PT3Decode
Decode a frame from PT3 song
----------------------------------------------------------------------------- */
void PT3_Decode(); 


// mute functions, 0=off, other=on
//void muteChannelA(char value);
//void muteChannelB(char value);
//void muteChannelC(char value);



//-----------------------------------------------------------------------------
/// Set note table memory address
inline void PT3_SetNoteTable(const void* nt)
{
	PT3_NoteTable = nt;
}

//-----------------------------------------------------------------------------
/// Check if loop is active
inline bool PT3_GetLoop()
{
	return (PT3_State & PT3_STATE_LOOP);
}

//-----------------------------------------------------------------------------
/// Check if music is playing
inline bool PT3_IsPlaying()
{
	return (PT3_State & PT3_STATE_PLAY);
}

//-----------------------------------------------------------------------------
/// Play the current music
inline void PT3_Play()
{
	PT3_Resume();
}

//-----------------------------------------------------------------------------
/// 
inline u8 PT3_GetVolume(u8 chan)
{
	return PT3_Regs[PSG_REG_AMP_A + chan] & 0x0F;
}

//-----------------------------------------------------------------------------
/// 
inline u16 PT3_GetFrequency(u8 chan)
{
	return *(u16*)&PT3_Regs[PSG_REG_TONE_A + chan * 2];
}

//-----------------------------------------------------------------------------
/// 
inline u8 PT3_GetPSGRegister(u8 reg)
{
	return PT3_Regs[reg];
}

#if (PT3_EXTRA)

// 
inline u8 PT3_GetPattern()
{
	return PT3_CrPsPtr - PT3_SrtCrPsPtr;
}

#endif // (PT3_EXTRA)

