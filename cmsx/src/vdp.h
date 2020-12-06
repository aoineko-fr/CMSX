//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
/**
 * @file Features to manage the VDP - Defines, structures and functions header
 *
 * References:
 * - V9938 Technical Data Book Programmer's Guide
 * - https://www.msx.org/wiki/Category:VDP_Registers
 * - http://map.grauw.nl/articles/
 */
#pragma sdcc_hash +

#include "core.h"
#include "vdp_reg.h"

//-----------------------------------------------------------------------------
// STRUCTURES
//-----------------------------------------------------------------------------

/// 
typedef struct tagVDP_Command
{
	u16 SX;  // 32-33
	u16 SY;  // 34-35
	u16 DX;  // 36-37
	u16 DY;  // 38-39
	u16 NX;  // 40-41
	u16 NY;  // 42-43
	u8  CLR; // 44
	u8  ARG; // 45
	u8  CMD; // 46
} VDP_Command;

//-----------------------------------------------------------------------------
// EXTERNALS
//-----------------------------------------------------------------------------

extern u8 g_VDP_REGSAV[28];
extern u8 g_VDP_STASAV[10];
extern VDP_Command g_VDP_Command;

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

#define REGSAV(_n) #(_g_VDP_REGSAV+_n)
#define STASAV(_n) #(g_VDP_STASAV+_n)

/// VDP display modes
enum VDP_MODE
{
	// VDP modes
	VDP_MODE_TEXT1 = 0,		// 00001	40 characters per line of text, one colour for each characte
	VDP_MODE_MULTICOLOR,	// 00010	pseudo-graphic, one character divided into four block
	VDP_MODE_GRAPHIC1,		// 00000	32 characters per one line of text, the COLOURed character available
	VDP_MODE_GRAPHIC2,		// 00100	256 x 192, the colour is specififed for each 8 dots
#if (MSX_VERSION >= MSX_2)
	VDP_MODE_TEXT2,			// 01001	80 characters per line of text, character blinkable selection
	VDP_MODE_GRAPHIC3,		// 01000	GRAPHIC 2 which can use sprite mode 2
	VDP_MODE_GRAPHIC4,		// 01100	256 x 212; 16 colours are available for each dot
	VDP_MODE_GRAPHIC5,		// 10000	512 x 212; 4 colours are available for each dot
	VDP_MODE_GRAPHIC6,		// 10100	512 x 212; 16 colours are available for each dot
	VDP_MODE_GRAPHIC7,		// 11100	256 x 212; 256 colours are available for each dot
#endif

	// BASIC screens
	VDP_MODE_SCREEN0,		// VDP_MODE_TEXT1
	VDP_MODE_SCREEN0_W40 = VDP_MODE_SCREEN0,
	VDP_MODE_SCREEN3,		// VDP_MODE_MULTICOLOR
	VDP_MODE_SCREEN1,		// VDP_MODE_GRAPHIC1
	VDP_MODE_SCREEN2,		// VDP_MODE_GRAPHIC2
#if (MSX_VERSION >= MSX_2)
	VDP_MODE_SCREEN0_W80,	// VDP_MODE_TEXT2,	
	VDP_MODE_SCREEN4,       // VDP_MODE_GRAPHIC3
	VDP_MODE_SCREEN5,       // VDP_MODE_GRAPHIC4
	VDP_MODE_SCREEN6,       // VDP_MODE_GRAPHIC5
	VDP_MODE_SCREEN7,       // VDP_MODE_GRAPHIC6
	VDP_MODE_SCREEN8,       // VDP_MODE_GRAPHIC7
	VDP_MODE_SCREEN9,
	VDP_MODE_SCREEN9_40 = VDP_MODE_SCREEN9,
	VDP_MODE_SCREEN9_80,
#endif
#if (MSX_VERSION >= MSX_2Plus)
	VDP_MODE_SCREEN10,
	VDP_MODE_SCREEN11,
	VDP_MODE_SCREEN12,
#endif
};

//-----------------------------------------------------------------------------
// MSX 1 FUNCTIONS
//-----------------------------------------------------------------------------

//
void VDP_SetModeText1();

//
void VDP_SetModeMultiColor();

//
void VDP_SetModeGraphic1();

//
void VDP_SetModeGraphic2();

//-----------------------------------------------------------------------------
// MSX 2 FUNCTIONS
//-----------------------------------------------------------------------------

#if (MSX_VERSION >= MSX_2)

//
void VDP_SetModeText2();

//
void VDP_SetModeGraphic3();

//
void VDP_SetModeGraphic4();

//
void VDP_SetModeGraphic5();

//
void VDP_SetModeGraphic6();

//
void VDP_SetModeGraphic7();

#endif

//-----------------------------------------------------------------------------
// MSX 2+ FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// COMMON FUNCTIONS
//-----------------------------------------------------------------------------

//
inline void VDP_SetScreen(const u8 mode);

// Wait for VBlank flag trigger
void VDP_WaitVBlank();

// Read default S#0 register
u8 VDP_ReadDefaultStatus();

// Read a given status register then reset status register to default (0)
u8 VDP_ReadStatus(u8 stat) __FASTCALL;

// Write data from RAM to VRAM
void VDP_WriteVRAM(u8* src, u16 destAddr, u8 destPage, u16 count);

// Fill VRAM area with a given value
void VDP_FillVRAM(u8 value, u16 destAddr, u8 destPage, u16 count);

//
void VDP_ReadVRAM(u16 srcAddr, u8 srcPage, u8* dest, u16 count);

// Enable/disable horizontal interruption
void VDP_InterruptHBlank(u8 enable) __FASTCALL;

// Enable/disable vertical interruption
void VDP_InterruptVBlank(u8 enable) __FASTCALL;

// Enable/disable screen display
void VDP_EnableDisplay(u8 enable) __FASTCALL;

// Set sprite parameters
void VDP_SetSpriteFlag(u8 flag) __FASTCALL;

// Enable/disable sprite
void VDP_EnableSprite(u8 flag) __FASTCALL;

// Enable/disable grayscale
void VDP_SetGrayScale(u8 enable) __FASTCALL;

// Change VDP frequency
void VDP_SetFrequency(u8 freq) __FASTCALL;

// Set text and border default color
void VDP_SetColor(u8 color) __FASTCALL;

//-----------------------------------------------------------------------------
// VDP REGISTERS
//-----------------------------------------------------------------------------

#define VDP_RegWrite(reg, value) VDP_RegWriteFC(FC88(reg, value))
void VDP_RegWriteFC(u16 reg_value) __FASTCALL;

#define VDP_RegWriteBak(reg, value) VDP_RegWriteBakFC(FC88(reg, value))
void VDP_RegWriteBakFC(u16 reg_value) __FASTCALL;

//-----------------------------------------------------------------------------
// VDP COMMANDS
//-----------------------------------------------------------------------------

/// Wait for previous VDP command to be finished
void VDP_WaitReady();

/// Send VDP command (form registres 32 to 46)
void VPD_SendCommand32();

/// Send VDP command (form registres 36 to 46)
void VPD_SendCommand36();

//-----------------------------------------------------------------------------
// INLINE FUNCTIONS
//-----------------------------------------------------------------------------

#include "vdp.inl"