//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// ayFX Replayer
//-----------------------------------------------------------------------------
// Based on ayFX Replayer v1.31 by SapphiRe
//  - v1.31		Fixed bug on previous version, only PSG channel C worked
//  - v1.3		Fixed volume and Relative volume versions on the same file, conditional compilation
//				Support for dynamic or fixed channel allocation
//  - v1.2f/r	ayFX bank support
//  - v1.11f/r	If a frame volume is zero then no AYREGS update
//  - v1.1f/r	Fixed volume for all ayFX streams
//  - v1.1		Explicit priority (as suggested by AR)
//  - v1.0f		Bug fixed (error when using noise)
//  - v1.0		Initial release
//-----------------------------------------------------------------------------
#pragma once

// INCLUDES

// CMSX headers
#include "core.h"
#include "psg.h"

// DEFINES

#define AYFX_RELATIVE 0

#define AYFX_MODE_FIXED		0
#define AYFX_MODE_SWITCH	1

extern u8  ayFX_Mode;		// ayFX mode
extern u16 ayFX_Bank;		// Current ayFX Bank
extern u8  ayFX_Priority;	// Current ayFX stream priotity
extern u16 ayFX_Pointer;	// Pointer to the current ayFX stream
extern u16 ayFX_Tone;		// Current tone of the ayFX stream
extern u8  ayFX_Noise;		// Current noise of the ayFX stream
extern u8  ayFX_Volume;		// Current volume of the ayFX stream
extern u8  ayFX_Channel;	// PSG channel to play the ayFX stream
#if (AYFX_RELATIVE)
extern u16 ayFX_VT;			// ayFX relative volume table pointer
#endif


// PROTOTYPES

///
void ayFX_Init(void* bank) __FASTCALL;


///
void ayFX_Play(u16 snd_prio) __FASTCALL __naked;

///
void ayFX_Update();

///
inline void ayFX_SetMode(u8 mode) { ayFX_Mode = mode; }

///
inline void ayFX_SetChannel(u8 chan) { ayFX_Channel = chan; }


///
inline u8 ayFX_GetNumber() { return *(u8*)ayFX_Bank; }
