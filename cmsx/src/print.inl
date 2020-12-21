//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

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
