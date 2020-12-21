//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Configuration defines
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// TARGET options
#define TARGET_BIN			100 // BASIC binary program (8000h~)
#define TARGET_ROM16		200 // 16KB ROM in page 1 (4000h ~ 7FFFh)
#define TARGET_ROM16P2		205 // 16KB ROM in page 2 (8000h ~ 7FFFh)
#define TARGET_ROM32		210 // 32KB ROM in page 1&2 (4000h ~ BFFFh)
#define TARGET_ROM48		220 // 48KB ROM in page 0-2 (0000h ~ BFFFh) No direct acces to Main-ROM @todo To be implemented!
#define TARGET_DOS			300 // MSX-DOS program (0100h~) No direct acces to Main-ROM
#define TARGET_DOSARG		310 // MSX-DOS program (using command line arguments ; 0100h~) No direct acces to Main-ROM
// TARGET is defined by the build tool

//-----------------------------------------------------------------------------
// TARGET_TYPE options
#define TARGET_TYPE_BIN		0 // BASIC binary program
#define TARGET_TYPE_ROM		1 // ROM program
#define TARGET_TYPE_DOS		2 // MSX-DOS program
// TARGET_TYPE is defined by the build tool

//-----------------------------------------------------------------------------
// MSX_VERSION options
#define MSX_1				0 // MSX
#define MSX_2				1 // MSX 2
#define MSX_2Plus			2 // MSX 2+
#define MSX_TurboR			3 // MSX Turbo-R

//-----------------------------------------------------------------------------
// RENDER_MODE options
#define RENDER_BIOS			0 // Use BIOS routines
#define RENDER_VDP			1 // Use direct VDP registers

//-----------------------------------------------------------------------------
// BIOS_CALL options
#define CALL_DIRECT			0 // Use direct access to Bios routines (Main-ROM must be in Slot 0)
#define CALL_INTERSLOT		1 // Use inter-slot access to Bios routines

//-----------------------------------------------------------------------------
// RANDOM_METHOD
#define RANDOM_LFSR_LCG_16	0 // Combined LFSR/LCG (16-bit seeds)
#define RANDOM_LFSR_LCG_32	1 // Combined LFSR/LCG (32-bit seeds)
#define RANDOM_XORSHIFT		2 // Xorshift RNGs (16-bit seeds)
