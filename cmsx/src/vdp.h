//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
/**
 * @file Features to manage the VDP - Defines, structures and functions header
 *
 * References:
 *  - V9938 Technical Data Book Programmer's Guide
 *  - https://www.msx.org/wiki/Category:VDP_Registers
 *  - http://map.grauw.nl/articles/
 */
#pragma once
#pragma sdcc_hash +

#include "core.h"
#include "vdp_reg.h"

// @todo Handle VRAM read/write access timing
//
// Minimum VRAM access timings in 3.58 MHz Z80 cycles
//
// Screen	VDP		MSX1	MSX2/2+
// mode		mode	cycles	cycles
//---------------------------------
// 0,W40	T1		12		20
// 0,W80	T2				20
// 1		G1		29		15
// 2		G2		29		15
// 3		MC		13		15
// 4		G3				15
// 5		G4				15
// 6		G5				15
// 7		G6				15
// 8		G7				15
//
// outi			; 18 T-States
//
// otir			; 23 T-States
//
// OutiToVram:  ; 29 T-States (18 + 11)
//     outi
//     jp nz, OutiToVram


//-----------------------------------------------------------------------------
// STRUCTURES
//-----------------------------------------------------------------------------

/// Structure used to store VDP module data
struct VDP_Data
{
	u8  Mode;		///< Current screen mode (@see VDP_MODE)
	u8  BPC : 4;	///< Bits per color of the current mode (can be 1, 2, 4 or 8)
	u8  Width : 1;	///< Width of the current screen (0: 256 px, 1: 512 px)
	u8  Height : 1;	///< Height of the current screen (0: 192 px, 1: 212 px)
};

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
    u8 Color;		///< Color index (Sprite Mode 1 only)
    // u8 Color   : 4;	///< Color index (Sprite Mode 1 only)
    // u8 _unused : 3;	///< (unused 3 bits)
    // u8 EC      : 1;	///< Early clock ; used to offset sprite by  32  dots  to  the  left  (Sprite Mode 1 only)
};

//-----------------------------------------------------------------------------
// EXTERNALS
//-----------------------------------------------------------------------------

extern u8 g_VDP_REGSAV[28];
extern u8 g_VDP_STASAV[10];
extern struct VDP_Data    g_VDP_Data;
extern struct VDP_Command g_VDP_Command;
extern struct VDP_Sprite  g_VDP_Sprite;

extern u16 g_ScreenLayoutLow;		///< Address of the Pattern Layout Table (Name)
extern u16 g_ScreenColorLow;		///< Address of the Color Table
extern u16 g_ScreenPatternLow;		///< Address of the Pattern Generator Table
extern u16 g_SpriteAtributeLow;		///< Address of the Sprite Attribute Table
extern u16 g_SpritePatternLow;		///< Address of the Sprite Pattern Generator Table
extern u16 g_SpriteColorLow;		///< Address of the Sprite Color Table
#if (VDP_VRAM_ADDR == VDP_VRAM_ADDR_17)
	extern u8 g_ScreenLayoutHigh;	///< Address of the Pattern Layout Table (Name)
	extern u8 g_ScreenColorHigh;	///< Address of the Color Table
	extern u8 g_ScreenPatternHigh;	///< Address of the Pattern Generator Table
	extern u8 g_SpriteAtributeHigh;	///< Address of the Sprite Attribute Table
	extern u8 g_SpritePatternHigh;	///< Address of the Sprite Pattern Generator Table
	extern u8 g_SpriteColorHigh;	///< Address of the Sprite Color Table
#endif

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

#if (VDP_VRAM_ADDR == VDP_VRAM_ADDR_16)
	#define VADDR			u16
	#define VADDR_Lo(a)		(a)
	#define VADDR_Hi(a)		0
#else // if (VDP_VRAM_ADDR == VDP_VRAM_ADDR_17)
	#define VADDR			u32
	#define VADDR_Lo(a)		(u16)(a)
	#define VADDR_Hi(a)		(u16)(a >> 16)
#endif

#if (VDP_UNIT == VDP_UNIT_U16)
	#define UX				u16
	#define UY				u16
#elif (VDP_UNIT == VDP_UNIT_X16)
	#define UX				u16
	#define UY				u8
#elif (VDP_UNIT == VDP_UNIT_Y16)
	#define UX				u8
	#define UY				u16
#else // if (VDP_UNIT == VDP_UNIT_U8)
	#define UX				u8
	#define UY				u8
#endif


#define VRAM16b(a)			(u16)((u32)(a >> 4))
#define VRAM17b(a)			(u16)((u32)(a >> 1))
#define Addr20bTo16b(a)		(u16)((u32)(a >> 4))	///< Convert 20-bits (V)RAM address into 16-bits with bit shifting
#define Addr17bTo16b(a)		(u16)((u32)(a >> 1))	///< Convert 17-bits (V)RAM address into 16-bits with bit shifting

#define REGSAV(a)			#(_g_VDP_REGSAV+a)
#define STASAV(a)			#(_g_VDP_STASAV+a)

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

/// VDP version enumaration
enum VDP_VERSION
{
	VDP_VERSION_TMS9918A = 0,	// MSX1 VDP
	VDP_VERSION_V9938,			// MSX2 VDP
	VDP_VERSION_V9958,			// MSX2+/tR VDP
};

//-----------------------------------------------------------------------------
// MSX 1 FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MSX 2 FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MSX 2+ FUNCTIONS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// COMMON FUNCTIONS
//-----------------------------------------------------------------------------

/// Set screen mode. @see VDP_MODE
void VDP_SetMode(const u8 mode) __FASTCALL;

/// Tell if the given screen mode is a bitmap mode (text mode otherwise)
bool VDP_IsBitmapMode(const u8 mode) __FASTCALL;

/// Get VDP version
u8 VDP_GetVersion() __naked;

/// Read default S#0 register
u8 VDP_ReadDefaultStatus();

/// Read a given status register then reset status register to default (0)
u8 VDP_ReadStatus(u8 stat) __FASTCALL;


/// Write data from RAM to VRAM
void VDP_WriteVRAM_64K(const u8* src, u16 dest, u16 count) __sdcccall(0);

/// Fill VRAM area with a given value
void VDP_FillVRAM_64K(u8 value, u16 dest, u16 count) __sdcccall(0);

/// Read data from VRAM to RAM
void VDP_ReadVRAM_64K(u16 src, u8* dest, u16 count) __sdcccall(0);

//-----------------------------------------------------------------------------
#if (VDP_VRAM_ADDR == VDP_VRAM_ADDR_16)

	#define VDP_WriteVRAM(src, destLow, destHigh, count)	VDP_WriteVRAM_64K(src, destLow, count)
	#define VDP_FillVRAM(value, destLow, destHigh, count)	VDP_FillVRAM_64K(value, destLow, count)
	#define VDP_ReadVRAM(srcLow, srcHigh, dest, count)		VDP_ReadVRAM_64K(srcLow, dest, count)

#else // (MSX_VERSION >= MSX_2)

	/// Write data from RAM to VRAM
	void VDP_WriteVRAM(const u8* src, u16 destLow, u8 destHigh, u16 count) __sdcccall(0);

	/// Fill VRAM area with a given value
	void VDP_FillVRAM(u8 value, u16 destLow, u8 destHigh, u16 count) __sdcccall(0);

	/// Read data from VRAM to RAM
	void VDP_ReadVRAM(u16 srcLow, u8 srcHigh, u8* dest, u16 count) __sdcccall(0);

#endif

/// Enable/disable horizontal interruption [MSX2/2+/TR]
void VDP_EnableHBlank(bool enable) __FASTCALL;

/// Set the horizontal-blank interruption line [MSX2/2+/TR]
void VDP_SetHBlankLine(u8 line) __FASTCALL;

/// Set the vertical rendeing offset (in pixel) [MSX2/2+/TR]
void VDP_SetVerticalOffset(u8 offset) __FASTCALL;

/// Adjustment of the display location on the screen [MSX2/2+/TR]
void VDP_SetAdjustOffset(u8 offset) __FASTCALL;

/// Enable/disable vertical interruption
void VDP_EnableVBlank(bool enable) __FASTCALL;

/// Enable/disable screen display
void VDP_EnableDisplay(bool enable) __FASTCALL;

/// Enable/disable grayscale
void VDP_SetGrayScale(bool enable) __FASTCALL;

#define VDP_FREQ_50HZ			R09_NT		///< Frequency at 50 Hz
#define VDP_FREQ_60HZ			0			///< Frequency at 60 Hz
/// Change VDP frequency
void VDP_SetFrequency(u8 freq) __FASTCALL;

/// Set current VRAM page
void VDP_SetPage(u8 page) __FASTCALL;

/// Set layout table VRAM address
void VDP_SetLayoutTable(VADDR addr) __FASTCALL;

/// Set color table VRAM address
void VDP_SetColorTable(VADDR addr) __FASTCALL;

/// Set pattern table VRAM address
void VDP_SetPatternTable(VADDR addr) __FASTCALL;

/// Set text and border default color (format: [TXT:4|BG:4])
void VDP_SetColor(u8 color) __FASTCALL;

/// Set a new palette [red|blue][0|green]
void VDP_SetPalette(const u8* pal) __FASTCALL;

/// Set palette entry color
void VDP_SetPaletteEntry(u8 index, u16 color);

#define VDP_LINE_192			0			///< 192 lines mode
#define VDP_LINE_212			R09_LN		///< 212 lines mode
/// Set line count for the current screen mode
void VDP_SetLineCount(u8 lines) __FASTCALL;

/// Enable or disable interlace mode
void VDP_SetInterlace(bool enable) __FASTCALL;

/// Enable automatic page switch on even/odd frames
void VDP_SetPageAlternance(bool enable) __FASTCALL;

//-----------------------------------------------------------------------------
// SPRITES
//-----------------------------------------------------------------------------
/// Enable/disable sprite rendering
void VDP_EnableSprite(u8 enable) __FASTCALL;

#define VDP_SPRITE_SIZE_8		0			///< Use 8x8 sprite size
#define VDP_SPRITE_SIZE_16		R01_ST		///< Use 16x16 sprite size
#define VDP_SPRITE_SCALE_1		0			///> Normal size of the sprite (1 dot = 1 px)
#define VDP_SPRITE_SCALE_2		R01_MAG		///> Double the size of the sprite (1 dot = 2 px)
/// Set sprite parameters
void VDP_SetSpriteFlag(u8 flag) __FASTCALL;

/// Set sprite attribute table address
void VDP_SetSpriteAttributeTable(VADDR addr) __FASTCALL;

/// Set sprite pattern table address
void VDP_SetSpritePatternTable(VADDR addr) __FASTCALL;

/// Set sprite table address (bit#16 to bit#1)
void VDP_SetSpriteTables(VADDR patternAddr, VADDR attribAddr);

/// Load pattern data into VRAM
void VDP_LoadSpritePattern(const u8* addr, u8 index, u8 count);

#define VDP_SPRITE_EC			0x80		///> Early clock ; used to offset sprite by 32 dots to the left
#define VDP_SPRITE_CC			0x40		///> Sprite priority control
#define VDP_SPRITE_IC			0x20		///> Line collision detection
/// Set sprite attribute for Sprite Mode 1 (MSX1)
void VDP_SetSpriteSM1(u8 index, u8 x, u8 y, u8 shape, u8 color);

/// Set sprite attribute for Sprite Mode 2
void VDP_SetSprite(u8 index, u8 x, u8 y, u8 shape);

/// Set sprite attribute for Sprite Mode 2 and fill color table with color data
void VDP_SetSpriteExMultiColor(u8 index, u8 x, u8 y, u8 shape, const u8* ram);

/// Set sprite attribute for Sprite Mode 2 and fill color table with unique color
void VDP_SetSpriteExUniColor(u8 index, u8 x, u8 y, u8 shape, u8 color);

/// Update sprite position
void VDP_SetSpritePosition(u8 index, u8 x, u8 y);

/// Update sprite position Y
void VDP_SetSpritePositionY(u8 index, u8 y);

/// Update sprite pattern
void VDP_SetSpritePattern(u8 index, u8 shape);

/// Update sprite pattern (Shader mode 1)
void VDP_SetSpriteColorSM1(u8 index, u8 color);

/// Update sprite color (Uni-color)
void VDP_SetSpriteUniColor(u8 index, u8 color);

/// Update sprite color (Multi-color)
void VDP_SetSpriteMultiColor(u8 index, const u8* ram);

/// Set sprite data for Sprite Mode 2
void VDP_SetSpriteData(u8 index, const u8* data);

///
#define VDP_SPRITE_DISABLE_SM1	208			///> This sprite and all lower priority sprites will be disabled (Sprite Mode 1)
#define VDP_SPRITE_DISABLE_SM2	216			///> This sprite and all lower priority sprites will be disabled (Sprite Mode 1)
#define VDP_SPRITE_HIDE			213			///> 
void VDP_HideSpriteFrom(u8 index) __FASTCALL;

//-----------------------------------------------------------------------------
// GRAPH MODE 2
//-----------------------------------------------------------------------------
#if (USE_VDP_MODE_G2 || USE_VDP_MODE_G3)
///
void VDP_FillScreen_GM2(u8 value) __FASTCALL;
///
void VDP_LoadPattern_GM2(const u8* src, u8 count, u8 offset);
///
void VDP_LoadColor_GM2(const u8* src, u8 count, u8 offset);
///
void VDP_WriteLayout_GM2(const u8* src, u8 dx, u8 dy, u8 nx, u8 ny);
///
void VDP_FillLayout_GM2(u8 value, u8 dx, u8 dy, u8 nx, u8 ny);
#endif

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
void VDP_CommandWait();

/// Send VDP command (form registres 32 to 46)
void VPD_CommandSetupR32();

/// Send VDP command (form registres 36 to 46)
void VPD_CommandSetupR36();

/// Write to VRAM command loop
void VPD_CommandWriteLoop(const u8* addr) __FASTCALL;

/// Read to VRAM command loop
void VPD_CommandReadLoop(u8* addr) __FASTCALL;

//-----------------------------------------------------------------------------
// INLINE FUNCTIONS
//-----------------------------------------------------------------------------

#include "vdp.inl"

// DATA
// inline u8 VDP_GetMode();

// VDP COMMANDS
// inline void VDP_CommandHMMC(const u8* addr, u16 dx, u16 dy, u16 nx, u16 ny); // High speed move CPU to VRAM.
// inline void VDP_CommandYMMM(u16 sy, u16 dx, u16 dy, u16 ny, u8 dir); // High speed move VRAM to VRAM, Y coordinate only.
// inline void VDP_CommandHMMM(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny); // High speed move VRAM to VRAM
// inline void VDP_CommandHMMV(u16 dx, u16 dy, u16 nx, u16 ny, u8 col); // High speed move VDP to VRAM
// inline void VDP_CommandLMMC(); // Logical move CPU to VRAM
// inline void VDP_CommandLMCM(); // Logical move VRAM to CPU
// inline void VDP_CommandLMMM(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny, u8 op); // Logical move VRAM to VRAM
// inline void VDP_CommandLMMV(u16 dx, u16 dy, u16 nx, u16 ny, u8 col, u8 op); // Logical move VDP to VRAM
// inline void VDP_CommandLINE(u16 dx, u16 dy, u16 nx, u16 ny, u8 col, u8 arg, u8 op); // Draw straight line in VRAM
// inline void VDP_CommandSRCH(u16 sx, u16 sy, u8 col, u8 arg); // Search for the specific color in VRAM to the right or left of the starting point
// inline void VDP_CommandPSET(u16 dx, u16 dy, u8 col, u8 op); // Draw a dot in VRAM 
// inline void VDP_CommandPOINT(u16 sx, u16 sy); // Read the color of the specified dot located in VRAM 
// inline void VDP_CommandSTOP(); // Abort current command

#define VDP_CopyRAMtoVRAM			VDP_CommandHMMC		///< High speed move CPU to VRAM
#define VDP_YMoveVRAM				VDP_CommandYMMM		///< High speed move VRAM to VRAM, Y coordinate only
#define VDP_MoveVRAM				VDP_CommandHMMM		///< High speed move VRAM to VRAM
//#define VDP_FillVRAM				VDP_CommandHMMV		///< High speed move VDP to VRAM
#define VDP_LogicalCopyRAMtoVRAM	VDP_CommandLMMC		///< Logical move CPU to VRAM
#define VDP_LogicalYMoveVRAM		VDP_CommandLMCM		///< Logical move VRAM to CPU
#define VDP_LogicalMoveVRAM			VDP_CommandLMMM		///< Logical move VRAM to VRAM
#define VDP_LogicalFillVRAM			VDP_CommandLMMV		///< Logical move VDP to VRAM
#define VDP_DrawLine				VDP_CommandLINE		///< Draw straight line in VRAM
#define VDP_SearchColor				VDP_CommandSRCH		///< Search for the specific color in VRAM to the right or left of the starting point
#define VDP_DrawPoint				VDP_CommandPSET		///< Draw a dot in VRAM 
#define VDP_ReadPoint				VDP_CommandPOINT	///< Read the color of the specified dot located in VRAM 
#define VDP_AbortCommand			VDP_CommandSTOP		///< Abort current command
