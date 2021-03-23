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

// RANDOM_METHOD
#ifndef RANDOM_METHOD
	#warning RANDOM_METHOD is not defined in "cmsx_config.h"! Default value will be used: RANDOM_XORSHIFT
	#define RANDOM_METHOD				RANDOM_XORSHIFT
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


//-----------------------------------------------------------------------------
//
// V A L I D A T I O N
//
//-----------------------------------------------------------------------------
#if ((MSX_VERSION == MSX_1) && (VDP_VRAM_ADDR == VDP_VRAM_ADDR_17))
	#warning VDP_VRAM_ADDR cant be equal to VDP_VRAM_ADDR_17 when MSX1 machine is selected! VDP_VRAM_ADDR_16 will be select instead
	#undef  VDP_VRAM_ADDR
	#define VDP_VRAM_ADDR				VDP_VRAM_ADDR_16
#endif


#if (MSX_VERSION < MSX_2)
	#if (USE_VDP_MODE_T2)
		#warning USE_VDP_MODE_T2 cant be use when MSX_VERSION < MSX2! USE_VDP_MODE_T2 will be disable
		#undef  USE_VDP_MODE_T2
		#define USE_VDP_MODE_T2			0
	#endif
	#if (USE_VDP_MODE_G3)
		#warning USE_VDP_MODE_G3 cant be use when MSX_VERSION < MSX2! USE_VDP_MODE_T2 will be disable
		#undef  USE_VDP_MODE_G3
		#define USE_VDP_MODE_G3			0
	#endif
	#if (USE_VDP_MODE_G4)
		#warning USE_VDP_MODE_G4 cant be use when MSX_VERSION < MSX2! USE_VDP_MODE_T2 will be disable
		#undef  USE_VDP_MODE_G4
		#define USE_VDP_MODE_G4			0
	#endif
	#if (USE_VDP_MODE_G5)
		#warning USE_VDP_MODE_G5 cant be use when MSX_VERSION < MSX2! USE_VDP_MODE_T2 will be disable
		#undef  USE_VDP_MODE_G5
		#define USE_VDP_MODE_G5			0
	#endif
	#if (USE_VDP_MODE_G6)
		#warning USE_VDP_MODE_G6 cant be use when MSX_VERSION < MSX2! USE_VDP_MODE_T2 will be disable
		#undef  USE_VDP_MODE_G6
		#define USE_VDP_MODE_G6			0
	#endif
	#if (USE_VDP_MODE_G7)
		#warning USE_VDP_MODE_G7 cant be use when MSX_VERSION < MSX2! USE_VDP_MODE_T2 will be disable
		#undef  USE_VDP_MODE_G7
		#define USE_VDP_MODE_G7			0
	#endif
#endif
