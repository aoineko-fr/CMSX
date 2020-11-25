//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Library configuration
//-----------------------------------------------------------------------------
#pragma once

// Target
// - TARGET_BIN........ BASIC binary program
// - TARGET_ROM16...... 16KB ROM in page 1
// - TARGET_ROM16P2.... 16KB ROM in page 2
// - TARGET_ROM32...... 32KB ROM in page 1&2
// - TARGET_ROM48...... 48KB ROM in page 0-2
// - TARGET_DOS........ MSX-DOS program
// - TARGET_DOSARG..... MSX-DOS program (using command line arguments)
// TARGET is defined by the build tool

// Target type
// - TARGET_TYPE_BIN... BASIC binary program
// - TARGET_TYPE_ROM... ROM
// - TARGET_TYPE_DOS... MSX-DOS program
// TARGET_TYPE is defined by the build tool

// MSX version
// - MSX_1............. MSX
// - MSX_2............. MSX 2
// - MSX_2Plus......... MSX 2+
// - MSX_TurboR........ MSX Turbo-R
#define MSX_VERSION		MSX_2

// Render mode
// - RENDER_BIOS....... Use BIOS routines
// - RENDER_VDP........ Use direct VDP registers
#define RENDER_MODE		RENDER_BIOS

// Default bios access
// - CALL_DIRECT....... Use direct access to Bios routines (ROM slot must be selected in corresponding page)
// - CALL_INTERSLOT.... Use inter-slot access to Bios routines (through CALSLT routine)
#if (TARGET_TYPE == TARGET_TYPE_DOS)
	#define CALL_MAINROM	CALL_INTERSLOT
	#define CALL_SUBROM		CALL_INTERSLOT
	#define CALL_DISCROM	CALL_INTERSLOT
#else
	#define CALL_MAINROM	CALL_DIRECT
	#define CALL_SUBROM		CALL_INTERSLOT
	#define CALL_DISCROM	CALL_INTERSLOT
#endif

// Include routines
#define USE_BIOS_VDP	1
#define USE_BIOS_PSG	1

// MSXi compressor support
#define USE_MSXi_COMP_NONE			0
#define USE_MSXi_COMP_CROP16		1
#define USE_MSXi_COMP_CROP32		0
#define USE_MSXi_COMP_CROP256		0
#define USE_MSXi_COMP_CROPLINE16	1
#define USE_MSXi_COMP_CROPLINE32	0
#define USE_MSXi_COMP_CROPLINE256	0
#define USE_MSXi_COMP_RLE0			1
#define USE_MSXi_COMP_RLE4			0
#define USE_MSXi_COMP_RLE8			0
