//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once
#include "core.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

/// Character display sources
enum PRINT_MODE
{
	PRINT_MODE_DEFAULT,			///< Draw characters from RAM (R-T unpack font data and draw it)
	PRINT_MODE_TRANSPARENT,		///< Draw characters from RAM (R-T unpack font data and draw it)
#if (USE_PRINT_VRAM)
	PRINT_MODE_VRAM,			///< Draw characters from VRAM (font data is upack once in VRAM thne drawing is done by VRAM copy)
#endif
#if (USE_PRINT_SPRITE)
	PRINT_MODE_SPRITE,			///< Draw characters from sprites (load font data as sprite pattern in VRAM then display characters using sprite system)
#endif
};

// Handle fixed of variables character width
#if (PRINT_WIDTH == PRINT_WIDTH_6)
	#define PRINT_W(a) 6
#elif (PRINT_WIDTH == PRINT_WIDTH_8)
	#define PRINT_W(a) 8
#else // (PRINT_WIDTH == PRINT_WIDTH_X)
	#define PRINT_W(a) (a)
#endif

// Handle fixed of variables character height
#if (PRINT_HEIGHT == PRINT_HEIGHT_8)
	#define PRINT_H(a) 8
#else // (PRINT_HEIGHT == PRINT_HEIGHT_X)
	#define PRINT_H(a) a
#endif

// Functions
typedef void (*print_drawchar)(u8) __FASTCALL; ///< Draw char callback signature
typedef void (*print_loadfont)(VADDR) __FASTCALL; ///< Font load callback signature

extern struct Print_Data g_PrintData;

//-----------------------------------------------------------------------------
// STRUCTURES
//-----------------------------------------------------------------------------

/// Print module configuration structure
struct Print_Data
{
	u8 PatternX;				///< X size of a character in screen unit (0-15)
	u8 PatternY;				///< Y size of a character in screen unit (0-15)
	u8 UnitX;					///< X size of a character in screen unit (0-15)
	u8 UnitY;					///< Y size of a character in screen unit (0-15)
	u8 TabSize;					///< Tabulation size in pixel (must be a power of 2 like 16, 32, 64, ...)
	UX CursorX;					///< Current X position (screen coordinate)
	UY CursorY;					///< Current Y position (screen coordinate)
#if (PRINT_COLOR_NUM == 1)
	u8 TextColor;				///< Text color
#else // (PRINT_COLOR_NUM > 1)
	u8 TextColor[PRINT_COLOR_NUM]; ///< Text colors (1 per line)
#endif
	u8 BGColor;					///< Background color
	u8 FontFirst;				///< ASCII code of the first character of the current font
	u8 FontLast;				///< ASCII code of the last character of the current font
	print_drawchar DrawChar;	///< Default function to draw a character (depend of the current mode)
	u8 SourceMode       : 4;	///< Character display mode : RAM, VRAM or Sprite (@see PRINT_MODE)
	// RAM
	const u8* FontPatterns;		///< Forms of the font
	const u8* FontAddr;			///< Address of the virtual index 0 character (used to quick drawing in DrawChar_GX functions)
	u16 ScreenWidth;			///< Screen width
#if (USE_PRINT_VRAM)
	// VRAM
	UY FontVRAMY;				///< Y position of the font in VRAM
	// u16 FormsPerLine;			///< Pre-compute number of characters per line
#endif
#if (USE_PRINT_SPRITE)
	// Sprites
	u8 SpritePattern;			///< Pattern index of the 1st sprite character
	u8 SpriteID;				///< Index of the current sprite
#endif
#if (USE_PRINT_FX_SHADOW)
	u8 Shadow			: 1;	///< Is shadow render active
	u8 ShadowOffsetX	: 3;	///< Shadow X offset (0:7 => -3:+4)
	u8 ShadowOffsetY	: 3;	///< Shadow Y offset (0:7 => -3:+4)
	u8 ShadowColor;				///< Shadow color
#endif
#if (USE_PRINT_FX_OUTLINE)
	u8 Outline			: 1;	///< Is shadow render active
	u8 OutlineColor;			///< Shadow color
#endif
	u8 Buffer[16];				///< Mode specifique buffer (used to pre-compute color combinations)
};

//-----------------------------------------------------------------------------
// INITIALIZATION
//-----------------------------------------------------------------------------

/// Initialize print module and set a font in RAM
bool Print_Initialize(const u8* font);

///
void Print_SetMode(u8 src) __FASTCALL;

/// Set the current font (and set mode to RAM)
void Print_SetFont(const u8* font) __FASTCALL;

#if (USE_PRINT_VRAM)
/// Set the current font and upload data to VRAM 
void Print_SetFontVRAM(const u8* font, UY y);
#endif

#if (USE_PRINT_SPRITE)
/// Set the current font and upload to Sprite Pattern Table
void Print_SetFontSprite(const u8* font, u8 patIdx, u8 sprtIdx);
#endif

/// Clear screen
void Print_Clear();

/// Clear X character back from current cursor position
void Print_Backspace(u8 num);

/// Set the draw color
void Print_SetColor(u8 text, u8 bg);

#if (PRINT_COLOR_NUM > 1)
/// Set color shade
void Print_SetColorShade(const u8* shade) __FASTCALL;
#endif

#if (USE_PRINT_FX_SHADOW)	
/// Set shadow effect
void Print_SetShadow(bool enable, i8 offsetX, i8 offsetY, u8 color);
/// Activate/desactivate shadow effect
void Print_EnableShadow(bool enable) __FASTCALL;
#endif

#if (USE_PRINT_FX_OUTLINE)	
/// Set shadow effect
void Print_SetOutline(bool enable, u8 color);
/// Activate/desactivate shadow effect
void Print_EnableOutline(bool enable) __FASTCALL;
#endif

//-----------------------------------------------------------------------------
// INLINE FUNCTIONS
//-----------------------------------------------------------------------------
// inline void Print_SetFontEx(u8 patternX, u8 patternY, u8 sizeX, u8 sizeY, u8 firstChr, u8 lastChr, const u8* patterns); // Set the current font
// inline void Print_SetPosition(u8 x, u8 y); // Set cursor position
// inline void Print_SetCharSize(u8 x, u8 y); // Set cursor position
// inline void Print_SetTabSize(u8 size) // Set tabulation size in pixel (must be a power of 2 like 16, 32, 64, ...);
// inline void Print_Space(); // Print space
// inline void Print_Tab(); // Print tabulation
// inline void Print_Return(); // Print return
#include "print.inl"

//-----------------------------------------------------------------------------
// DRAW FUNCTION
//-----------------------------------------------------------------------------

/// Print a single character 
void Print_DrawChar(u8 chr) __FASTCALL;

/// Print the same character many times
void Print_DrawCharX(c8 chr, u8 num);

/// Print a character string
void Print_DrawText(const c8* string) __FASTCALL;

/// Print a character string many times
void Print_DrawTextX(const c8* str, u8 num);

/// Print a 8-bits binary value 
void Print_DrawHex8(u8 value) __FASTCALL;

/// Print a 8-bits hexadecimal value
void Print_DrawHex16(u16 value) __FASTCALL;

/// Print a 16-bits hexadecimal value
void Print_DrawBin8(u8 value) __FASTCALL;

/// Print a 16-bits signed decimal value
void Print_DrawInt(i16 value) __FASTCALL;

