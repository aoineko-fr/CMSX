//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Library configuration
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// BUILD
//-----------------------------------------------------------------------------

// Target
// - TARGET_BIN ...................	BASIC binary program
// - TARGET_ROM16 ................. 16KB ROM in page 1
// - TARGET_ROM16P0 ............... 16KB ROM in page 0
// - TARGET_ROM16P2 ............... 16KB ROM in page 2
// - TARGET_ROM32 ................. 32KB ROM in page 1-2
// - TARGET_ROM32P0 ............... 32KB ROM in page 0-1
// - TARGET_ROM48 ................. 48KB ROM in page 0-2
// - TARGET_DOS ................... MSX-DOS program
// - TARGET_DOSARG ................ MSX-DOS program (using command line arguments)
// TARGET is defined by the build tool

// Target type
// - TARGET_TYPE_BIN .............. BASIC binary program
// - TARGET_TYPE_ROM .............. ROM
// - TARGET_TYPE_DOS .............. MSX-DOS program
// TARGET_TYPE is defined by the build tool

// MSX version
// - MSX_1 ........................ MSX
// - MSX_2 ........................ MSX 2
// - MSX_2Plus .................... MSX 2+
// - MSX_TurboR ................... MSX Turbo-R
#define MSX_VERSION					MSX_2

//-----------------------------------------------------------------------------
// BIOS MODULE
//-----------------------------------------------------------------------------

// Render mode
// - RENDER_VDP ................... Use direct access to VDP registers
// - RENDER_BIOS .................. Use BIOS routines
#define RENDER_MODE					RENDER_VDP

// Default bios access
// - CALL_DIRECT .................. Use direct access to Bios routines (ROM slot must be selected in corresponding page)
// - CALL_INTERSLOT ............... Use inter-slot access to Bios routines (through CALSLT routine)
#define CALL_MAINROM				CALL_DIRECT
#define CALL_SUBROM					CALL_INTERSLOT
#define CALL_DISCROM				CALL_INTERSLOT

// MAIN-Bios module setting
#define USE_BIOS_MAINROM			1
#define USE_BIOS_VDP				1
#define USE_BIOS_PSG				1
#define USE_BIOS_SUBROM				1
#define USE_BIOS_DISCROM			1

//-----------------------------------------------------------------------------
// VDP MODULE
//-----------------------------------------------------------------------------

// VRAM address unit
// - VDP_VRAM_ADDR_16 ............. Use 16-bits VRAM address (u16)
// - VDP_VRAM_ADDR_17 ............. Use 17-bits VRAM address (u32)
#define VDP_VRAM_ADDR				VDP_VRAM_ADDR_17

// VDP X/Y units
// - VDP_UNIT_U8 .................. X and Y use 8-bits values
// - VDP_UNIT_X16 ................. X use 16-bits and Y use 8-bits values
// - VDP_UNIT_Y16 ................. X use 8-bits and Y use 16-bits values
// - VDP_UNIT_U16 ................. X and Y use 16-bits values
#define VDP_UNIT					VDP_UNIT_U16

// VDP screen modes (additionnal limitations come from the selected MSX_VERSION)
#define USE_VDP_MODE_T1				0	// MSX1		Screen 0 Width 40
#define USE_VDP_MODE_MC				0	// MSX1		Screen 3
#define USE_VDP_MODE_G1				0	// MSX1		Screen 1
#define USE_VDP_MODE_G2				0	// MSX1		Screen 2
#define USE_VDP_MODE_T2				0	// MSX2		Screen 0 Width 80
#define USE_VDP_MODE_G3				0	// MSX2		Screen 4
#define USE_VDP_MODE_G4				1	// MSX2		Screen 5
#define USE_VDP_MODE_G5				0	// MSX2		Screen 6
#define USE_VDP_MODE_G6				0	// MSX2		Screen 7
#define USE_VDP_MODE_G7				0	// MSX2/2+	Screen 8, 10, 11 & 12

//-----------------------------------------------------------------------------
// INPUT MODULE
//-----------------------------------------------------------------------------

// Input module setting
#define USE_INPUT_JOYSTICK			1
#define USE_INPUT_KEYBOARD			1
#define USE_INPUT_MANAGER			0

//-----------------------------------------------------------------------------
// DRAW MODULE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PRINT MODULE
//-----------------------------------------------------------------------------

// Print module setting
#define USE_PRINT_VALIDATOR			0	// Add validator character code
#define USE_PRINT_VRAM				0	// Allow use of VRAM stored font
#define USE_PRINT_SPRITE			1	// Allow use of Sprite font
#define USE_PRINT_FX_SHADOW			0	// Allow use of text shadow
#define USE_PRINT_FX_OUTLINE		0	// Allow use of text outline
#define USE_PRINT_UNIT				0	// Display integer type (h: hexadecimal, b: binary)
#define PRINT_COLOR_NUM				1	// 1 color per line
// - PRINT_WIDTH_6
// - PRINT_WIDTH_8
// - PRINT_WIDTH_X
#define PRINT_WIDTH					PRINT_WIDTH_8
// - PRINT_HEIGHT_8
// - PRINT_HEIGHT_X
#define PRINT_HEIGHT				PRINT_HEIGHT_8

//-----------------------------------------------------------------------------
// MSXi MODULE
//-----------------------------------------------------------------------------

// MSXi compressor support
#define USE_MSXi_COMP_NONE			1
#define USE_MSXi_COMP_CROP16		1
#define USE_MSXi_COMP_CROP32		1
#define USE_MSXi_COMP_CROP256		1
#define USE_MSXi_COMP_CROPLINE16	1
#define USE_MSXi_COMP_CROPLINE32	1
#define USE_MSXi_COMP_CROPLINE256	1
#define USE_MSXi_COMP_RLE0			1
#define USE_MSXi_COMP_RLE4			1
#define USE_MSXi_COMP_RLE8			1

//-----------------------------------------------------------------------------
// MATH MODULE
//-----------------------------------------------------------------------------

// Random method
// - RANDOM_LFSR_LCG_16 ........... Combined LFSR/LCG (16-bit seeds)
// - RANDOM_LFSR_LCG_32 ........... Combined LFSR/LCG (32-bit seeds)
// - RANDOM_XORSHIFT .............. Xorshift RNGs (16-bit seeds)
#define RANDOM_METHOD				RANDOM_XORSHIFT
