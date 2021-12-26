//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
//
// ROM Mapper Header
//-----------------------------------------------------------------------------
// #pragma once

//-----------------------------------------------------------------------------
// TARGET_TYPE options
#define TYPE_BIN			0 // BASIC binary program
#define TYPE_ROM			1 // ROM program
#define TYPE_DOS			2 // MSX-DOS program

// ROM_MAPPER options
#define ROM_PLAIN			0
#define ROM_ASCII8			1
#define ROM_ASCII16			2
#define ROM_KONAMI			3
#define ROM_KONAMI_SCC		4

// ROM_SIZE options
#define ROM_8K				0	
#define ROM_16K				1	
#define ROM_32K				2	
#define ROM_48K				3	
#define ROM_64K				4	
#define ROM_128K			5	
#define ROM_256K			6	
#define ROM_512K			7	
#define ROM_1M				8	
#define ROM_2M				9	
#define ROM_4M				10	

#define ROM_ISR				(1<<12)	

// ROM type define
//	15	14	13	12	11	10	9	8	7	6	5	4	3	2	1	0
//	0	1	x	ISR	M3	M2	M1	M0	S3	S2	S1	S0	P1	P0	B1	B0	
//  			│	│	│	│	│	│	│	│	│	│	│	└───┴── ROM boot page (address of 'AB' header)
//  			│	│	│	│	│	│	│	│	│	└───┴────────── ROM starting page (first address of the ROM ; can be different from booting page)
//  			│	│	│	│	│	└───┴───┴───┴────────────────── ROM size
//				│	└───┴───┴───┴────────────────────────────────── ROM Mapper Type
//				└────────────────────────────────────────────────── Use custom Interrupt Service Routine (place custom code in page 0)
#define MAKE_ROM(m, s, p, b)	((TYPE_ROM << 14) | ((m) << 8) | ((s) << 4) | ((p) << 2) | (b))
#define MAKE_BASIC(x)			((TYPE_BIN << 14) | (x))
#define MAKE_DOS(x)				((TYPE_DOS << 14) | (x))

// Get current targer parameters
#define GET_TARGET_TYPE(t)		(((t) >> 14) & 0x3)
#define GET_TARGET_ISR(t)		(((t) >> 12) & 0x1)
#define GET_TARGET_MAPPER(t)	(((t) >> 8)  & 0xF)
#define GET_TARGET_SIZE(t)		(((t) >> 4)  & 0xF)
#define GET_TARGET_START(t)		(((t) >> 2)  & 0x3)
#define GET_TARGET_BOOT(t)		( (t)        & 0x3)

#define TARGET_TYPE				GET_TARGET_TYPE(TARGET)
#define ROM_MAPPER				GET_TARGET_MAPPER(TARGET)
#define ROM_SIZE				GET_TARGET_SIZE(TARGET)

// Get current targer parameters
#if (ROM_MAPPER == ROM_ASCII8)
	#define ADDR_BANK_0			0x6000 // 4000h - 5FFFh
	#define ADDR_BANK_1			0x6800 // 6000h - 7FFFh
	#define ADDR_BANK_2			0x7000 // 8000h - 9FFFh
	#define ADDR_BANK_3			0x7800 // A000h - BFFFh
#elif (ROM_MAPPER == ROM_ASCII16)
	#define ADDR_BANK_0			0x6000 // 4000h - 7FFFh
	#define ADDR_BANK_1			0x77FF // 8000h - BFFFh (or 0x7000 ?)
#elif (ROM_MAPPER == ROM_KONAMI)
	//#define ADDR_BANK_0		        / 4000h - 5FFFh
	#define ADDR_BANK_1			0x6000 // 6000h - 7FFFh
	#define ADDR_BANK_2			0x8000 // 8000h - 9FFFh
	#define ADDR_BANK_3			0xA000 // A000h - BFFFh
#elif (ROM_MAPPER == ROM_KONAMI_SCC)
	#define ADDR_BANK_0			0x5000 // 4000h - 5FFFh
	#define ADDR_BANK_1			0x7000 // 6000h - 7FFFh
	#define ADDR_BANK_2			0x9000 // 8000h - 9FFFh
	#define ADDR_BANK_3			0xB000 // A000h - BFFFh
#endif

#if (ROM_MAPPER > ROM_PLAIN)
	#define SET_BANK_SEGMENT(b, s)	(*(unsigned char*)(ADDR_BANK_##b) = (s))
#endif






