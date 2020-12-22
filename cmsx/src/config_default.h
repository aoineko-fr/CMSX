//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Default defines configuration
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// MSX_VERSION
#ifndef MSX_VERSION
	#define MSX_VERSION		MSX_2
#endif

//-----------------------------------------------------------------------------
// RENDER_MODE
#ifndef RENDER_MODE
	#define RENDER_MODE		RENDER_VDP
#endif

//-----------------------------------------------------------------------------
// BIOS_CALL
#ifndef CALL_MAINROM
	#define CALL_MAINROM	CALL_DIRECT
#endif
#ifndef CALL_SUBROM
	#define CALL_SUBROM		CALL_INTERSLOT
#endif
#ifndef CALL_DISCROM
	#define CALL_DISCROM	CALL_INTERSLOT
#endif

//-----------------------------------------------------------------------------
// RANDOM_METHOD
#ifndef RANDOM_METHOD
	#define RANDOM_METHOD	RANDOM_XORSHIFT
#endif

//-----------------------------------------------------------------------------
// PRINT
#ifndef USE_PRINT_VALIDATOR
	#define USE_PRINT_VALIDATOR	1
#endif
#ifndef USE_PRINT_SHADOW
	#define USE_PRINT_SHADOW	0
#endif
#ifndef USE_PRINT_NO8
	#define USE_PRINT_NO8		1
#endif
#ifndef PRINT_WIDTH
	#define PRINT_WIDTH			PRINT_WIDTH_8
#endif
#ifndef PRINT_HEIGHT
	#define PRINT_HEIGHT		PRINT_HEIGHT_8
#endif
