//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
/**
 * @file Features to manage the VDP - Source code
 *
 * References:
 * - V9938 Technical Data Book Programmer's Guide
 * - https://www.msx.org/wiki/Category:VDP_Registers
 * - http://map.grauw.nl/articles/
 */
#include "vdp.h"
#include "ports.h"

//-----------------------------------------------------------------------------
// VDP Registers

u8 g_VDP_REGSAV[28];
u8 g_VDP_STASAV[10];
VDP_Command g_VDP_Command;

//-----------------------------------------------------------------------------
// VDP Registers Flags

#define F_VDP_REG	0x80	// VDP register write port (bit 7=1 in second write)
#define F_VDP_VRAM	0x00	// VRAM address register (bit 7=0 in second write)
#define F_VDP_WRIT	0x40	// bit 6: read/write access (0=read)
#define F_VDP_READ	0x00	//

#define VDP_REG(_r) (F_VDP_REG | _r)

//-----------------------------------------------------------------------------
//
//   M S X   1   F U N C T I O N S
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
static const u8 modeT1[] = 
{
	0x00, VDP_REG(0), // Text Mode, No External Video
	0xD0, VDP_REG(1), // 16K, Enable Disp., Disable Int.
	0x02, VDP_REG(2), // Address of Name Table in VRAM = 0800h
	//    VDP_REG(3), // (not used) Color is defined in Reg. 7
	0x00, VDP_REG(4), // Address of Patern Table in VRAM = 0000h
	//    VDP_REG(5), // (not used)
	//    VDP_REG(6), // (not used)
	0xF5, VDP_REG(7), // White Text on Light Blue Background
};
void VDP_SetModeText1()
{
	// Backup vdp register @todo Can be optimized
	for(i8 i = 0; i < 10; i += 2)
		g_VDP_REGSAV[modeT1[i + 1] & 0x7F] = modeT1[i];

	__asm
		ld		hl, #_modeT1
		ld		bc, #(10 * 0x100 + P_VDP_ADDR) // [num|0x99]
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
static const u8 modeMC[] = 
{
	0x00, VDP_REG(0), // Multicolor Mode, No External Video
	0xCB, VDP_REG(1), // 16K, Enable Disp., Disable Int., 16x16 Sprites, Mag. On
	0x05, VDP_REG(2), // Address of Name Table in VRAM = 1400h
	//    VDP_REG(3), // (not used)
	0x01, VDP_REG(4), // Address of Patern Table in VRAM = 0800h
	0x20, VDP_REG(5), // Address of Sprite Attribute Table in VRAM = 1000h
	0x00, VDP_REG(6), // Address of Sprite Pattern Table in VRAM = 0000h
	0x04, VDP_REG(7), // Backdrop Color = Dark Blue
};
void VDP_SetModeMultiColor()
{
	// Backup vdp register @todo Can be optimized
	for(i8 i = 0; i < 14; i += 2)
		g_VDP_REGSAV[modeMC[i + 1] & 0x7F] = modeMC[i];

	__asm
		ld		hl, #_modeMC
		ld		bc, #(14 * 0x100 + P_VDP_ADDR) // [num|0x99]
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
static const u8 modeG1[] = 
{
	0x00, VDP_REG(0), // Graphics 1 Mode, No External Video
	0xC0, VDP_REG(1), // 16K, Enable display, Disable Int, 8x8 Srpites, Mag. off
	0x05, VDP_REG(2), // Address of Name Table in VRAM = 1400h
	0x80, VDP_REG(3), // Address of Color Table in VRAM = 2000h
	0x01, VDP_REG(4), // Address of Patern Table in VRAM = 0800h
	0x20, VDP_REG(5), // Address of Sprite Attribute Table in VRAM = 1000h
	0x00, VDP_REG(6), // Address of Sprite Pattern Table in VRAM = 0000h
	0x01, VDP_REG(7), // Backdrop Color = Black
};
void VDP_SetModeGraphic1()
{
	// Backup vdp register @todo Can be optimized
	for(i8 i = 0; i < 16; i += 2)
		g_VDP_REGSAV[modeG1[i + 1] & 0x7F] = modeG1[i];

	__asm
		ld		hl, #_modeG1
		ld		bc, #(16 * 0x100 + P_VDP_ADDR) // [num|0x99]
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
static const u8 modeG2[] = 
{
	0x02, VDP_REG(0), // Graphics 2 Mode, No External Video
	0xC2, VDP_REG(1), // 16K, Enable Disp., Disable Int., 16x16 Sprites, Mag. Off
	0x0E, VDP_REG(2), // Address of Name Table in VRAM = 3800h
	0xFF, VDP_REG(3), // Address of Color Table in VRAM = 2000h
	0x03, VDP_REG(4), // Address of Patern Table in VRAM = 0000h
	0x76, VDP_REG(5), // Address of Sprite Attribute Table in VRAM = 3B00h
	0x03, VDP_REG(6), // Address of Sprite Pattern Table in VRAM = 1800h
	0x0F, VDP_REG(7), // Backdrop Color = White
};
void VDP_SetModeGraphic2()
{
	// Backup vdp register @todo Can be optimized
	for(i8 i = 0; i < 16; i += 2)
		g_VDP_REGSAV[modeG2[i + 1] & 0x7F] = modeG2[i];

	__asm
		ld		hl, #_modeG2
		ld		bc, #(16 * 0x100 + P_VDP_ADDR) // [num|0x99]
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
//   M S X   2   F U N C T I O N S
//
//-----------------------------------------------------------------------------
#if (MSX_VERSION >= MSX_2)

void VDP_RegIncWrite(u16 src, u8 size, u8 reg)
{
	src, size, reg;
	
	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		// Backup VDP registers
		//ld		de, #(_g_VDP_REGSAV + 0) // first reg
		ld		hl, #_g_VDP_REGSAV
		ld		b, #0
		ld		c, 7(ix)				// first register
		add		hl, bc
		ld		d, h
		ld		e, l

		ld		b, #0
		ld		c, 6(ix)				// size
		ld		l, 4(ix)				// source address
		ld		h, 5(ix)
		ldir

		// Setup incremental VDP port writing
		ld		a, 7(ix)				// first register
		di //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a
		ld		a, #VDP_REG(17)
		ei //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a

		// Do incremental VDP port writing
		ld		b, 6(ix)				// size
		ld		c, #P_VDP_REGS
		ld		l, 4(ix)				// source address
		ld		h, 5(ix)
		otir

		pop		ix
	__endasm;
}

#define OUTI_1	\
	ei			\
	outi

#define OUTI_2	\
	OUTI_1		\
	outi

#define OUTI_3	\
	OUTI_2		\
	outi

#define OUTI_4	\
	OUTI_3		\
	outi

#define OUTI_5	\
	OUTI_4		\
	outi

#define OUTI_6	\
	OUTI_5	\
	outi

#define OUTI_7	\
	OUTI_6		\
	outi

#define OUTI_8	\
	OUTI_7		\
	outi

#define OUTI_9	\
	OUTI_8		\
	outi

#define OUTI_10	\
	OUTI_9		\
	outi

//#define OUTI(_x)	OUTI_##_x
#define OUTI(_x)	\
	.rept _x		\
		outi		\
	.endm			\
	ei

// Fast incremental write to VDP register with backup to RAM
#define ASM_REG_WRITE_INC_BK(_addr, _reg, _count)	\
	__asm											\
		ld		hl, #(_##_addr)						\
		ld		de, #(_g_VDP_REGSAV + _reg)			\
		ld		bc, #(_count)						\
		ldir										\
	__endasm;										\
	ASM_REG_WRITE_INC(_addr, _reg, _count)

// Fast incremental write to VDP register
#define ASM_REG_WRITE_INC(_addr, _reg, _count)		\
	__asm											\
		ld		a, #(_reg)							\
		di											\
		out		(#P_VDP_ADDR), a					\
		ld		a, #VDP_REG(17)						\
		ei											\
		out		(#P_VDP_ADDR), a					\
		ld		hl, #(_##_addr)						\
		ld		c, #P_VDP_REGS						\
		di											\
		OUTI(_count) ; 'ei' included				\
	__endasm

//-----------------------------------------------------------------------------
static const u8 modeT2[] = 
{
	0x04, // R#0
	0x70, // R#1
	0x03, // R#2
	0x27, // R#3
	0x02, // R#4
	0x36, // R#5
	0x07, // R#6
	0xF4, // R#7
	0x08, // R#8
	0x02, // R#9
};
// Set Screen Mode Text 2
void VDP_SetModeText2()
{
	ASM_REG_WRITE_INC_BK(modeT2, 0, 10);
}

//-----------------------------------------------------------------------------
static const u8 modeG3[] = 
{
	0x04, // R#0
	0x60, // R#1
	0x06, // R#2
	0xFF, // R#3
	0x03, // R#4
	0x3F, // R#5
	0x07, // R#6
	0x04, // R#7
	0x08, // R#8
	0x02, // R#9
};
// Set Screen Mode Graphics 3
void VDP_SetModeGraphic3()
{
	ASM_REG_WRITE_INC_BK(modeG3, 0, 10);
}

//-----------------------------------------------------------------------------
static const u8 modeG4[] = 
{
	0x06, // R#0
	0x60, // R#1
	0x1F, // R#2
	0xFF, // R#3
	0x03, // R#4
	0xEF, // R#5
	0x0F, // R#6
	0x04, // R#7
	0x08, // R#8
	0x82, // R#9
};
// Set Screen Mode Graphics 4
void VDP_SetModeGraphic4()
{
	ASM_REG_WRITE_INC_BK(modeG4, 0, 10);
}

//-----------------------------------------------------------------------------
static const u8 modeG5[] = 
{
	0x08, // R#0
	0x60, // R#1
	0x1F, // R#2
	0xFF, // R#3
	0x03, // R#4
	0xEF, // R#5
	0x0F, // R#6
	0x04, // R#7
	0x08, // R#8
	0x82, // R#9
};
// Set Screen Mode Graphics 5
void VDP_SetModeGraphic5()
{
	ASM_REG_WRITE_INC_BK(modeG5, 0, 10);
}

//-----------------------------------------------------------------------------
static const u8 modeG6[] = 
{
	0x0A, // R#0
	0x60, // R#1
	0x1F, // R#2
	0xFF, // R#3
	0x03, // R#4
	0xF7, // R#5
	0x1E, // R#6
	0x04, // R#7
	0x08, // R#8
	0x82, // R#9
};
// Set Screen Mode Graphics 6
void VDP_SetModeGraphic6()
{
	ASM_REG_WRITE_INC_BK(modeG6, 0, 10);
}

//-----------------------------------------------------------------------------
static const u8 modeG7[] = 
{
	0x0E, // R#0
	0x60, // R#1
	0x1F, // R#2
	0xFF, // R#3
	0x03, // R#4
	0xF7, // R#5
	0x1E, // R#6
	0x04, // R#7
	0x08, // R#8
	0x82, // R#9
};
// Set Screen Mode Graphics 7
void VDP_SetModeGraphic7()
{
	ASM_REG_WRITE_INC_BK(modeG7, 0, 10);
}

#endif

//-----------------------------------------------------------------------------
//
//   M S X   2 +   F U N C T I O N S
//
//-----------------------------------------------------------------------------






//-----------------------------------------------------------------------------
//
//   C O M M O N   F U N C T I O N S
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
inline void VDP_SetScreen(const u8 mode)
{
	switch(mode)
	{
	//case VDP_MODE_SCREEN0:
	case VDP_MODE_SCREEN0_W40:
	case VDP_MODE_TEXT1:
		VDP_SetModeText1();
		return;
	
	case VDP_MODE_SCREEN3:
	case VDP_MODE_MULTICOLOR:
		VDP_SetModeMultiColor();
		return;

	case VDP_MODE_SCREEN1:
	case VDP_MODE_GRAPHIC1:
		VDP_SetModeGraphic1();
		return;

	case VDP_MODE_SCREEN2:
	case VDP_MODE_GRAPHIC2:
		VDP_SetModeGraphic2();
		return;

#if (MSX_VERSION >= MSX_2)
	case VDP_MODE_SCREEN0_W80:
	case VDP_MODE_TEXT2:
		VDP_SetModeText2();
		return;

	case VDP_MODE_SCREEN4:
	case VDP_MODE_GRAPHIC3:
		VDP_SetModeGraphic3();
		return;
	
	case VDP_MODE_SCREEN5:
	case VDP_MODE_GRAPHIC4:
		VDP_SetModeGraphic4();
		return;
	
	case VDP_MODE_SCREEN6:
	case VDP_MODE_GRAPHIC5:
		VDP_SetModeGraphic5();
		return;
	
	case VDP_MODE_SCREEN7:
	case VDP_MODE_GRAPHIC6:
		VDP_SetModeGraphic6();
		return;
		
	case VDP_MODE_SCREEN8:
	case VDP_MODE_GRAPHIC7:
		VDP_SetModeGraphic7();
		return;
		
	// case VDP_MODE_SCREEN9:
	case VDP_MODE_SCREEN9_40:
		VDP_SetModeGraphic5();
		// @todo Further setting needed
		return;
	
	case VDP_MODE_SCREEN9_80:
		VDP_SetModeGraphic4();
		// @todo Further setting needed
		return;

#endif
#if (MSX_VERSION >= MSX_2Plus)
	case VDP_MODE_SCREEN10:
		VDP_SetModeGraphic7();
		// @todo Further setting needed
		return;

	case VDP_MODE_SCREEN11:
		VDP_SetModeGraphic7();
		// @todo Further setting needed
		return;

	case VDP_MODE_SCREEN12:
		VDP_SetModeGraphic7();
		// @todo Further setting needed
		return;
#endif
	}
}

//-----------------------------------------------------------------------------
// Wait for VBlank flag trigger
void VDP_WaitVBlank()
{
	__asm
	WAIT_RETRACE:
		in		a, (P_VDP_STAT)
		// rla
		// jr		nc, WAIT_RETRACE
		and		#0x80
		cp		#0
		jr		z, WAIT_RETRACE
	__endasm;
}

//-----------------------------------------------------------------------------
// Read default S#0 register
u8 VDP_ReadDefaultStatus()
{
	__asm
		in		a, (P_VDP_STAT)
		ld		l, a
	__endasm;
}

//-----------------------------------------------------------------------------
// Read a given status register then reset status register to default (0)
u8 VDP_ReadStatus(u8 stat) __FASTCALL
{
	stat;
	// FastCall
	//	ld		l, stat
	__asm
		ld		a, l
		di //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a
		ld		a, #VDP_REG(15)
		out		(#P_VDP_ADDR), a
		in		a, (#P_VDP_STAT)
		ld		l, a				// return value
		xor		a           		// ld a, 0
		out		(#P_VDP_ADDR), a
		ld		a, #VDP_REG(15)
		ei //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR),a
	__endasm;
}

//-----------------------------------------------------------------------------
// Write data from RAM to VRAM
void VDP_WriteVRAM(u8* src, u16 destAddr, u8 destPage, u16 count)
{
	src, destAddr, destPage, count;
	__asm
		push	ix
		ld		ix, #0
		add		ix, sp
		
		// g_PortVDPAddr = (page << 2) + (dest >> 14);
		ld		a, 8 (ix)
		add		a, a
		add		a, a
		ld		c, a
		ld		a, 7 (ix)
		rlca
		rlca
		and		a, #0x03
		add		a, c
		di //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(P_VDP_ADDR), a
		// g_PortVDPAddr = VDP_REG(14);
		ld		a, #VDP_REG(14)
		out		(P_VDP_ADDR), a
		// g_PortVDPAddr = (dest & 0xFF);
		ld		a, 6 (ix)
		out		(P_VDP_ADDR), a
		// g_PortVDPAddr = ((dest >> 8) & 0x3F) + F_VDP_WRIT;
		ld		a, 7 (ix)
		and		a, #0x3f
		add		a, #F_VDP_WRIT
		out		(P_VDP_ADDR), a
		ei //~~~~~~~~~~~~~~~~~~~~~~~~~~
		
		// while(count--) g_PortVDPData = *src++;
		ld		l, 4 (ix)				// source address
		ld		h, 5 (ix)
		ld		c, #P_VDP_DATA			// data register
		ld		b, 9 (ix)				// count LSB
		
		otir							// send first package (count & 0x00FF)
		
		ld		a, 10 (ix)				// count MSB
	wrt_loop_start:
		cp		a, #0
		jp		z, wrt_loop_end

		ld		b, #0					// send 256 bytes packages
		otir
		dec		a
		jp		wrt_loop_start
	wrt_loop_end:
	
		pop	ix
	__endasm;
}

//-----------------------------------------------------------------------------
// Fill VRAM area with a given value
void VDP_FillVRAM(u8 value, u16 dest, u8 page, u16 count)
{
	dest, page, value, count;
	__asm
		push	ix
		ld		ix, #0
		add		ix, sp
		
		// g_PortVDPAddr = (page << 2) + (dest >> 14);
		ld		a, 7 (ix)
		add		a, a
		add		a, a
		ld		c, a
		ld		a, 6 (ix)
		rlca
		rlca
		and		a, #0x03
		add		a, c
		di //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(P_VDP_ADDR), a
		// g_PortVDPAddr = VDP_REG(14);
		ld		a, #VDP_REG(14)
		out		(P_VDP_ADDR), a
		// g_PortVDPAddr = (dest & 0xFF);
		ld		a, 5 (ix)
		out		(P_VDP_ADDR), a
		// g_PortVDPAddr = ((dest >> 8) & 0x3F) + F_VDP_WRIT;
		ld		a, 6 (ix)
		and		a, #0x3f
		add		a, #F_VDP_WRIT
		out		(P_VDP_ADDR), a
		ei //~~~~~~~~~~~~~~~~~~~~~~~~~~
		
		// while(count--) g_PortVDPData = value;
		ld		e, 8 (ix)				// count
		ld		d, 9 (ix)
		ld		a, 4 (ix)				// value
		// fast 16-bits loop
		ld		b, e					// Number of loops is in DE
		dec		de						// Calculate DB value (destroys B, D and E)
		inc		d
	fll_loop_start:
		out		(P_VDP_DATA), a
		// ... do something here
		djnz	fll_loop_start
		dec		d
		jp		nz, fll_loop_start

		pop	ix
	__endasm;
}

//-----------------------------------------------------------------------------
//
void VDP_ReadVRAM(u16 srcAddr, u8 srcPage, u8* dest, u16 count)
{
	// @todo Convert to assembler
	DisableInterrupt();
	g_PortVDPAddr = (srcPage << 2) + (srcAddr >> 14);
	g_PortVDPAddr = (u8)(VDP_REG(14));
	g_PortVDPAddr = (srcAddr & 0xFF);
	g_PortVDPAddr = ((srcAddr >> 8) & 0x3F) + F_VDP_READ;
	while(count--)
		*dest++ = g_PortVDPData;
	EnableInterrupt();
}

//-----------------------------------------------------------------------------
void VDP_RegWriteFC(u16 reg_value) __FASTCALL
{
	reg_value;
	// FastCall
	//	ld		h, reg
	//	ld		l, value
	__asm
		ld		a, l
		di //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a
		ld		a, h
		add		#0x80
		ei //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a
	__endasm;
}

//-----------------------------------------------------------------------------
void VDP_RegWriteBakFC(u16 reg_value) __FASTCALL
{
	reg_value;
	// FastCall
	//	ld		h, reg
	//	ld		l, value
	__asm
		ld		a, l
		ld		c, h
		// Backup
		ld		b, #0
		ld		hl, #_g_VDP_REGSAV
		add		hl, bc
		ld		(hl), a
		// Write to register
		di //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a
		ld		a, c
		add		#0x80
		ei //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(#P_VDP_ADDR), a
	__endasm;
}

//-----------------------------------------------------------------------------
// Fast write to VDP register
#define ASM_REG_WRITE(_reg, _val)					\
	__asm											\
		ld		a, _val								\
		di											\
		out		(#P_VDP_ADDR), a					\
		ld		a, #VDP_REG(_reg)					\
		ei											\
		out		(#P_VDP_ADDR), a					\
	__endasm

//-----------------------------------------------------------------------------
// Fast write to VDP register with backup to RAM
#define ASM_REG_WRITE_BK(_reg, _val)				\
	__asm											\
		ld		a, _val								\
		ld		(_g_VDP_REGSAV + 7), a				\
	__endasm;										\
	ASM_REG_WRITE(_reg, _val)

//-----------------------------------------------------------------------------
// Enable/disable horizontal interruption
void VDP_InterruptHBlank(u8 enable) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[0];
	reg &= ~R00_IE1;
	if(enable)
		reg |= R00_IE1;	
	VDP_RegWriteBak(0, reg);
}

//-----------------------------------------------------------------------------
// Enable/disable vertical interruption
void VDP_InterruptVBlank(u8 enable) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[1];
	reg &= ~R01_IE0;
	if(enable)
		reg |= R01_IE0;	
	VDP_RegWriteBak(1, reg);
}

//-----------------------------------------------------------------------------
// Enable/disable screen display
void VDP_EnableDisplay(u8 enable) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[1];
	reg &= ~R01_BL;
	if(enable)
		reg |= R01_BL;	
	VDP_RegWriteBak(1, reg);
}

//-----------------------------------------------------------------------------
// Set sprite parameters
#define VDP_SPRITE_SIZE_8		0
#define VDP_SPRITE_SIZE_16		R01_ST
#define VDP_SPRITE_ENLARGE		R01_MAG
void VDP_SetSpriteFlag(u8 flag) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[1];
	reg &= ~(R01_ST | R01_MAG);
	if(flag & R01_ST)
		reg |= R01_ST;
	if(flag & R01_MAG)
		reg |= R01_MAG;
	VDP_RegWriteBak(1, reg);
}

//-----------------------------------------------------------------------------
// Enable/disable sprite
void VDP_EnableSprite(u8 enable) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[8];
	reg &= ~R08_SPD;
	if(enable)
		reg |= R08_SPD;
	VDP_RegWriteBak(8, reg);
}

//-----------------------------------------------------------------------------
// Enable/disable grayscale
void VDP_SetGrayScale(u8 enable) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[8];
	reg &= ~R08_BW;
	if(enable)
		reg |= R08_BW;
	VDP_RegWriteBak(8, reg);
}

//-----------------------------------------------------------------------------
// Change VDP frequency
#define VDP_FREQ_50HZ			R09_NT
#define VDP_FREQ_60HZ			0
void VDP_SetFrequency(u8 freq) __FASTCALL
{
	u8 reg = g_VDP_REGSAV[9];
	reg &= ~R09_NT;
	if(freq)
		reg |= R09_NT;
	VDP_RegWriteBak(9, reg);
}

//-----------------------------------------------------------------------------
// Set text and border default color
void VDP_SetColor(u8 color) __FASTCALL
{
	VDP_RegWrite(7, color);
}


//-----------------------------------------------------------------------------
//
//   V D P   C O M M A N D S
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Wait for previous VDP command to be finished
void VDP_WaitReady()
{
	__asm
	wait_vdp_ready:
		ld		a, #2
		di  //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(P_VDP_ADDR), a		; select s#2
		ld		a, #VDP_REG(15)
		out		(P_VDP_ADDR), a
		in		a, (P_VDP_ADDR)
		rra
		ld		a, #0				; back to s#0, enable ints
		out		(P_VDP_ADDR), a
		ld		a, #VDP_REG(15)
		ei  //~~~~~~~~~~~~~~~~~~~~~~~~~~
		out		(P_VDP_ADDR), a		; loop if vdp not ready (CE)
		jp		c, wait_vdp_ready
	__endasm;
}

//-----------------------------------------------------------------------------
// Send VDP command (form registres 32 to 46)
void VPD_SendCommand32()
{
	VDP_WaitReady();
	ASM_REG_WRITE_INC(g_VDP_Command, 32, 15);
}

//-----------------------------------------------------------------------------
// Send VDP command (form registres 36 to 46)
void VPD_SendCommand36()
{
	VDP_WaitReady();
	ASM_REG_WRITE_INC(g_VDP_Command + 4, 36, 11);
}

