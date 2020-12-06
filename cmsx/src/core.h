//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "config.h"
#include "cmsx_config.h"

//-----------------------------------------------------------------------------
// Compilation switch
#pragma disable_warning 85	// remove "unreferenced function argument" warning
#pragma disable_warning 59	// remove "function must return value" warning
#pragma disable_warning 218 // remove "z80instructionSize() failed to parse line node, assuming 999 bytes" info

//-----------------------------------------------------------------------------
// Boolean
typedef unsigned char bool; // 8 bits boolean
#define true  0x01
#define false 0x00

// Integer
typedef char i8;			// 8 bits signed integer
typedef unsigned char u8;	// 8 bits unsigned integer
typedef short i16;			// 16 bits signed integer
typedef unsigned short u16;	// 16 bits unsigned integer
typedef long i32;			// 32 bits signed integer
typedef unsigned long u32;	// 32 bits unsigned integer

// Float
typedef float f32;			// 32 bits float (IEEE 754)

// Character
typedef unsigned char c8;	// 8 bits character
typedef unsigned short c16;	// 16 bits character

// Pointer
typedef void* ptr;
#define null 0

// Functions
typedef void (*callback)(void);

// 

//-----------------------------------------------------------------------------
// Helper macros
#define numberof(tab)		sizeof(tab) / sizeof(tab[0])

#define Abs8(i)				(((u8)(i) & 0x80) ? ~((u8)(i) - 1) : (i))
#define Abs16(i)			(((u16)(i) & 0x8000) ? ~((u16)(i) - 1) : (i))
#define Abs32(i)			(((u32)(i) & 0x80000000) ? ~((u32)(i) - 1) : (i))

#define Posi(a)				(a)
#define Nega(a)				((^a)++)

#define Merge4(a, b)		(((a) & 0xF) << 4 | ((b) & 0xF))
#define ModuloPow2(a, b)	((a) & ((b) - 1))

//-----------------------------------------------------------------------------
// Helper inline functions

// Direct call a routine at a given address (generate ASM code: "call XXXX")
inline void Call(u16 addr) { ((void(*)(void))(addr))(); }

// Enable interruption
inline void EnableInterrupt() { __asm__("ei"); }

// Disable interruption
inline void DisableInterrupt() { __asm__("di"); }

//-----------------------------------------------------------------------------
// Fastcall (__z88dk_fastcall)
// - 8-bit values are passed in L
// - 16-bit values in HL
// - 32-bit values in DEHL
#define __FASTCALL __z88dk_fastcall
// For 8 bits parameter
#define FC8(a)				(u8)(a)										// L
// For 16 bits parameters
#define FC16(a)				(u16)(a)									// HL
#define FC88(a, b)			(u16)((a << 8) + b)							// H, L
// For 32 bits parameters
#define FC32(a)				(u32)(a)									// DEHL
#define FC1616(a, b)		(u32)((a << 16) + b)						// DE, HL
#define FC1688(a, b, c)		(u32)((a << 16) + (b << 8) + c)				// DE, H, L
#define FC8816(a, b, c)		(u32)((a << 24) + (b << 16) + c)			// D, E, HL
#define FC8888(a, b, c, d)	(u32)((a << 24) + (b << 16) + (c << 8) + d)	// D, E, H, L
































