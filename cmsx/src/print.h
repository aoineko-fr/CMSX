//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "core.h"

//-----------------------------------------------------------------------------
// INITIALIZATION
//-----------------------------------------------------------------------------

/// Character display mode
enum PRINT_MODE
{
	PRINT_MODE_TEXT = 0,
	PRINT_MODE_GRAPH,
};

/// Print module configuration structure
struct PrintConfig
{
	u8 Mode;				///< Render mode (0-3)
	u8 Page;				///< The page to draw (0-3)
	u8 Bpp;					///< Number of bits-per-pixel (0-15)
	u8 UnitX;				///< X size of a character in screen unit (0-15)
	u8 UnitY;				///< Y size of a character in screen unit (0-15)
	u8 TextColor;			///< Text colot
	u8 BackgroundColor;		///< Background color
	u8 FontSize;			///< Current font size in pixels [x|y]
	u8 FontFirst;			///< ASCII code of the first character of the current font
	u8 FontLast;			///< ASCII code of the last character of the current font
	const u8* FontForms;	///< Forms of the font
};

/// Initialize print module
bool Print_Initialize(u8 screen, const u8* font);

/// Set the current font
void Print_SetFont(const u8* font) __FASTCALL;

/// Set the draw color
void Print_SetColor(u8 text, u8 background);

//-----------------------------------------------------------------------------
// DRAW FUNCTION
//-----------------------------------------------------------------------------

/// 
void Print_DrawText(u8 x, u8 y, const c8* string);


inline u8 IsTextScreen();
void PrintInit(u8 color);
void SetPrintPos(u8 x, u8 y);
void PrintReturn();
void PrintTab();
void PrintChar(c8 chr);
void PrintCharX(c8 chr, u8 num);
void PrintText(const c8* str);
void PrintTextX(const c8* str, u8 num);
void PrintHex8(u8 value);
void PrintHex16(u16 value);
void PrintBin8(u8 value);
void PrintInt(i16 value);
void PrintSlot(u8 slot);

void PrintLineX(u8 x, u8 y, u8 len);
void PrintLineY(u8 x, u8 y, u8 len);
void PrintBox(u8 sx, u8 sy, u8 dx, u8 dy);


/*
// Draw a text in graphical mode at given position
void PrintString(const c8* string, u8 x, u8 y, u8 color, u8 scr);

void PrintCharGraph(u8 chr, u8 x, u8 y, u8 color);
void PrintCharText(u8 chr, u8 x, u8 y, u8 color);
*/