//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "bios_main.h"
#include "vdp.h"
#include "print.h"
#include "memory.h"
#include "math.h"

struct Print_Data g_PrintData;

#if (MSX_VERSION >= MSX_2)
void PutChar_G4(u8 chr) __FASTCALL;
void PutChar_G6(u8 chr) __FASTCALL;
void PutChar_G7(u8 chr) __FASTCALL;
#endif

#if USE_PRINT_VALIDATOR
u8 g_PrintInvalid[] =
{
	0xFF, /* ######## */ 
	0x81, /* #......# */ 
	0x81, /* #......# */ 
	0x81, /* #......# */ 
	0x81, /* #......# */ 
	0x81, /* #......# */ 
	0x81, /* #......# */ 
	0xFF, /* ######## */ 
};
#endif

static const c8 hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

//-----------------------------------------------------------------------------
//
// INITIALIZATION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initialize print module
bool Print_Initialize(u8 screen, const u8* font)
{
	g_PrintData.Mode = screen; // must be set first because used in other functions (like SetColor)
	g_PrintData.Page = 0;
	g_PrintData.TabSize = 32;

	Print_SetFont(font);
	Print_SetColor(0xF, 0x0);
	Print_SetPosition(0, 0);
#if USE_PRINT_SHADOW
	Print_SetShadow(false, 0, 0, 0);
#endif

	switch(screen)
	{
#if (MSX_VERSION >= MSX_2)
	case VDP_MODE_GRAPHIC4:		// 256 x 212; 16 colours are available for each dot
		g_PrintData.PutChar     = PutChar_G4;
		g_PrintData.ScreenWidth = 256;
		break;
	case VDP_MODE_GRAPHIC6:		// 512 x 212; 16 colours are available for each dot
		g_PrintData.PutChar     = PutChar_G6;
		g_PrintData.ScreenWidth = 512;
		break;
	case VDP_MODE_GRAPHIC7:		// 256 x 212; 256 colours are available for each dot
		Print_SetColor(0xFF, 0x0);
		g_PrintData.PutChar     = PutChar_G7;
		g_PrintData.ScreenWidth = 256;
		break;
#endif // (MSX_VERSION >= MSX_2)
	default:
		// Screen mode not (yet) supported!
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
/// Set the current font
void Print_SetFont(const u8* font) __FASTCALL
{
	if(font == null) // Use Bios font (if any)
		Print_SetFontEx(8, 8, 6, 8, 0, 255, (const u8*)g_CGTABL);
	else
		Print_SetFontEx(font[0] >> 4, font[0] & 0x0F, font[1] >> 4, font[1] & 0x0F, font[2], font[3], font+4);
}

//-----------------------------------------------------------------------------
/// Clear screen
void Print_Clear()
{
	VDP_HMMV(0, 0, g_PrintData.ScreenWidth, 256, g_PrintData.BackgroundColor);
}

//-----------------------------------------------------------------------------
/// Set shadow effect
#if USE_PRINT_SHADOW	
void Print_SetShadow(bool activate, i8 offsetX, i8 offsetY, u8 color)
{
	g_PrintData.Shadow        = activate;
	g_PrintData.ShadowOffsetX = 3 + offsetX; // Math_Clamp(offsetX, (i8)-3, 4);
	g_PrintData.ShadowOffsetY = 3 + offsetY; // Math_Clamp(offsetY, (i8)-3, 4);
	g_PrintData.ShadowColor   = color;
}
#endif // USE_PRINT_SHADOW	

//-----------------------------------------------------------------------------
//
// SCREEN MODE FUNCTION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
///
#if USE_PRINT_VALIDATOR
void Print_ValidateForm(u8* chr, const c8** form)
{
	if((*chr < g_PrintData.FontFirst) || (*chr > g_PrintData.FontLast))
	{
		if((*chr >= 'a') && (*chr <= 'z') && (g_PrintData.FontFirst <= 'A') && (g_PrintData.FontLast >= 'Z')) // try to remap to upper case letters
		{
			*chr = *chr - 'a' + 'A';
			*form = g_PrintData.FontForms + g_PrintData.FormY * (*chr - g_PrintData.FontFirst);
		}
		else
			*form = g_PrintInvalid;
	}
}
#endif // USE_PRINT_VALIDATOR

#if (MSX_VERSION >= MSX_2)

//-----------------------------------------------------------------------------
///
void PutChar_G4(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontForms + g_PrintData.FormY * (chr - g_PrintData.FontFirst);
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	for(i8 j = 0; j < g_PrintData.FormY; j++) // lines
	{
		u8 f = form[j];
		u8* l = &g_PrintData.Buffer[4];
		*l++ = g_PrintData.Buffer[f >> 6];
		*l++ = g_PrintData.Buffer[(f >> 4) & 0x03];
		*l++ = g_PrintData.Buffer[(f >> 2) & 0x03];
		*l   = g_PrintData.Buffer[f & 0x03];
		VDP_WriteVRAM(&g_PrintData.Buffer[4], ((g_PrintData.CursorY + j) * 128) + (g_PrintData.CursorX >> 1), 0, 4);
	}
}

//-----------------------------------------------------------------------------
///
void PutChar_G6(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontForms + g_PrintData.FormY * (chr - g_PrintData.FontFirst);
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	for(i8 j = 0; j < g_PrintData.FormY; j++) // lines
	{
		u8 f = form[j];
		u8* l = &g_PrintData.Buffer[4];
		*l++ = g_PrintData.Buffer[f >> 6];
		*l++ = g_PrintData.Buffer[(f >> 4) & 0x03];
		*l++ = g_PrintData.Buffer[(f >> 2) & 0x03];
		*l   = g_PrintData.Buffer[f & 0x03];
		VDP_WriteVRAM(&g_PrintData.Buffer[4], ((g_PrintData.CursorY + j) * 256) + (g_PrintData.CursorX >> 1), 0, 4);
	}
}

//-----------------------------------------------------------------------------
///
void PutChar_G7(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontForms + g_PrintData.FormY * (chr - g_PrintData.FontFirst);
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	for(i8 j = 0; j < g_PrintData.FormY; j++) // lines
	{
		for(i8 i = 0; i < g_PrintData.FormX; i++)
		{
			if(form[j] & (1 << (7 - i)))
				g_PrintData.Buffer[i] = g_PrintData.TextColor;
			else
				g_PrintData.Buffer[i] = g_PrintData.BackgroundColor;
		}
		VDP_WriteVRAM(g_PrintData.Buffer, ((g_PrintData.CursorY + j) * 256) + g_PrintData.CursorX, 0, g_PrintData.FormX);
	}
}

#endif // (MSX_VERSION >= MSX_2)

//-----------------------------------------------------------------------------
//
// DRAW FUNCTION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
///
void Print_DrawChar(u8 chr) __FASTCALL
{
#if USE_PRINT_SHADOW
	if(g_PrintData.Shadow)
	{
		// Backup
		u8 cx = g_PrintData.CursorX;
		u8 cy = g_PrintData.CursorY;
		u8 tc = g_PrintData.TextColor;
		// Set
		g_PrintData.CursorX += g_PrintData.ShadowOffsetX - 3;
		g_PrintData.CursorY += g_PrintData.ShadowOffsetY - 3;
		g_PrintData.TextColor = g_PrintData.ShadowColor;
		g_PrintData.PutChar(chr);
		// Restore
		g_PrintData.CursorX = cx;
		g_PrintData.CursorY = cy;
		g_PrintData.TextColor = tc;
	}
#endif
#if USE_PRINT_VALIDATOR
	if(g_PrintData.CursorX + g_PrintData.UnitX > g_PrintData.ScreenWidth)
		Print_Return();
#endif
	g_PrintData.PutChar(chr);
	g_PrintData.CursorX += g_PrintData.UnitX;
}

//-----------------------------------------------------------------------------
///
void Print_DrawCharX(c8 chr, u8 num)
{
	for(i8 i = 0; i < num; i++)
		Print_DrawChar(chr);
}

//-----------------------------------------------------------------------------
///
void Print_DrawText(const c8* str) __FASTCALL
{
	while(*str != 0)
	{
		if(*str == ' ')
			Print_Space();
		else if(*str == '\t')
			Print_Tab();
		else if(*str == '\n')
			Print_Return();
		else
			Print_DrawChar(*str);
		str++;
	}
}

//-----------------------------------------------------------------------------
///
void Print_DrawTextX(const c8* str, u8 num)
{
	for(i8 i = 0; i < num; i++)
		Print_DrawText(str);	
}

//-----------------------------------------------------------------------------
///
void Print_DrawBin8(u8 value) __FASTCALL
{
	for(i8 i = 0; i < 8; i++)
	{
		if(value & (1 << (7 - i)))
			Print_DrawChar('1');
		else
			Print_DrawChar('0');
	}
	Print_DrawChar('b');
}

//-----------------------------------------------------------------------------
///
void Print_DrawHex8(u8 value) __FASTCALL
{
	Print_DrawChar(hexChar[(value >> 4) & 0x000F]);
	Print_DrawChar(hexChar[value & 0x000F]);
	Print_DrawChar('h');
}

//-----------------------------------------------------------------------------
///
void Print_DrawHex16(u16 value) __FASTCALL
{
	Print_DrawChar(hexChar[(value >> 12) & 0x000F]);
	Print_DrawChar(hexChar[(value >> 8) & 0x000F]);
	Print_DrawHex8((u8)value);
}

//-----------------------------------------------------------------------------
///
void Print_DrawInt(i16 value) __FASTCALL
{
	if(value < 0)
	{	
		Print_DrawChar('-');
		value = -value;
	}
	
	c8 str[8];
	c8* ptr = str;
	*ptr = 0;
	while(value >= 10)
	{
		*++ptr = '0' + (value % 10);
		value /= 10;
	}
	*++ptr = '0' + value;
	while(*ptr != 0)
		Print_DrawChar(*ptr--);	
}
