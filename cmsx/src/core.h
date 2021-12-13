//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
#pragma once
// Library configuration
#include "config_option.h"		// Configuration options
#include "cmsx_config.h"		// Application configuration
#include "config_default.h"		// Default configuration
// #include "config_validate.h" // @todo Implement configuration parameters validation (for ex. no Screen Mode 7 support for MSX 1)

//-----------------------------------------------------------------------------
// Compilation switch
//-----------------------------------------------------------------------------
#pragma disable_warning	59		///< remove "function must return value" warning
#pragma disable_warning	85		///< remove "unreferenced function argument" warning
#pragma disable_warning	218 	///< remove "z80instructionSize() failed to parse line node, assuming 999 bytes" info

//-----------------------------------------------------------------------------
// Basic types
//-----------------------------------------------------------------------------

// Boolean
typedef unsigned char		bool; 	///< 8 bits boolean type
#define true				1		///< Value for "true" boolean
#define false				0		///< Value for "false" boolean

// Integer	
typedef signed char			i8;		///< 8 bits signed integer type
typedef unsigned char		u8;		///< 8 bits unsigned integer type
typedef signed short		i16;	///< 16 bits signed integer type
typedef unsigned short		u16;	///< 16 bits unsigned integer type
typedef signed long			i32;	///< 32 bits signed integer type
typedef unsigned long		u32;	///< 32 bits unsigned integer type
	
// Float	
typedef float				f32;	///< 32 bits float type (IEEE 754)
	
// Character	
typedef unsigned char		c8;		///< 8 bits character type
typedef unsigned short		c16;	///< 16 bits character type
	
// Pointer	
typedef void*				ptr;	///< Pointer type
#define null				0		///< Pointer "null" value

// Functions
typedef void (*callback)(void);	///< Callback default signature

//-----------------------------------------------------------------------------
// Helper macros
//-----------------------------------------------------------------------------

/// Get the number of elements of a static initialized structure
#define numberof(tab)		sizeof(tab) / sizeof(tab[0])
	
#define loop(a, b)			for(u8 a = 0; a < b; ++a)
#define loopx(a)			for(u8 i##__LINE__ = 0; i##__LINE__ < a; ++i##__LINE__)

//-----------------------------------------------------------------------------
// PEEK & POKE
//-----------------------------------------------------------------------------
#define POKE(addr, val)		(*(u8*)(addr) = (val))
#define POKEW(addr, val)	(*(u16*)(addr) = (val))
#define PEEK(addr)			(*(u8*)(addr))
#define PEEKW(addr)			(*(u16*)(addr))

//-----------------------------------------------------------------------------
// Fastcall (__z88dk_fastcall)
//-----------------------------------------------------------------------------
// - 8 bits values are passed in L
// - 16 bits values in HL
// - 32 bits values in DEHL
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
// SDCCCall 0 (__sdcccall(0))
//-----------------------------------------------------------------------------
#define __SDCCCALL0			__sdcccall(0)
#define __SDCCCALL1			__sdcccall(1)
// Return value:
//	8 bits			A
//	16 bits			DE
//	32 bits			HL-DE
// Parameters value:
//	8 bits			A
//	16 bits			HL
//	32 bits			HL-DE
//	8 + 8 bits		A + L
//	8 + 16 bits		A + DE
//	16 + 16 bits	HL + DE

//-----------------------------------------------------------------------------
// Bits handling
//-----------------------------------------------------------------------------
#define BIT_0				0x01	// LSB
#define BIT_1				0x02
#define BIT_2				0x04
#define BIT_3				0x08
#define BIT_4				0x10
#define BIT_5				0x20
#define BIT_6				0x40
#define BIT_7				0x80	// 8 bits MSB
#define BIT_8				0x0100
#define BIT_9				0x0200
#define BIT_10				0x0400
#define BIT_11				0x0800
#define BIT_12				0x1000
#define BIT_13				0x2000
#define BIT_14				0x4000
#define BIT_15				0x8000	// 16 bits MSB

#define BIT_SET(val, bit)	val |=  (1 << bit)  	///< Macro to set a given bit in an integer
#define BIT_CLR(val, bit)	val &= ~(1 << bit)  	///< Macro to clear a given bit in an integer
#define BIT_ISSET(val, bit)	(val & (1 << bit) != 0)	///< Macro to tell if a given bit is set or not

//-----------------------------------------------------------------------------

// __FILE__
// This macro expands to the name of the current input file, in the form of a C string constant.
// The precise name returned is the one that was specified in `#include' or as the input file name argument.
// __LINE__
// This macro expands to the current input line number, in the form of a decimal integer constant. 
// While we call it a predefined macro, it's a pretty strange macro, since its "definition" changes with each new line of source code.
// This and `__FILE__' are useful in generating an error message to report an inconsistency detected by the program; 
// the message can state the source line at which the inconsistency was detected. For example,
// __DATE__
// This macro expands to a string constant that describes the date on which the preprocessor is being run. 
// The string constant contains eleven characters and looks like `"Feb 1 1996"'.
// __TIME__
// This macro expands to a string constant that describes the time at which the preprocessor is being run. 
// The string constant contains eight characters and looks like `"23:59:01"'.