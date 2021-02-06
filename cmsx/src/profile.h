//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █
//_____________________________________________________________________________
//
// Helper macros for OpenMSX profiler tool
// Ref: 
//  - https://hg.sr.ht/~grauw/neonlib/browse/tools/profile.tcl
//  - https://github.com/MartinezTorres/openMSX_profiler
#pragma once

#include "core.h"

#define PROFILE_START(a, b, c)
#define PROFILE_END(a, b, c)

/*
//-----------------------------------------------------------------------------
#if (PROFILE_MODE == PROFILE_OM_GRAUW)

	__sfr __at 0x2C g_PortProfileSection;
	__sfr __at 0x2D g_PortProfileFrame;

	#define PROFILE_SECTION_START(id, level) if (l < PROFILE_LEVEL) { __asm		\
		\
	\
	__endasm; }
	
	
	
	#define PROFILE_SECTION_END(id, level)   if (l < PROFILE_LEVEL) { g_PortProfileSection; }

	#define PROFILE_FRAME_START()
	#define PROFILE_FRAME_END()

//-----------------------------------------------------------------------------
#elif (PROFILE_MODE == PROFILE_OM_SALUTTE)

	__sfr __at 0x2C g_PortStartProfile;
	__sfr __at 0x2D g_PortEndProfile;

	const c8* __at 0xF931 g_ProfileMsg;

	#define PROFILE_SECTION_START(id, level) if (l < PROFILE_LEVEL) { g_ProfileMsg = #id; g_PortStartProfile = 0; }
	#define PROFILE_SECTION_END(id, level)   if (l < PROFILE_LEVEL) { g_ProfileMsg = #id; g_PortEndProfile = 0; }

	#define PROFILE_FRAME_START()	{ g_ProfileMsg = "frame"; g_PortStartProfile = 0; }
	#define PROFILE_FRAME_END()		{ g_ProfileMsg = "frame"; g_PortEndProfile = 0; }

//-----------------------------------------------------------------------------
#else // if (PROFILE_MODE == PROFILE_DISABLE)

	#define PROFILE_SECTION_START(id, level)
	#define PROFILE_SECTION_END(id, level)

	#define PROFILE_FRAME_START()
	#define PROFILE_FRAME_END()

#endif
*/