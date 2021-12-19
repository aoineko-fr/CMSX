//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
// Default defines configuration
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
//
// D E F A U L T
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// BUILD
//-----------------------------------------------------------------------------

// TARGET
#if !defined(TARGET)
	#warning TARGET is not defined in "cmsx_config.h"! Default value will be used: TARGET_ROM32
	#define TARGET						TARGET_ROM32
#endif

// TARGET_TYPE
#if !defined(TARGET_TYPE)
	#warning TARGET_TYPE is not defined in "cmsx_config.h"! Default value will be used: TARGET_ROM32
	#define TARGET_TYPE					TARGET_TYPE_ROM
#endif

// MSX_VERSION
#if !defined(MSX_VERSION)
	#warning MSX_VERSION is not defined in "cmsx_config.h"! Default value will be used: MSX_2
	#define MSX_VERSION					MSX_2
#endif

//-----------------------------------------------------------------------------
// BIOS MODULE
//-----------------------------------------------------------------------------

// BIOS_CALL
#ifndef BIOS_CALL_MAINROM
	#warning BIOS_CALL_MAINROM is not defined in "cmsx_config.h"! Default value will be used: BIOS_CALL_DIRECT
	#define BIOS_CALL_MAINROM			BIOS_CALL_DIRECT
#endif
#ifndef BIOS_CALL_SUBROM
	#warning BIOS_CALL_SUBROM is not defined in "cmsx_config.h"! Default value will be used: BIOS_CALL_INTERSLOT
	#define BIOS_CALL_SUBROM			BIOS_CALL_INTERSLOT
#endif
#ifndef BIOS_CALL_DISKROM
	#warning BIOS_CALL_DISKROM is not defined in "cmsx_config.h"! Default value will be used: BIOS_CALL_INTERSLOT
	#define BIOS_CALL_DISKROM			BIOS_CALL_INTERSLOT
#endif

//-----------------------------------------------------------------------------
// VDP MODULE
//-----------------------------------------------------------------------------

// VDP_VRAM_ADDR
#ifndef VDP_VRAM_ADDR
	#if (MSX_VERSION == MSX_1)
		#warning VDP_VRAM_ADDR is not defined in "cmsx_config.h"! Default value will be used: VDP_VRAM_ADDR_14
		#define VDP_VRAM_ADDR			VDP_VRAM_ADDR_14
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
// PRINT_SKIP_SPACE
#ifndef PRINT_SKIP_SPACE
	#warning PRINT_SKIP_SPACE is not defined in "cmsx_config.h"! Default value will be used: 0
	#define PRINT_SKIP_SPACE			0
#endif
// USE_PRINT_GRAPH
#ifndef USE_PRINT_GRAPH
	#warning USE_PRINT_GRAPH is not defined in "cmsx_config.h"! Default value will be used: 1
	#define USE_PRINT_GRAPH				1
#endif


//-----------------------------------------------------------------------------
// GAME MODULE
//-----------------------------------------------------------------------------

// USE_GAME_STATE
#ifndef USE_GAME_STATE
	#warning USE_GAME_STATE is not defined in "cmsx_config.h"! Default value will be used: 1
	#define USE_GAME_STATE				1
#endif

// USE_GAME_VSYNC
#ifndef USE_GAME_VSYNC
	#warning USE_GAME_VSYNC is not defined in "cmsx_config.h"! Default value will be used: 1
	#define USE_GAME_VSYNC				1
#endif

// USE_GAME_LOOP
#ifndef USE_GAME_LOOP
	#warning USE_GAME_LOOP is not defined in "cmsx_config.h"! Default value will be used: 1
	#define USE_GAME_LOOP				1
#endif

//-----------------------------------------------------------------------------
// MATH MODULE
//-----------------------------------------------------------------------------

// RANDOM_8_METHOD
#ifndef RANDOM_8_METHOD
	#warning RANDOM_8_METHOD is not defined in "cmsx_config.h"! Default value will be used: RANDOM_8_ION
	#define RANDOM_8_METHOD				RANDOM_8_ION
#endif

// RANDOM_16_METHOD
#ifndef RANDOM_16_METHOD
	#warning RANDOM_16_METHOD is not defined in "cmsx_config.h"! Default value will be used: RANDOM_16_XORSHIFT
	#define RANDOM_16_METHOD			RANDOM_16_XORSHIFT
#endif

//-----------------------------------------------------------------------------
// MISC
//-----------------------------------------------------------------------------

// PROFILE_MODE
#if !defined(PROFILE_MODE)
	#warning PROFILE_MODE is not defined in "cmsx_config.h"! Default value will be used: PROFILE_DISABLE
	#define PROFILE_MODE				PROFILE_DISABLE
#endif

// PROFILE_LEVEL
#if !defined(PROFILE_LEVEL)
	#warning PROFILE_LEVEL is not defined in "cmsx_config.h"! Default value will be used: 10
	#define PROFILE_LEVEL				10
#endif

// RANDOM
#if !defined(RANDOM_METHOD)
#define RANDOM_METHOD RANDOM_XORSHIFT
#endif

//-----------------------------------------------------------------------------
//
// V A L I D A T I O N
//
//-----------------------------------------------------------------------------

// Check MSX 1 dependency
#if (MSX_VERSION == MSX_1)

	// Check for MSX1 supported 64K max VRAM size
	#if (VDP_VRAM_ADDR == VDP_VRAM_ADDR_17)
		#warning Cant use VDP_VRAM_ADDR_17 with MSX1! VDP_VRAM_ADDR_14 will be used instead
		#undef  VDP_VRAM_ADDR
		#define VDP_VRAM_ADDR			VDP_VRAM_ADDR_14
	#endif

	// Check for MSX1 supported screen mode
	#if (USE_VDP_MODE_T2)
		#warning Cant use USE_VDP_MODE_T2 with MSX1! Option will be disable
		#undef  USE_VDP_MODE_T2
		#define USE_VDP_MODE_T2			0
	#endif
	#if (USE_VDP_MODE_G3)
		#warning Cant use USE_VDP_MODE_G3 with MSX1! Option will be disable
		#undef  USE_VDP_MODE_G3
		#define USE_VDP_MODE_G3			0
	#endif
	#if (USE_VDP_MODE_G4)
		#warning Cant use USE_VDP_MODE_G4 with MSX1! Option will be disable
		#undef  USE_VDP_MODE_G4
		#define USE_VDP_MODE_G4			0
	#endif
	#if (USE_VDP_MODE_G5)
		#warning Cant use USE_VDP_MODE_G5 with MSX1! Option will be disable
		#undef  USE_VDP_MODE_G5
		#define USE_VDP_MODE_G5			0
	#endif
	#if (USE_VDP_MODE_G6)
		#warning Cant use USE_VDP_MODE_G6 with MSX1! Option will be disable
		#undef  USE_VDP_MODE_G6
		#define USE_VDP_MODE_G6			0
	#endif
	#if (USE_VDP_MODE_G7)
		#warning Cant use USE_VDP_MODE_G7 with MSX1! Option will be disable
		#undef  USE_VDP_MODE_G7
		#define USE_VDP_MODE_G7			0
	#endif
	#if (USE_PRINT_BITMAP)
		#warning Cant use USE_PRINT_BITMAP with MSX1! Option will be disable
		#undef  USE_PRINT_BITMAP
		#define USE_PRINT_BITMAP		0
	#endif
#endif // (MSX_VERSION == MSX_1)

// Check USE_PRINT_BITMAP dependency
#if(!USE_PRINT_BITMAP)
	#if(USE_PRINT_VRAM)
		#warning USE_PRINT_VRAM need USE_PRINT_BITMAP to be set! Option will be disable
		#undef  USE_PRINT_VRAM
		#define USE_PRINT_VRAM			0
	#endif

	#if(USE_PRINT_FX_SHADOW)
		#warning USE_PRINT_FX_SHADOW need USE_PRINT_BITMAP to be set! Option will be disable
		#undef  USE_PRINT_FX_SHADOW
		#define USE_PRINT_FX_SHADOW			0
	#endif

	#if(USE_PRINT_FX_OUTLINE)
		#warning USE_PRINT_FX_OUTLINE need USE_PRINT_BITMAP to be set! Option will be disable
		#undef  USE_PRINT_FX_OUTLINE
		#define USE_PRINT_FX_OUTLINE			0
	#endif
#endif // (!USE_PRINT_BITMAP)
