//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// PSG module header file
//
// References:
//  - AY-3-8910 / 8912 Programmable Sound Generator Data Manual
//  - https://www.angelfire.com/art2/unicorndreams/msx/RR-PSG.html
//-----------------------------------------------------------------------------
#include "psg.h"

u8 g_PSG_Regs[16];

//-----------------------------------------------------------------------------
/// Send data to PSG registers #1 to #13
/// @note					Must be executed on each V-Blank interruption
void PSG_UpdateRegisters()
{
__asm

	// Update mixer register wanted value with I/O 2-bits from the current mixer register value
	ld		A, (#_g_PSG_Regs + PSG_REG_MIXER)
	and		#0b00111111
	ld		B, A
	ld		A, #PSG_REG_MIXER
	out		(#PSG_PORT_REG), A
	in		A, (#PSG_PORT_READ)  
	and		#0b11000000
	or		B
	ld		(#_g_PSG_Regs + PSG_REG_MIXER), A

	// Registers value copy loop (528 T-States)
	ld		HL, #_g_PSG_Regs	// 11	Data to copy to PSG registers
	ld		C, #PSG_PORT_WRITE		// 8	Setup outi register
	xor		A						// 5	Initialize register number
	// R#0-12
	.rept 13
		out		(PSG_PORT_REG), A	// 12	port_reg <- reg_num
		outi						// 18	port_data <- data[i++]
		inc		A					// 5
	.endm
	// R#13
	out		(PSG_PORT_REG), A		// 12	port_reg <- reg_num
	ld		A, (HL)					// 8
	and		A						// 5
	ret		M						// 12	don't copy R#13 if value is negative
	out		(PSG_PORT_WRITE), A		// 12	port_data <- data[i]

__endasm;
}