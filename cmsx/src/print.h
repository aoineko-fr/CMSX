//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "core.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// INITIALIZATION
//-----------------------------------------------------------------------------

extern struct Print_Data g_PrintData;

// Functions
typedef void (*print_putchar)(u8) __FASTCALL; ///< Callback default signature

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
	print_putchar PutChar;		///< Put char function
	u16 ScreenWidth;			///< Screen width
#if USE_PRINT_SHADOW	
	u8 Shadow			: 1;	///< Is shadow render active
	u8 ShadowOffsetX	: 3;	///< Shadow X offset (0:7 => -3:+4)
	u8 ShadowOffsetY	: 3;	///< Shadow Y offset (0:7 => -3:+4)
	u8 ShadowColor;				///< Shadow color
#endif
	u8 Buffer[10];				///< Mode specifique buffer
};

/// Initialize print module
bool Print_Initialize(u8 screen, const u8* font);

/// Set the current font
void Print_SetFont(const u8* font) __FASTCALL;

/// Clear screen
void Print_Clear();

/// Set shadow effect
#if USE_PRINT_SHADOW	
void Print_SetShadow(bool activate, i8 offsetX, i8 offsetY, u8 color);
#endif

//-----------------------------------------------------------------------------
/// Set the current font
inline void Print_SetFontEx(u8 formX, u8 formY, u8 sizeX, u8 sizeY, u8 firstChr, u8 lastChr, const u8* forms)
{
	g_PrintData.FormX     = formX;
	g_PrintData.FormY     = formY;
	g_PrintData.UnitX     = sizeX;
	g_PrintData.UnitY     = sizeY;
	g_PrintData.FontFirst = firstChr;
	g_PrintData.FontLast  = lastChr;
	g_PrintData.FontForms = forms;
}

//-----------------------------------------------------------------------------
/// Set the draw color
inline void Print_SetColor(u8 text, u8 background)
{
	g_PrintData.TextColor = text;
	g_PrintData.BackgroundColor = background;

	if((g_PrintData.Mode == VDP_MODE_GRAPHIC4) || (g_PrintData.Mode == VDP_MODE_GRAPHIC6))
	{
		g_PrintData.Buffer[0] = g_PrintData.BackgroundColor << 4 | g_PrintData.BackgroundColor;
		g_PrintData.Buffer[1] = g_PrintData.BackgroundColor << 4 | g_PrintData.TextColor;
		g_PrintData.Buffer[2] = g_PrintData.TextColor << 4 | g_PrintData.BackgroundColor;
		g_PrintData.Buffer[3] = g_PrintData.TextColor << 4 | g_PrintData.TextColor;
	}
}

//-----------------------------------------------------------------------------
/// Set cursor position
inline void Print_SetPosition(u8 x, u8 y)
{
	g_PrintData.CursorX = x;
	g_PrintData.CursorY = y;	
}

//-----------------------------------------------------------------------------
/// Set cursor position
inline void Print_SetCharSize(u8 x, u8 y)
{
	g_PrintData.UnitX = x;
	g_PrintData.UnitY = y;	
}

//-----------------------------------------------------------------------------
/// Set tabulation size in pixel (must be a power of 2 like 16, 32, 64, ...)
inline void Print_SetTabSize(u8 size)
{
	g_PrintData.TabSize = size;
}

//-----------------------------------------------------------------------------
/// Print space
inline void Print_Space()
{
	g_PrintData.CursorX += g_PrintData.UnitX;
}

//-----------------------------------------------------------------------------
/// Print tabulation
inline void Print_Tab()
{
	g_PrintData.CursorX += g_PrintData.UnitX + g_PrintData.TabSize - 1;
	g_PrintData.CursorX &= ~(g_PrintData.TabSize - 1);
}

//-----------------------------------------------------------------------------
/// Print return
inline void Print_Return()
{
	g_PrintData.CursorX = 0;
	g_PrintData.CursorY += g_PrintData.UnitY;
}

//-----------------------------------------------------------------------------
// DRAW FUNCTION
//-----------------------------------------------------------------------------

/// 
void Print_DrawText(const c8* string) __FASTCALL;

/// 
void Print_DrawTextX(const c8* str, u8 num);

/// 
void Print_DrawChar(u8 chr) __FASTCALL;

/// 
void Print_DrawCharX(c8 chr, u8 num);

/// 
void Print_DrawHex8(u8 value) __FASTCALL;

/// 
void Print_DrawHex16(u16 value) __FASTCALL;

/// 
void Print_DrawBin8(u8 value) __FASTCALL;

/// 
void Print_DrawInt(i16 value) __FASTCALL;

