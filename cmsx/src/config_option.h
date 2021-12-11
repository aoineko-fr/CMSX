//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
// Configuration defines
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// BUILD
//-----------------------------------------------------------------------------

// TARGET options
#define TARGET_BIN			  0 // BASIC binary program (8000h~)
#define TARGET_ROM16		110 // see ROM16P1
#define TARGET_ROM16P1		110 // 16KB ROM in page 1 (4000h ~ 7FFFh)
#define TARGET_ROM16P2		120 // 16KB ROM in page 2 (8000h ~ BFFFh)
#define TARGET_ROM32		310 // see ROM32P1
#define TARGET_ROM32P0		300 // 32KB ROM in pages 0-1 (0000h ~ 7FFFh)
#define TARGET_ROM32P1		310 // 32KB ROM in pages 1-2 (4000h ~ BFFFh)
#define TARGET_ROM48		400 // 48KB ROM in pages 0-2 (0000h ~ BFFFh) No direct acces to Main-ROM
#define TARGET_ROM48_ISR	401 // 48KB ROM in pages 0-2 (0000h ~ BFFFh) With ISR replacement
#define TARGET_ROM64_ISR	601 // 64KB ROM in pages 0-3 (0000h ~ FFFFh) With ISR replacement
#define TARGET_DOS			900 // MSX-DOS program (0100h~) No direct acces to Main-ROM
#define TARGET_DOSARG		910 // MSX-DOS program (using command line arguments ; 0100h~) No direct acces to Main-ROM
// TARGET is defined by the build tool

// TARGET_TYPE options
#define TARGET_TYPE_BIN		0 // BASIC binary program
#define TARGET_TYPE_ROM		1 // ROM program
#define TARGET_TYPE_DOS		2 // MSX-DOS program
// TARGET_TYPE is defined by the build tool

// MSX_VERSION options
#define MSX_1				0 // MSX 1
#define MSX_2				1 // MSX 2
#define MSX_2P				2 // MSX 2+
#define MSX_TR				3 // MSX Turbo-R
// MSX_VERSION is defined by the build tool

//-----------------------------------------------------------------------------
// BIOS MODULE
//-----------------------------------------------------------------------------

// RENDER_MODE options
#define RENDER_BIOS			0 // Use BIOS routines
#define RENDER_VDP			1 // Use direct VDP registers

// BIOS_CALL options
#define CALL_DIRECT			0 // Use direct access to Bios routines (Main-ROM must be in Slot 0)
#define CALL_INTERSLOT		1 // Use inter-slot access to Bios routines

//-----------------------------------------------------------------------------
// PRINT MODULE
//-----------------------------------------------------------------------------

// PRINT_WIDTH
#define PRINT_WIDTH_6		0 // 
#define PRINT_WIDTH_8		1 // 
#define PRINT_WIDTH_X		2 // Not supported (fall back to PRINT_WIDTH_8)
// PRINT_HEIGHT
#define PRINT_HEIGHT_8		1 // 
#define PRINT_HEIGHT_X		2 // 

//-----------------------------------------------------------------------------
// VDP MODULE
//-----------------------------------------------------------------------------

// VDP_VRAM
#define VDP_VRAM_ADDR_14	0 // Use 14-bits 16K VRAM addressing for MSX 1 (u16)
#define VDP_VRAM_ADDR_17	1 // Use 17-bits 128K VRAM addressing for MSX 2/2+/Turbo-R (u32)

// VDP_UNIT
#define VDP_UNIT_U8			0 // X and Y use 8-bits values
#define VDP_UNIT_X16		1 // X use 16-bits and Y use 8-bits values
#define VDP_UNIT_Y16		2 // X use 8-bits and Y use 16-bits values
#define VDP_UNIT_U16		3 // X and Y use 16-bits values


//-----------------------------------------------------------------------------
// DRAW MODULE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MATH MODULE
//-----------------------------------------------------------------------------

// RANDOM_METHOD
#define RANDOM_LFSR_LCG_16	0 // Combined LFSR/LCG (16-bit seeds)
#define RANDOM_LFSR_LCG_32	1 // Combined LFSR/LCG (32-bit seeds)
#define RANDOM_XORSHIFT		2 // XOR Shift RNGs (16-bit seeds)
#define RANDOM_GNU			3 // GNU C Library function (16-bit seed)
#define RANDOM_LINEAR		4 // Linear congruential generator

//-----------------------------------------------------------------------------
// MISC
//-----------------------------------------------------------------------------

// PROFILE_MODE
#define PROFILE_DISABLE		0 // No profiler
#define PROFILE_OPENMSX_G	1 // Grauw profile script for OpenMSX
#define PROFILE_OPENMSX_S	2 // Salutte profile script for OpenMSX