//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
#pragma once
#include "core.h"

//-----------------------------------------------------------------------------
// Helper macros
//-----------------------------------------------------------------------------

/// Get absolute value of a usigned 8-bits integer
#define Abs8(i)				(((u8)(i) & 0x80) ? ~((u8)(i) - 1) : (i))
/// Get absolute value of a usigned 16-bits integer
#define Abs16(i)			(((u16)(i) & 0x8000) ? ~((u16)(i) - 1) : (i))
/// Get absolute value of a usigned 32-bits integer
#define Abs32(i)			(((u32)(i) & 0x80000000) ? ~((u32)(i) - 1) : (i))

/// Invert sign of a signed integer
#define Invert(a)			((^a)++)

/// Merge two 4 bits value into a 8 bits integer
#define Merge44(a, b)		(u8)(((a) & 0x0F) << 4 | ((b) & 0x0F))
/// Merge two 8 bits value into a 16 bits integer
#define Merge88(a, b)		(u16)((u8)(a) << 8 | (u8)(b))
/// Get the power-of-2 modulo of a integer (ie. "ModuloPow2(100, 32)")
#define ModuloPow2(a, b)	((a) & ((b) - 1))

/// Clamp a 8-bits value into a interval 
#define Clamp8(a, b, c)		((i8)(a) < (i8)(b)) ? (b) : ((i8)(a) > (i8)(c)) ? (c) : (a)
/// Clamp a 16-bits value into a interval 
#define Clamp16(a, b, c)	((i16)(a) < (i16)(b)) ? (b) : ((i16)(a) > (i16)(c)) ? (c) : (a)

/// 
#define Max(a, b)				((a) > (b)) ? (a) : (b)
/// 
#define Min(a, b)				((a) > (b)) ? (b) : (a)

//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------

/// Clamp a value into a interval 
i8 Math_Clamp(i8 val, i8 min, i8 max);

//-----------------------------------------------------------------------------
// Quick math routines
//-----------------------------------------------------------------------------

/// 8-bits fast 10 times division 
i8 Math_Div10(i8 val) __FASTCALL;

/// 16-bits fast 10 times division 
i16 Math_Div10_16b(i16 val) __FASTCALL;

/// 8-bits fast modulo-10 
u8 Math_Mod10(u8 val) __FASTCALL;

/// 16-bits fast modulo-10 
u8 Math_Mod10_16b(u16 val) __FASTCALL;

/// Bits flip routine
u8 Math_Flip(u8 val) __FASTCALL;

/// Bits flip routine
u16 Math_Flip_16b(u16 val) __FASTCALL;

//-----------------------------------------------------------------------------
// Random routines
//-----------------------------------------------------------------------------

// R register value
#if (RANDOM_8_METHOD == RANDOM_8_REGISTER)
	#define RANDOM_8_NAME		"R Register"
// Ion Random
#elif (RANDOM_8_METHOD == RANDOM_8_ION)
	#define RANDOM_8_NAME		"Ion Random"
// Memory Peek from R
#elif (RANDOM_8_METHOD == RANDOM_8_MEMORY)
	#define RANDOM_8_NAME		"Memory Peek from R"
#endif

/// Initialize random generator seed
void Math_SetRandomSeed8(u8 seed);

/// Generates 8-bit pseudorandom numbers
u8 Math_GetRandom8();


// Linear congruential generator
#if (RANDOM_16_METHOD == RANDOM_16_LINEAR)
	#define RANDOM_16_LINEAR_A	3			// 3		75		129
	#define RANDOM_16_LINEAR_C	0x8721		// 0x8721	74		0x4321
	#define RANDOM_16_NAME		"Linear congruential"
// 16-bit xorshift pseudorandom number generator
#elif (RANDOM_16_METHOD == RANDOM_16_XORSHIFT)
	#define RANDOM_16_NAME 		"XOR Shift"
// Combination of a 16-bit Linear Feedback Shift Register (LFSR) and a 16-bit LCG
#elif (RANDOM_16_METHOD == RANDOM_16_LFSR_LCG)
	#define RANDOM_16_NAME 		"Combined LFSR/LCG"
#endif

/// Initialize random generator seed
void Math_SetRandomSeed16(u16 seed);

/// Generates 16-bit pseudorandom numbers
u16 Math_GetRandom16() __FASTCALL;