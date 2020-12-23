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

extern struct Print_Data g_PrintData;

// Functions
typedef void (*print_putchar)(u8) __FASTCALL; ///< Callback default signature

//-----------------------------------------------------------------------------
// STRUCTURES
//-----------------------------------------------------------------------------

/// Print module configuration structure
struct Print_Data
{
	u8 Mode				: 4;	///< Screen mode (0-15)
	u8 Page				: 4;	///< The page to draw (0-15)
	u8 FormX;					///< X size of a character in screen unit (0-15)
	u8 FormY;					///< Y size of a character in screen unit (0-15)
	u8 UnitX;					///< X size of a character in screen unit (0-15)
	u8 UnitY;					///< Y size of a character in screen unit (0-15)
	u8 TabSize;					///< Tabulation size in pixel (must be a power of 2 like 16, 32, 64, ...)
	u16 CursorX;				///< Current X position (screen coordinate)
	u8 CursorY;					///< Current Y position (screen coordinate)
	u8 TextColor;				///< Text colot
	u8 BackgroundColor;			///< Background color
	u8 FontFirst;				///< ASCII code of the first character of the current font
	u8 FontLast;				///< ASCII code of the last character of the current font
	const u8* FontForms;		///< Forms of the font
	const u8* FontAddr;			///< 
	print_putchar PutChar;		///< Put char function
	u16 ScreenWidth;			///< Screen width
#if USE_PRINT_SHADOW	
	u8 Shadow			: 1;	///< Is shadow render active
	u8 ShadowOffsetX	: 3;	///< Shadow X offset (0:7 => -3:+4)
	u8 ShadowOffsetY	: 3;	///< Shadow Y offset (0:7 => -3:+4)
	u8 ShadowColor;				///< Shadow color
#endif
	u8 Buffer[20];				///< Mode specifique buffer
};

//-----------------------------------------------------------------------------
// INITIALIZATION
//-----------------------------------------------------------------------------

/// Initialize print module
bool Print_Initialize(u8 screen, const u8* font);

/// Set the current font
void Print_SetFont(const u8* font) __FASTCALL;

/// Clear screen
void Print_Clear();

/// Set the draw color
void Print_SetColor(u8 text, u8 bg);

/// Set shadow effect
#if USE_PRINT_SHADOW	
void Print_SetShadow(bool activate, i8 offsetX, i8 offsetY, u8 color);
#endif

//-----------------------------------------------------------------------------
// INLINE FUNCTIONS
//-----------------------------------------------------------------------------
// inline void Print_SetFontEx(u8 formX, u8 formY, u8 sizeX, u8 sizeY, u8 firstChr, u8 lastChr, const u8* forms); // Set the current font
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

