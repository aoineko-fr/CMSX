//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "config.h"
#include "cmsx_config.h"
#include "config_default.h"

//-----------------------------------------------------------------------------
// Compilation switch
//-----------------------------------------------------------------------------
#pragma disable_warning 59	///< remove "function must return value" warning
#pragma disable_warning 85	///< remove "unreferenced function argument" warning
#pragma disable_warning 218 ///< remove "z80instructionSize() failed to parse line node, assuming 999 bytes" info

//-----------------------------------------------------------------------------
// Basic types
//-----------------------------------------------------------------------------

// Boolean
typedef unsigned char bool; ///< 8 bits boolean type
#define true  0x01			///< Value for "true" boolean
#define false 0x00			///< Value for "false" boolean

// Integer
typedef char i8;			///< 8 bits signed integer type
typedef unsigned char u8;	///< 8 bits unsigned integer type
typedef short i16;			///< 16 bits signed integer type
typedef unsigned short u16;	///< 16 bits unsigned integer type
typedef long i32;			///< 32 bits signed integer type
typedef unsigned long u32;	///< 32 bits unsigned integer type

// Float
typedef float f32;			///< 32 bits float type (IEEE 754)

// Character
typedef unsigned char c8;	///< 8 bits character type
typedef unsigned short c16;	///< 16 bits character type

// Pointer
typedef void* ptr;			///< Pointer type
#define null 0				///< Pointer "null" value

// Functions
typedef void (*callback)(void); ///< Callback default signature

//-----------------------------------------------------------------------------
// Helper macros
//-----------------------------------------------------------------------------

/// Get the nimmber of element of a static initialized structure
#define numberof(tab)		sizeof(tab) / sizeof(tab[0])

//-----------------------------------------------------------------------------
// Helper inline functions

/// Direct call a routine at a given address (generate ASM code: "call XXXX")
inline void Call(u16 addr) { ((void(*)(void))(addr))(); }

/// Enable interruption
inline void EnableInterrupt() { __asm__("ei"); }

/// Disable interruption
inline void DisableInterrupt() { __asm__("di"); }

//-----------------------------------------------------------------------------
// Fastcall (__z88dk_fastcall)
//-----------------------------------------------------------------------------
// - 8-bit values are passed in L
// - 16-bit values in HL
// - 32-bit values in DEHL
#define __FASTCALL			__z88dk_fastcall
// For 8 bits parameter
#define FC8(a)				(u8)(a)					// L
// For 16 bits parameters
#define FC16(a)				(u16)(a)				// HL
#define FC88(a, b)			(u16)((a << 8) + b)		// H, L
// For 32 bits parameters
#define FC32(a)				(u32)(a)															// DEHL
#define FC1616(a, b)		(u32)(((u32)(a) << 16) + b)											// DE, HL
#define FC1688(a, b, c)		(u32)(((u32)(a) << 16) + ((u16)(b) << 8) + c)						// DE, H, L
#define FC8816(a, b, c)		(u32)(((u32)(a) << 24) + ((u32)(b) << 16) + c)						// D, E, HL
#define FC8888(a, b, c, d)	(u32)(((u32)(a) << 24) + ((u32)(b) << 16) + ((u16)(c) << 8) + d)	// D, E, H, L

//-----------------------------------------------------------------------------
// Bits handling
//-----------------------------------------------------------------------------
#define BIT_0 0x01  // LSB
#define BIT_1 0x02
#define BIT_2 0x04
#define BIT_3 0x08
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80  // MSB

#define BIT_SET(val, bit) val |=  (1 << bit)  // Sets a bit
#define BIT_CLR(val, bit) val &= ~(1 << bit)  // Clears a bit
