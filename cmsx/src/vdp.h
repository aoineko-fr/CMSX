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
#pragma once
#pragma sdcc_hash +

#include "core.h"
#include "vdp_reg.h"

//-----------------------------------------------------------------------------
// STRUCTURES
//-----------------------------------------------------------------------------

/// Structure used to store register data for VDP command
struct VDP_Command
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
};

/// Structure to store a sprite attribute. @see function VDP_SetSpriteAttribute
struct VDP_Sprite
{
    u8 Y;			///< Y coordinate on screen (all lower priority sprite will be disable if equal to 216 or 0xD0)
    u8 X;			///< X coordinate of the sprite
    u8 Pattern;		///< Pattern index
    u8 Color   : 4;	///< Color index (Sprite Mode 1 only)
    u8 _unused : 3;	///< (unused 3 bits)
    u8 EC      : 1;	///< Early clock ; used to offset sprite by  32  dots  to  the  left  (Sprite Mode 1 only)
} ;
 
//-----------------------------------------------------------------------------
// EXTERNALS
//-----------------------------------------------------------------------------

extern u8 g_VDP_REGSAV[28];
extern u8 g_VDP_STASAV[10];
extern struct VDP_Command g_VDP_Command;
extern struct VDP_Sprite g_VDP_Sprite;

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
	VDP_MODE_SCREEN0     = VDP_MODE_TEXT1,
	VDP_MODE_SCREEN0_W40 = VDP_MODE_SCREEN0,
	VDP_MODE_SCREEN1     = VDP_MODE_GRAPHIC1,
	VDP_MODE_SCREEN2     = VDP_MODE_GRAPHIC2,
	VDP_MODE_SCREEN3     = VDP_MODE_MULTICOLOR,
#if (MSX_VERSION >= MSX_2)
	VDP_MODE_SCREEN0_W80 = VDP_MODE_TEXT2,
	VDP_MODE_SCREEN4     = VDP_MODE_GRAPHIC3,
	VDP_MODE_SCREEN5     = VDP_MODE_GRAPHIC4,
	VDP_MODE_SCREEN6     = VDP_MODE_GRAPHIC5,
	VDP_MODE_SCREEN7     = VDP_MODE_GRAPHIC6,
	VDP_MODE_SCREEN8     = VDP_MODE_GRAPHIC7,
	VDP_MODE_SCREEN9,
	VDP_MODE_SCREEN9_40 = VDP_MODE_SCREEN9,
	VDP_MODE_SCREEN9_80,
#endif
#if (MSX_VERSION >= MSX_2Plus)
	VDP_MODE_SCREEN10,
	VDP_MODE_SCREEN11,
	VDP_MODE_SCREEN12,
#endif
	VDP_MODE_MAX,
};

//-----------------------------------------------------------------------------
// MSX 1 FUNCTIONS
//-----------------------------------------------------------------------------

/// Set screen mode to Text 1
void VDP_SetModeText1();

/// Set screen mode to Multi-color
void VDP_SetModeMultiColor();

/// Set screen mode to Graphic 1
void VDP_SetModeGraphic1();

/// Set screen mode to Graphic 2
void VDP_SetModeGraphic2();

//-----------------------------------------------------------------------------
// MSX 2 FUNCTIONS
//-----------------------------------------------------------------------------

#if (MSX_VERSION >= MSX_2)

/// Set screen mode to Text 2
void VDP_SetModeText2();

/// Set screen mode to Graphic 3
void VDP_SetModeGraphic3();

/// Set screen mode to Graphic 4
void VDP_SetModeGraphic4();

/// Set screen mode to Graphic 5
void VDP_SetModeGraphic5();

/// Set screen mode to Graphic 6
void VDP_SetModeGraphic6();

/// Set screen mode to Graphic 7
void VDP_SetModeGraphic7();

#endif // (MSX_VERSION >= MSX_2)

//-----------------------------------------------------------------------------
// MSX 2+ FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// COMMON FUNCTIONS
//-----------------------------------------------------------------------------

/// Set screen mode. @see VDP_MODE
inline void VDP_SetScreen(const u8 mode);

/// Wait for VBlank flag trigger
void VDP_WaitVBlank();

/// Read default S#0 register
u8 VDP_ReadDefaultStatus();

/// Read a given status register then reset status register to default (0)
u8 VDP_ReadStatus(u8 stat) __FASTCALL;

/// Write data from RAM to VRAM
void VDP_WriteVRAM(u8* src, u16 destAddr, u8 destPage, u16 count);

/// Fill VRAM area with a given value
void VDP_FillVRAM(u8 value, u16 destAddr, u8 destPage, u16 count);

///
void VDP_ReadVRAM(u16 srcAddr, u8 srcPage, u8* dest, u16 count);

/// Enable/disable horizontal interruption
void VDP_EnableHBlank(u8 enable) __FASTCALL;

///
void VDP_SetHBlankLine(u8 line) __FASTCALL;

///
void VDP_SetVerticalOffset(u8 offset) __FASTCALL;

/// Enable/disable vertical interruption
void VDP_EnableVBlank(u8 enable) __FASTCALL;

/// Enable/disable screen display
void VDP_EnableDisplay(u8 enable) __FASTCALL;

/// Enable/disable grayscale
void VDP_SetGrayScale(u8 enable) __FASTCALL;

#define VDP_FREQ_50HZ			R09_NT		///< Frequency at 50 Hz
#define VDP_FREQ_60HZ			0			///< Frequency at 60 Hz
/// Change VDP frequency
void VDP_SetFrequency(u8 freq) __FASTCALL;

/// Set current VRAM page
void VDP_SetPage(u8 page) __FASTCALL;

/// Set text and border default color
void VDP_SetColor(u8 color) __FASTCALL;

/// Set a new palette [red|blue][0|green]
void VDP_SetPalette(void* pal) __FASTCALL;

/// Set palette entry color
void VDP_SetPaletteEntry(u8 index, u16 color);

#define VDP_LINE_192			0			///< 192 lines mode
#define VDP_LINE_212			R09_LN		///< 212 lines mode
/// Set line count for the current screen mode
void VDP_SetLineCount(u8 lines) __FASTCALL;

/// Enable or disable interlace mode
void VDP_SetInterlace(u8 enable) __FASTCALL;

/// Enable automatic page switch on even/odd frames
void VDP_SetPageAlternance(u8 enable) __FASTCALL;

//-----------------------------------------------------------------------------
// SPRITES
//-----------------------------------------------------------------------------
#define VDP_SPRITE_SIZE_8		0			///< Use 8x8 sprite size
#define VDP_SPRITE_SIZE_16		R01_ST		///< Use 16x16 sprite size
#define VDP_SPRITE_ENLARGE		R01_MAG		///> Double the size of the sprite (1 dot = 2 pixels)
/// Set sprite parameters
void VDP_SetSpriteFlag(u8 flag) __FASTCALL;

/// Enable/disable sprite
void VDP_EnableSprite(u8 enable) __FASTCALL;

/// Set sprite attribute
void VDP_SendSpriteAttribute(u8 index) __FASTCALL;

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

/// Write to VRAM command loop
void VPD_WriteCommandLoop(void* address) __FASTCALL;

//-----------------------------------------------------------------------------
// INLINE FUNCTIONS
//-----------------------------------------------------------------------------

#include "vdp_cmd.h"