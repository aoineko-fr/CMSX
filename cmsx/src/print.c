//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "bios_mainrom.h"
#include "vdp.h"
#include "print.h"
#include "memory.h"
#include "math.h"

//-----------------------------------------------------------------------------
//
// DEFINE
//
//-----------------------------------------------------------------------------

#if USE_VDP_MODE_G4
void PutChar_G4(u8 chr) __FASTCALL;
#endif
#if USE_VDP_MODE_G5
void PutChar_G5(u8 chr) __FASTCALL;
#endif
#if USE_VDP_MODE_G6
void PutChar_G6(u8 chr) __FASTCALL;
#endif
#if USE_VDP_MODE_G7
void PutChar_G7(u8 chr) __FASTCALL;
#endif

//-----------------------------------------------------------------------------
//
// DATA
//
//-----------------------------------------------------------------------------

/// Allocate memory for Print module data structure
struct Print_Data g_PrintData;

#if USE_PRINT_VALIDATOR
/// Character use by character validator to show invalid character
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

/// Table use to quick decimal-to-hexadecimal conversion
static const c8 hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

//-----------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//-----------------------------------------------------------------------------

u8 Print_SplitColor(u8 color) __FASTCALL
{
	switch(g_PrintData.Mode)
	{
#if (USE_VDP_MODE_G4)
	case VDP_MODE_GRAPHIC4: return color & 0x0F;
#endif
#if (USE_VDP_MODE_G5)
	case VDP_MODE_GRAPHIC5: return color & 0x03;
#endif
#if (USE_VDP_MODE_G6)
	case VDP_MODE_GRAPHIC6: return color & 0x0F;
#endif
#if (USE_VDP_MODE_G7)
	case VDP_MODE_GRAPHIC7: return color;
#endif
	}
}

u8 Print_MergeColor(u8 color) __FASTCALL
{
	switch(g_PrintData.Mode)
	{
#if (USE_VDP_MODE_G4)
	case VDP_MODE_GRAPHIC4: return (color & 0x0F) << 4 | (color & 0x0F);
#endif
#if (USE_VDP_MODE_G5)
	case VDP_MODE_GRAPHIC5: return (color & 0x03) << 6 + (color & 0x03) << 4 + (color & 0x03) << 2 + (color & 0x03);
#endif
#if (USE_VDP_MODE_G6)
	case VDP_MODE_GRAPHIC6: return (color & 0x0F) << 4 | (color & 0x0F);
#endif
#if (USE_VDP_MODE_G7)
	case VDP_MODE_GRAPHIC7: return color;
#endif
	}
}

//-----------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initialize print module
/// @param		screen		The current screen mode. @see VDP_MODE enum
/// @param		font		Pointer to font data to use (null=use Main-ROM font)
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

	switch(screen) // Screen mode specific initialization
	{
#if USE_VDP_MODE_G4
	case VDP_MODE_GRAPHIC4:		// 256 x 212; 16 colours are available for each dot
		g_PrintData.PutChar     = PutChar_G4;
		g_PrintData.ScreenWidth = 256;
		break;
#endif // USE_VDP_MODE_G4
#if USE_VDP_MODE_G5
	case VDP_MODE_GRAPHIC5:		// 512 x 212; 4 colours are available for each dot
		Print_SetColor(0x03, 0x0);
		g_PrintData.PutChar     = PutChar_G5;
		g_PrintData.ScreenWidth = 512;
		break;
#endif // USE_VDP_MODE_G5
#if USE_VDP_MODE_G6
	case VDP_MODE_GRAPHIC6:		// 512 x 212; 16 colours are available for each dot
		g_PrintData.PutChar     = PutChar_G6;
		g_PrintData.ScreenWidth = 512;
		break;
#endif // USE_VDP_MODE_G6
#if USE_VDP_MODE_G7
	case VDP_MODE_GRAPHIC7:		// 256 x 212; 256 colours are available for each dot
		Print_SetColor(0xFF, 0x0);
		g_PrintData.PutChar     = PutChar_G7;
		g_PrintData.ScreenWidth = 256;
		break;
#endif // USE_VDP_MODE_G7
	default:
		// Screen mode not (yet) supported!
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
/// Set the current font
/// @param		font		Pointer to font data to use (null=use Main-ROM font)
void Print_SetFont(const u8* font) __FASTCALL
{
	if(font == null) // Use Bios font (if any)
		Print_SetFontEx(8, 8, 6, 8, 0, 255, (const u8*)g_CGTABL);
	else
		Print_SetFontEx(font[0] >> 4, font[0] & 0x0F, font[1] >> 4, font[1] & 0x0F, font[2], font[3], font+4);
}

//-----------------------------------------------------------------------------
/// Clear screen on the current page
void Print_Clear()
{
	u8 color = Print_MergeColor(g_PrintData.BackgroundColor);
	VDP_CommandHMMV(0, g_PrintData.Page * 256, g_PrintData.ScreenWidth, 212, color); // @todo Check the 192/212 lines parameter
	VDP_CommandWait();
}

//-----------------------------------------------------------------------------
/// Set the draw color
/// @param		text		Text color (format depend of current screen mode)
/// @param		bg			Background color (format depend of current screen mode)
void Print_SetColor(u8 text, u8 bg)
{
	register u8 t = text;
	register u8 b = bg;
	
#if USE_PRINT_VALIDATOR
	t = Print_SplitColor(t);
	b = Print_SplitColor(b);
#endif

	g_PrintData.TextColor = t;
	g_PrintData.BackgroundColor = b;

	// Pre-compute colors combinaison for 2-bits of a character form line (used to quick drawing in PutChar_GX functions)
	switch(g_PrintData.Mode)
	{
#if (USE_VDP_MODE_G4)
	case VDP_MODE_GRAPHIC4:
#endif
#if (USE_VDP_MODE_G6)
	case VDP_MODE_GRAPHIC6:
#endif
#if (USE_VDP_MODE_G4 || USE_VDP_MODE_G6)
		g_PrintData.Buffer[0] = (b << 4) | b;	// [ 0, 0 ]
		g_PrintData.Buffer[1] = (b << 4) | t;	// [ 0, 1 ]
		g_PrintData.Buffer[2] = (t << 4) | b;	// [ 1, 0 ]
		g_PrintData.Buffer[3] = (t << 4) | t;	// [ 1, 1 ]
		break;
#endif
#if (USE_VDP_MODE_G5)
	case VDP_MODE_GRAPHIC5:
		g_PrintData.Buffer[ 0] = (b << 6) | (b << 4) | (b << 2) | b;	// [ 0, 0, 0, 0 ]
		g_PrintData.Buffer[ 1] = (b << 6) | (b << 4) | (b << 2) | t;	// [ 0, 0, 0, 1 ]
		g_PrintData.Buffer[ 2] = (b << 6) | (b << 4) | (t << 2) | b;	// [ 0, 0, 1, 0 ]
		g_PrintData.Buffer[ 3] = (b << 6) | (b << 4) | (t << 2) | t;	// [ 0, 0, 1, 1 ]
		g_PrintData.Buffer[ 4] = (b << 6) | (t << 4) | (b << 2) | b;	// [ 0, 1, 0, 0 ]
		g_PrintData.Buffer[ 5] = (b << 6) | (t << 4) | (b << 2) | t;	// [ 0, 1, 0, 1 ]
		g_PrintData.Buffer[ 6] = (b << 6) | (t << 4) | (t << 2) | b;	// [ 0, 1, 1, 0 ]
		g_PrintData.Buffer[ 7] = (b << 6) | (t << 4) | (t << 2) | t;	// [ 0, 1, 1, 1 ]
		g_PrintData.Buffer[ 8] = (t << 6) | (b << 4) | (b << 2) | b;	// [ 1, 0, 0, 0 ]
		g_PrintData.Buffer[ 9] = (t << 6) | (b << 4) | (b << 2) | t;	// [ 1, 0, 0, 1 ]
		g_PrintData.Buffer[10] = (t << 6) | (b << 4) | (t << 2) | b;	// [ 1, 0, 1, 0 ]
		g_PrintData.Buffer[11] = (t << 6) | (b << 4) | (t << 2) | t;	// [ 1, 0, 1, 1 ]
		g_PrintData.Buffer[12] = (t << 6) | (t << 4) | (b << 2) | b;	// [ 1, 1, 0, 0 ]
		g_PrintData.Buffer[13] = (t << 6) | (t << 4) | (b << 2) | t;	// [ 1, 1, 0, 1 ]
		g_PrintData.Buffer[14] = (t << 6) | (t << 4) | (t << 2) | b;	// [ 1, 1, 1, 0 ]
		g_PrintData.Buffer[15] = (t << 6) | (t << 4) | (t << 2) | t;	// [ 1, 1, 1, 1 ]
		break;
#endif
#if (USE_VDP_MODE_G7)
	case VDP_MODE_GRAPHIC7:
		g_PrintData.Buffer[0] = b;	// [ 0, 0 ]
		g_PrintData.Buffer[1] = b;
		g_PrintData.Buffer[2] = b;	// [ 0, 1 ]
		g_PrintData.Buffer[3] = t;
		g_PrintData.Buffer[4] = t;	// [ 1, 0 ]
		g_PrintData.Buffer[5] = b;
		g_PrintData.Buffer[6] = t;	// [ 1, 1 ]
		g_PrintData.Buffer[7] = t;
		break;
#endif
	}
}

#if USE_PRINT_SHADOW	
//-----------------------------------------------------------------------------
/// Set shadow effect
/// @param		activate	Activate/deactivate shadow
/// @param		offsetX		Shadow offset on X axis (can be from -3 to +4)
/// @param		offsetY		Shadow offset on Y axis (can be from -3 to +4)
/// @param		color		Shadow color (depend of the screen mode)
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
// SCREEN MODE FUNCTIONS
//
//-----------------------------------------------------------------------------

#if USE_PRINT_VALIDATOR
//-----------------------------------------------------------------------------
/// Validate character. Try to convert invalid letter to their upper/lower case conterpart or use default invalid character
/// @param		chr			Address of the character to check
/// @param		form		Address of the font data to check
void Print_ValidateForm(u8* chr, const c8** form)
{
	if((*chr < g_PrintData.FontFirst) || (*chr > g_PrintData.FontLast))
	{
		if((*chr >= 'a') && (*chr <= 'z') && (g_PrintData.FontFirst <= 'A') && (g_PrintData.FontLast >= 'Z')) // try to remap to upper case letter
		{
			*chr = *chr - 'a' + 'A';
			*form = g_PrintData.FontForms + g_PrintData.FormY * (*chr - g_PrintData.FontFirst);
		}
		else if((*chr >= 'A') && (*chr <= 'Z') && (g_PrintData.FontFirst <= 'a') && (g_PrintData.FontLast >= 'z')) // try to remap to lower case letter
		{
			*chr = *chr - 'A' + 'a';
			*form = g_PrintData.FontForms + g_PrintData.FormY * (*chr - g_PrintData.FontFirst);
		}
		else
			*form = g_PrintInvalid;
	}
}
#endif // USE_PRINT_VALIDATOR

#if USE_VDP_MODE_G4
//-----------------------------------------------------------------------------
/// Graphic 4 (Screen mode 5) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void PutChar_G4(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.FormY); // Get character form's base address
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	u16 addr = (g_PrintData.CursorY * 128) + (g_PrintData.CursorX >> 1); // Get VRAM destination base address
	for(u8 j = 0; j < PRINT_H(g_PrintData.FormY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		u8 f = form[j];
		u8* l = &g_PrintData.Buffer[4];
		  *l = g_PrintData.Buffer[f >> 6];
		*++l = g_PrintData.Buffer[(f >> 4) & 0x03];
		*++l = g_PrintData.Buffer[(f >> 2) & 0x03];
#if (PRINT_WIDTH == PRINT_WIDTH_6)
		VDP_WriteVRAM(&g_PrintData.Buffer[4], addr, 0, 3);
#else
		*++l = g_PrintData.Buffer[f & 0x03];
		VDP_WriteVRAM(&g_PrintData.Buffer[4], addr, 0, 4);
#endif
		addr += 128;
	}
}
#endif // USE_VDP_MODE_G4

#if USE_VDP_MODE_G5
//-----------------------------------------------------------------------------
/// Graphic 5 (Screen mode 6) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void PutChar_G5(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.FormY); // Get character form's base address
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	u16 addr = (g_PrintData.CursorY * 128) + (g_PrintData.CursorX >> 2); // Get VRAM destination base address
	for(u8 j = 0; j < PRINT_H(g_PrintData.FormY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		u8 f = form[j];
		u8* l = &g_PrintData.Buffer[16];
		  *l = g_PrintData.Buffer[f >> 4];
		*++l = g_PrintData.Buffer[f & 0x0F];
		VDP_WriteVRAM(&g_PrintData.Buffer[16], addr, 0, 2);
		addr += 128;
	}
}
#endif // USE_VDP_MODE_G5

#if USE_VDP_MODE_G6
//-----------------------------------------------------------------------------
/// Graphic 6 (Screen mode 7) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void PutChar_G6(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.FormY); // Get character form's base address
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	u16 addr = (g_PrintData.CursorY * 256) + (g_PrintData.CursorX >> 1); // Get VRAM destination base address
	for(u8 j = 0; j < PRINT_H(g_PrintData.FormY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		u8 f = form[j];
		u8* l = &g_PrintData.Buffer[4];
		  *l = g_PrintData.Buffer[f >> 6];
		*++l = g_PrintData.Buffer[(f >> 4) & 0x03];
		*++l = g_PrintData.Buffer[(f >> 2) & 0x03];
#if (PRINT_WIDTH == PRINT_WIDTH_6)
		VDP_WriteVRAM(&g_PrintData.Buffer[4], addr, 0, 3);
#else
		*++l = g_PrintData.Buffer[f & 0x03];
		VDP_WriteVRAM(&g_PrintData.Buffer[4], addr, 0, 4);
#endif
		addr += 256;
	}
}
#endif // USE_VDP_MODE_G6

#if USE_VDP_MODE_G7
//-----------------------------------------------------------------------------
/// Graphic 7 (Screen mode 8) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void PutChar_G7(u8 chr) __FASTCALL
{
	const u8* form = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.FormY); // Get character form's base address
#if USE_PRINT_VALIDATOR
	Print_ValidateForm(&chr, &form);
#endif
	u16 addr = (g_PrintData.CursorY * 256) + g_PrintData.CursorX; // Get VRAM destination base address
	for(u8 j = 0; j < PRINT_H(g_PrintData.FormY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		u8 f = form[j];
		u16* l = &g_PrintData.Buffer[8];
		  *l = ((u16*)g_PrintData.Buffer)[f >> 6];
		*++l = ((u16*)g_PrintData.Buffer)[(f >> 4) & 0x03];
		*++l = ((u16*)g_PrintData.Buffer)[(f >> 2) & 0x03];
#if (PRINT_WIDTH == PRINT_WIDTH_6)
		VDP_WriteVRAM(&g_PrintData.Buffer[8], addr, 0, 6);
#else
		*++l = ((u16*)g_PrintData.Buffer)[f & 0x03];
		VDP_WriteVRAM(&g_PrintData.Buffer[8], addr, 0, 8);
#endif
		addr += 256;
	}
}
#endif // USE_VDP_MODE_G7

//-----------------------------------------------------------------------------
//
// DRAW FUNCTION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Print a single character
/// @param		chr			The character to draw
void Print_DrawChar(u8 chr) __FASTCALL
{
#if USE_PRINT_SHADOW
	if(g_PrintData.Shadow) // Handle shadow drawing (@todo To optimize without impacting the no-shadow rendering pipeline)
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
	if(g_PrintData.CursorX + PRINT_W(g_PrintData.UnitX) > g_PrintData.ScreenWidth) // Handle automatic new-line when 
		Print_Return();
	VDP_CommandWait();
#endif
	g_PrintData.PutChar(chr);
	g_PrintData.CursorX += PRINT_W(g_PrintData.UnitX);
}

//-----------------------------------------------------------------------------
/// Print the same character many times
/// @param		chr			Character to draw
/// @param		num			Number of drawing
void Print_DrawCharX(c8 chr, u8 num)
{
	for(u8 i = 0; i < num; ++i)
		Print_DrawChar(chr);
}

//-----------------------------------------------------------------------------
/// Print a character string
/// @param		chr			String to draw (must be null-terminated)
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
/// Print a character string many times
/// @param		chr			String to draw (must be null-terminated)
/// @param		num			Number of drawing
void Print_DrawTextX(const c8* str, u8 num)
{
	for(u8 i = 0; i < num; ++i)
		Print_DrawText(str);	
}

//-----------------------------------------------------------------------------
/// Print a 8-bits binary value
/// @param		value		Value to print
void Print_DrawBin8(u8 value) __FASTCALL
{
	for(u8 i = 0; i < 8; ++i)
	{
		if(value & (1 << (7 - i)))
			Print_DrawChar('1');
		else
			Print_DrawChar('0');
	}
	Print_DrawChar('b');
}

//-----------------------------------------------------------------------------
/// Print a 8-bits hexadecimal value
/// @param		value		Value to print
void Print_DrawHex8(u8 value) __FASTCALL
{
	Print_DrawChar(hexChar[(value >> 4) & 0x000F]);
	Print_DrawChar(hexChar[value & 0x000F]);
	Print_DrawChar('h');
}

//-----------------------------------------------------------------------------
/// Print a 16-bits hexadecimal value
/// @param		value		Value to print
void Print_DrawHex16(u16 value) __FASTCALL
{
	Print_DrawChar(hexChar[(value >> 12) & 0x000F]);
	Print_DrawChar(hexChar[(value >> 8) & 0x000F]);
	Print_DrawHex8((u8)value);
}

//-----------------------------------------------------------------------------
/// Print a 16-bits signed decimal value
/// @param		value		Value to print
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
