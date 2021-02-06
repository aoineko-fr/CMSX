//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
// References:
// - http://map.grauw.nl/sources/external/z80bits.html#4
// - http://z80-heaven.wikidot.com/math
// - http://z80-heaven.wikidot.com/advanced-math
// - https://wikiti.brandonw.net/index.php?title=Category:Z80_Routines:Math
//-----------------------------------------------------------------------------
#include "core.h"
#include "math.h"

/// Clamp a value into a interval 
i8 Math_Clamp(i8 val, i8 min, i8 max)
{
	if(val < min)
		return min;
	if(val > max)
		return max;
	return val;
}

//-----------------------------------------------------------------------------
// DIVISION
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// 8-bits fast 10 times division 
i8 Math_Div10(i8 val) __FASTCALL
{
	val;
	__asm
//		ld		l, val		// fast call
		ld		d, #0
		ld		h, d
		add		hl, hl
		add		hl, de
		add		hl, hl
		add		hl, hl
		add		hl, de
		add		hl, hl
		ld		l, h
	__endasm;
}

//-----------------------------------------------------------------------------
/// 16-bits fast 10 times division 
i16 Math_Div10_16b(i16 val) __FASTCALL
{
	val;
	__asm
//		ld		hl, val		// fast call
		ld		bc, #0x0D0A
		xor		a
		add		hl, hl
		rla
		add		hl, hl
		rla
		add		hl, hl
		rla
		add		hl, hl
		rla
		cp		c
	label_0:
		jr		c, label_0 + #4
		sub		c
		inc		l
	label_1:
		djnz	label_1 - #7
	__endasm;
}

//-----------------------------------------------------------------------------
// MODULO
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// 18-bits fast modulo-10 
// Inputs:		A	8-bit unsigned integer
// Outputs:		A	HL mod 10
//				Z	Flag is set if divisible by 10
// 20 bytes, 83 cycles
u8 Math_Mod10(u8 val) __FASTCALL
{
	val;
	__asm
//		ld		l, val		// fast call
		ld		a, l		// add bytes
		ld		h, a		// add nibbles
		rrca
		rrca
		rrca
		rrca
		add		a, h
		adc		a, #0		// n mod 15 (+1) in both nibbles
		daa
		ld		l, a
		sub		h			// test if quotient is even or odd
		rra
		sbc		a, a
		and		#5
		add		a, l
		daa
		and		#0x0F
		ld		l, a
	__endasm;
}

//-----------------------------------------------------------------------------
/// 16-bits fast modulo-10 
// Inputs:		HL	16-bit unsigned integer
// Outputs:		A	HL mod 10
//				Z	Flag is set if divisible by 10
// 24 bytes,  98cc
u8 Math_Mod10_16b(u16 val) __FASTCALL
{
	val;
	__asm
//		ld		hl, val		// fast call
		ld		a, h		// add bytes
		add		a, l
		adc		a, #0		// n mod 255 (+1)
		ld		h, a		// add nibbles
		rrca
		rrca
		rrca
		rrca
		add		a, h
		adc		a, #0		// n mod 15 (+1) in both nibbles
		daa
		ld		h, a
		sub		l			// test if quotient is even or odd
		rra
		sbc		a, a
		and		#5
		add		a, h
		daa
		and		#0x0F
		ld		l, a
	__endasm;
}

//-----------------------------------------------------------------------------
// BITS OPERATION
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// 8-bits fast bits flip 
// Inputs:		L		8-bits value
// Outputs:		L		Bits flipped value
// Author:		John Metcalf (http://www.retroprogramming.com/2014/01/fast-z80-bit-reversal.html)
u8 Math_Flip(u8 val) __FASTCALL
{
	val;
	__asm
//		ld		l, val		// fast call
		ld		a, l		// a = 76543210
#if 0	// Methode 2: 8 bytes / 206 cycles
		ld		b, #8
		ld		l, a
	FlipLoop:
		rl		l
		rra
		djnz FlipLoop
#else	// Methode 2: 17 bytes / 66 cycles
		rlca
		rlca				// a = 54321076
		xor		l
		and		#0xAA
		xor		l			// a = 56341270
		ld		l, a
		rlca
		rlca
		rlca				// a = 41270563
		rrc 	l			// l = 05634127
		xor 	l
		and 	#0x66
		xor 	l			// a = 01234567
#endif
		ld		l, a
	__endasm;
}

//-----------------------------------------------------------------------------
/// 16-bits bits flip 
u16 Math_Flip_16b(u16 val) __FASTCALL
{
	// register u8 a = val >> 8;
	// register u8 b = val & 0x00FF;
	// a = Math_Flip(a);
	// b = Math_Flip(b);
	// return (b << 8) + a;
	
	val;
	__asm
//		ld		hl, val		// fast call
		ld		a, h
		call	_Math_Flip	// flip high bits
		ld		b, a
		ld		a, l
		call	_Math_Flip	// flip low bits
		ld		h, a		// flip high/low
		ld		l, b
	__endasm;
}

//-----------------------------------------------------------------------------
// RANDOM
//-----------------------------------------------------------------------------

// RANDOM_LFSR_LCG_16		// Combined LFSR/LCG (16-bit seeds)
// RANDOM_LFSR_LCG_32		// Combined LFSR/LCG (32-bit seeds)
// RANDOM_XORSHIFT			// Xorshift RNGs (16-bit seeds)
#if !defined(RANDOM_METHOD)
#define RANDOM_METHOD RANDOM_XORSHIFT
#endif

#if (RANDOM_METHOD == RANDOM_LFSR_LCG_16)

u16 g_RandomSeed1 = 1;
u16 g_RandomSeed2 = 2;
//-----------------------------------------------------------------------------
/// Generates 16-bit pseudorandom number
/// Combined LFSR/LCG (16-bit seeds)
// Inputs:		(seed1) contains a 16-bit seed value
//				(seed2) contains a NON-ZERO 16-bit seed value
// Outputs:		HL is the result
//				BC is the result of the LCG, so not that great of quality
//				DE is preserved
// cycle: 4,294,901,760 (almost 4.3 billion)
// 26 bytes, 160 cycles
u16 Math_GetRandom()
{
	__asm
		ld		hl,(_g_RandomSeed1)
		ld		b, h
		ld		c, l
		add		hl, hl
		add		hl, hl
		inc		l
		add		hl, bc
		ld		(_g_RandomSeed1), hl
		ld		hl, (_g_RandomSeed2)
		add		hl, hl
		sbc		a, a
		and		#0b00101101
		xor		l
		ld		l, a
		ld		(_g_RandomSeed2), hl
		add		hl, bc
	__endasm;
}

#elif (RANDOM_METHOD == RANDOM_LFSR_LCG_32)

u16 g_RandomSeed1_0 = 1;
u16 g_RandomSeed1_1 = 2;
u16 g_RandomSeed2_0 = 3;
u16 g_RandomSeed2_1 = 4;
//-----------------------------------------------------------------------------
/// Generates 16-bit pseudorandom number
/// Combined LFSR/LCG (32-bit seeds)
// Inputs:		(seed1_0) holds the lower 16 bits of the first seed
//				(seed1_1) holds the upper 16 bits of the first seed
//				(seed2_0) holds the lower 16 bits of the second seed
//				(seed2_1) holds the upper 16 bits of the second seed
//				**NOTE: seed2 must be non-zero
// Outputs:		HL is the result
//				BC,DE can be used as lower quality values, but are not independent of HL.
// LFSR taps: 0,2,6,7  = 11000101
// 291 cycles
u16 Math_GetRandom()
{
	__asm
		ld		hl, #12345
		ld		de, #6789
		ld		b, h
		ld		c, l
		add		hl, hl
		rl		e
		rl		d
		add		hl, hl
		rl		e
		rl		d
		inc		l
		add		hl, bc
		ld		(_g_RandomSeed1_0), hl
		ld		hl, (_g_RandomSeed1_1)
		adc		hl, de
		ld		(_g_RandomSeed1_1), hl
		ex		de, hl
		ld		hl, #9876
		ld		bc, #54321
		add		hl, hl
		rl		c
		rl		b
		ld		(_g_RandomSeed2_1), bc
		sbc		a, a
		and		#0b11000101
		xor		l
		ld		l, a
		ld		(_g_RandomSeed2_0), hl
		ex		de, hl
		add		hl, bc
	__endasm;
}

#elif (RANDOM_METHOD == RANDOM_XORSHIFT)

u16 g_RandomSeed = 1;
//-----------------------------------------------------------------------------
/// Generates 16-bit pseudorandom number
/// 16-bit xorshift pseudorandom number generator by John Metcalf
// Outputs:		HL	Pseudorandom number
// Using the xor-shift method:
//	hl ^= hl << 7
//	hl ^= hl >> 9
//	hl ^= hl << 8
// Some alternative shift triplets which also perform well are:
//	6, 7, 13; 7, 9, 13; 9, 7, 13.
// 20 bytes, 86 cycles
u16 Math_GetRandom()
{
	__asm
	xrnd:
		ld		hl, (_g_RandomSeed)
		ld		a, h
		rra
		ld		a, l
		rra
		xor		h
		ld		h, a
		ld		a, l
		rra
		ld		a, h
		rra
		xor		l
		ld		l, a
		xor		h
		ld		h, a
		ld		(_g_RandomSeed), hl
	__endasm;
}

#endif
