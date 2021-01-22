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
#if !defined(MSX_VERSION)
	#warning MSX_VERSION is not defined in "cmsx_config.h"! Default value will be used: MSX_2
	#define MSX_VERSION					MSX_2
#endif

//-----------------------------------------------------------------------------
// BIOS MODULE
//-----------------------------------------------------------------------------

// RENDER_MODE
#ifndef RENDER_MODE
	#warning RENDER_MODE is not defined in "cmsx_config.h"! Default value will be used: RENDER_VDP
	#define RENDER_MODE					RENDER_VDP
#endif

// BIOS_CALL
#ifndef CALL_MAINROM
	#warning CALL_MAINROM is not defined in "cmsx_config.h"! Default value will be used: CALL_DIRECT
	#define CALL_MAINROM				CALL_DIRECT
#endif
#ifndef CALL_SUBROM
	#warning CALL_SUBROM is not defined in "cmsx_config.h"! Default value will be used: CALL_INTERSLOT
	#define CALL_SUBROM					CALL_INTERSLOT
#endif
#ifndef CALL_DISCROM
	#warning CALL_DISCROM is not defined in "cmsx_config.h"! Default value will be used: CALL_INTERSLOT
	#define CALL_DISCROM				CALL_INTERSLOT
#endif

//-----------------------------------------------------------------------------
// VDP MODULE
//-----------------------------------------------------------------------------

// VDP_VRAM_ADDR
#ifndef VDP_VRAM_ADDR
	#if (MSX_VERSION == MSX_1)
		#warning VDP_VRAM_ADDR is not defined in "cmsx_config.h"! Default value will be used: VDP_VRAM_ADDR_16
		#define VDP_VRAM_ADDR			VDP_VRAM_ADDR_16
	#else
		#warning VDP_VRAM_ADDR is not defined in "cmsx_config.h"! Default value will be used: VDP_VRAM_ADDR_17
		#define VDP_VRAM_ADDR			VDP_VRAM_ADDR_17
	#endif	
#endif

// VDP_UNIT
#ifndef VDP_UNIT
	#warning VDP_UNIT is not defined in "cmsx_config.h"! Default value will be used: VDP_UNIT_U8
	#define VDP_UNIT					VDP_UNIT_U8
#endif


//-----------------------------------------------------------------------------
// DRAW MODULE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PRINT MODULE
//-----------------------------------------------------------------------------

// USE_PRINT_VALIDATOR
#ifndef USE_PRINT_VALIDATOR
	#warning USE_PRINT_VALIDATOR is not defined in "cmsx_config.h"! Default value will be used: 0
	#define USE_PRINT_VALIDATOR			0
#endif
// USE_PRINT_VRAM
#ifndef USE_PRINT_VRAM
	#warning USE_PRINT_VRAM is not defined in "cmsx_config.h"! Default value will be used: 0
	#define USE_PRINT_VRAM				0
#endif
// USE_PRINT_SPRITE
#ifndef USE_PRINT_SPRITE
	#warning USE_PRINT_SPRITE is not defined in "cmsx_config.h"! Default value will be used: 0
	#define USE_PRINT_SPRITE			0
#endif
// USE_PRINT_FX_SHADOW
#ifndef USE_PRINT_FX_SHADOW
	#warning USE_PRINT_FX_SHADOW is not defined in "cmsx_config.h"! Default value will be used: 0
	#define USE_PRINT_FX_SHADOW			0
#endif
// USE_PRINT_FX_OUTLINE
#ifndef USE_PRINT_FX_OUTLINE
	#warning USE_PRINT_FX_OUTLINE is not defined in "cmsx_config.h"! Default value will be used: 0
	#define USE_PRINT_FX_OUTLINE		0
#endif
// USE_PRINT_UNIT
#ifndef USE_PRINT_UNIT
	#warning USE_PRINT_UNIT is not defined in "cmsx_config.h"! Default value will be used: 0
	#define USE_PRINT_UNIT				0
#endif
// PRINT_WIDTH
#ifndef PRINT_WIDTH
	#warning PRINT_WIDTH is not defined in "cmsx_config.h"! Default value will be used: PRINT_WIDTH_8
	#define PRINT_WIDTH					PRINT_WIDTH_8
#endif
// PRINT_HEIGHT
#ifndef PRINT_HEIGHT
	#warning PRINT_HEIGHT is not defined in "cmsx_config.h"! Default value will be used: PRINT_HEIGHT_8
	#define PRINT_HEIGHT				PRINT_HEIGHT_8
#endif
// PRINT_COLOR
#ifndef PRINT_COLOR_NUM
	#warning PRINT_COLOR_NUM is not defined in "cmsx_config.h"! Default value will be used: 1
	#define PRINT_COLOR_NUM				1
#endif

//-----------------------------------------------------------------------------
// MATH MODULE
//-----------------------------------------------------------------------------

// RANDOM_METHOD
#ifndef RANDOM_METHOD
	#warning RANDOM_METHOD is not defined in "cmsx_config.h"! Default value will be used: RANDOM_XORSHIFT
	#define RANDOM_METHOD			RANDOM_XORSHIFT
#endif

