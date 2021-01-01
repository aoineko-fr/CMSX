//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
/// Set the current font
/// @param		formX		Width of the character data (should be always 8)
/// @param		formY		Height of the character data
/// @param		sizeX		Width of the chatacter to display (can be different than the one of the data)
/// @param		sizeY		Height of the chatacter to display (can be different than the one of the data)
/// @param		firstChr	ASCII code of the first character included in the font data
/// @param		lastChr		ASCII code of the last character included in the font data
/// @param		forms		Address of the character data table
inline void Print_SetFontEx(u8 formX, u8 formY, u8 sizeX, u8 sizeY, u8 firstChr, u8 lastChr, const u8* forms)
{
	g_PrintData.FormX     = formX;
	g_PrintData.FormY     = formY;
	g_PrintData.UnitX     = sizeX;
	g_PrintData.UnitY     = sizeY;
	g_PrintData.FontFirst = firstChr;
	g_PrintData.FontLast  = lastChr;
	g_PrintData.FontForms = forms;
	g_PrintData.FontAddr  = g_PrintData.FontForms - (g_PrintData.FontFirst * g_PrintData.FormY); // pre-compute address of the virtual index 0 character (used to quick drawing in PutChar_GX functions)
}

//-----------------------------------------------------------------------------
/// Set cursor position
inline void Print_SetPosition(u16 x, u8 y)
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
/// Set tabulation size in pixel
/// @param		size		Size of the tabulation in pixel (must be a power of 2 like 16, 32, 64, ...)
inline void Print_SetTabSize(u8 size)
{
	g_PrintData.TabSize = size;
}

//-----------------------------------------------------------------------------
/// Print space
inline void Print_Space()
{
	g_PrintData.CursorX += PRINT_W(g_PrintData.UnitX);
}

//-----------------------------------------------------------------------------
/// Print tabulation
inline void Print_Tab()
{
	g_PrintData.CursorX += PRINT_W(g_PrintData.UnitX) + g_PrintData.TabSize - 1;
	g_PrintData.CursorX &= ~(g_PrintData.TabSize - 1);
}

//-----------------------------------------------------------------------------
/// Print return
inline void Print_Return()
{
	g_PrintData.CursorX = 0;
	g_PrintData.CursorY += PRINT_H(g_PrintData.UnitY);
}
