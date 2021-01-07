//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Default defines configuration
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// BUILD
//-----------------------------------------------------------------------------

// MSX_VERSION
#ifndef MSX_VERSION
	#define MSX_VERSION			MSX_2
#endif

//-----------------------------------------------------------------------------
// BIOS MODULE
//-----------------------------------------------------------------------------

// RENDER_MODE
#ifndef RENDER_MODE
	#define RENDER_MODE			RENDER_VDP
#endif

// BIOS_CALL
#ifndef CALL_MAINROM
	#define CALL_MAINROM		CALL_DIRECT
#endif
#ifndef CALL_SUBROM
	#define CALL_SUBROM			CALL_INTERSLOT
#endif
#ifndef CALL_DISCROM
	#define CALL_DISCROM		CALL_INTERSLOT
#endif

//-----------------------------------------------------------------------------
// DRAW MODULE
//-----------------------------------------------------------------------------

// DRAW_UNIT
#ifndef DRAW_UNIT
	#define DRAW_UNIT			DRAW_UNIT_U8
#endif

//-----------------------------------------------------------------------------
// PRINT MODULE
//-----------------------------------------------------------------------------

// USE_PRINT_VALIDATOR
#ifndef USE_PRINT_VALIDATOR
	#define USE_PRINT_VALIDATOR	1
#endif
// USE_PRINT_SHADOW
#ifndef USE_PRINT_SHADOW
	#define USE_PRINT_SHADOW	0
#endif
// USE_PRINT_NO8
#ifndef USE_PRINT_NO8
	#define USE_PRINT_NO8		1
#endif
// PRINT_WIDTH
#ifndef PRINT_WIDTH
	#define PRINT_WIDTH			PRINT_WIDTH_8
#endif
// PRINT_HEIGHT
#ifndef PRINT_HEIGHT
	#define PRINT_HEIGHT		PRINT_HEIGHT_8
#endif

//-----------------------------------------------------------------------------
// MATH MODULE
//-----------------------------------------------------------------------------

// RANDOM_METHOD
#ifndef RANDOM_METHOD
	#define RANDOM_METHOD		RANDOM_XORSHIFT
#endif

