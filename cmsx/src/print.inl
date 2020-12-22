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
/// Set the draw color
/// @param		text		Text color (format depend of current screen mode)
/// @param		bg			Background color (format depend of current screen mode)
inline void Print_SetColor(u8 text, u8 bg)
{
	g_PrintData.TextColor = text;
	g_PrintData.BackgroundColor = bg;

	// Pre-compute colors combinaison for 2-bits of a character form line (used to quick drawing in PutChar_GX functions)
	if((g_PrintData.Mode == VDP_MODE_GRAPHIC4) || (g_PrintData.Mode == VDP_MODE_GRAPHIC6))
	{
		g_PrintData.Buffer[0] = (bg << 4)   | bg;
		g_PrintData.Buffer[1] = (bg << 4)   | text;
		g_PrintData.Buffer[2] = (text << 4) | bg;
		g_PrintData.Buffer[3] = (text << 4) | text;
	}
	else if(g_PrintData.Mode == VDP_MODE_GRAPHIC7)
	{
		((u16*)g_PrintData.Buffer)[0] = ((u16)bg << 8)   | (u16)bg;
		((u16*)g_PrintData.Buffer)[1] = ((u16)text << 8) | (u16)bg;
		((u16*)g_PrintData.Buffer)[2] = ((u16)bg << 8)   | (u16)text;
		((u16*)g_PrintData.Buffer)[3] = ((u16)text << 8) | (u16)text;
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
