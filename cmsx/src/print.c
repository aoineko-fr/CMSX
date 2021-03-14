//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
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

#if (PRINT_WIDTH == PRINT_WIDTH_6)
	#define DATA_LEN 6
#else
	#define DATA_LEN 8
#endif

//-----------------------------------------------------------------------------
// PROTOTYPE

#if (USE_PRINT_BITMAP)
// Draw character from RAM - 8-bits colors
void DrawChar_8B(u8 chr) __FASTCALL;
// Draw character from RAM - 4-bits colors
void DrawChar_4B(u8 chr) __FASTCALL;
// Draw character from RAM - 2-bits colors
void DrawChar_2B(u8 chr) __FASTCALL;

// Draw character from RAM - Multi-mode transparent 
void DrawChar_Trans(u8 chr) __FASTCALL;
#endif

#if (USE_PRINT_VRAM)
// Draw character from VRAM - 256 pixel screen
void DrawChar_VRAM256(u8 chr) __FASTCALL;
// Draw character from VRAM - 512 pixel screen
void DrawChar_VRAM512(u8 chr) __FASTCALL;
#endif

#if (USE_PRINT_SPRITE)
// Draw character from Sprites
void DrawChar_Sprite(u8 chr) __FASTCALL;
#endif

#if (USE_PRINT_TEXT)
// Draw characters as pattern name
void DrawChar_Layout(u8 chr) __FASTCALL;
#endif


//-----------------------------------------------------------------------------
//
// DATA
//
//-----------------------------------------------------------------------------

/// Address of the heap top
extern u16 g_HeapStartAddress;

/// Allocate memory for Print module data structure
struct Print_Data g_PrintData;

/// Table use to quick decimal-to-hexadecimal conversion
static const c8 hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

#if (USE_PRINT_VALIDATOR)
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


//-----------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//-----------------------------------------------------------------------------

/// Slit color from merged colors
u8 Print_SplitColor(u8 color) __FASTCALL
{
	switch(VDP_GetMode())
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
	return color;
}

u8 Print_MergeColor(u8 color) __FASTCALL
{
	switch(VDP_GetMode())
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
	return color;
}

//-----------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initialize print module. Must be called after VDP_SetMode()
bool Print_Initialize()
{
	Print_SetColor(0xF, 0x0);
	Print_SetPosition(0, 0);
	#if (USE_PRINT_FX_SHADOW)
		Print_EnableShadow(false);
	#endif
	#if (USE_PRINT_FX_OUTLINE)
		Print_EnableOutline(false);
	#endif

	switch(VDP_GetMode()) // Screen mode specific initialization
	{
	#if (USE_VDP_MODE_T1)
		case VDP_MODE_TEXT1:		// 40 characters per line of text, one colour for each character
			Print_SetTabSize(3);
			g_PrintData.ScreenWidth = 40;
			break;
	#endif
	#if (USE_VDP_MODE_MC)
		case VDP_MODE_MULTICOLOR:	// pseudo-graphic, one character divided into four block
			break;
	#endif
	#if (USE_VDP_MODE_G1)
		case VDP_MODE_GRAPHIC1:		// 32 characters per one line of text, the COLOURed character available
			Print_SetTabSize(3);
			g_PrintData.ScreenWidth = 32;
			break;
	#endif
	#if (USE_VDP_MODE_G2)
		case VDP_MODE_GRAPHIC2:		// 256 x 192, the colour is specififed for each 8 dots
			Print_SetTabSize(3);
			g_PrintData.ScreenWidth = 32;
			break;
	#endif
	#if (USE_VDP_MODE_T2)
		case VDP_MODE_TEXT2:		// 80 characters per line of text, character blinkable selection
			Print_SetTabSize(3);
			g_PrintData.ScreenWidth = 80;
			break;
	#endif
	#if (USE_VDP_MODE_G3)
		case VDP_MODE_GRAPHIC3:		// GRAPHIC 2 which can use sprite mode 2
			Print_SetTabSize(3);
			g_PrintData.ScreenWidth = 32;
			break;
	#endif
	#if (USE_VDP_MODE_G4)
		case VDP_MODE_GRAPHIC4:		// 256 x 212; 16 colours are available for each dot
			Print_SetTabSize(PRINT_TAB_SIZE);
			g_PrintData.ScreenWidth = 256;
			break;
	#endif
	#if (USE_VDP_MODE_G5)
		case VDP_MODE_GRAPHIC5:		// 512 x 212; 4 colours are available for each dot
			Print_SetTabSize(PRINT_TAB_SIZE);
			Print_SetColor(0x03, 0x0);
			g_PrintData.ScreenWidth = 512;
			break;
	#endif
	#if (USE_VDP_MODE_G6)
		case VDP_MODE_GRAPHIC6:		// 512 x 212; 16 colours are available for each dot
			Print_SetTabSize(PRINT_TAB_SIZE);
			g_PrintData.ScreenWidth = 512;
			break;
	#endif
	#if (USE_VDP_MODE_G7)
		case VDP_MODE_GRAPHIC7:		// 256 x 212; 256 colours are available for each dot
			Print_SetTabSize(PRINT_TAB_SIZE);
			Print_SetColor(0xFF, 0x0);
			g_PrintData.ScreenWidth = 256;
			break;
	#endif
	default:
		// Screen mode not (yet) supported!
		return false;
	}
	
	// g_PrintData.PatternsPerLine = g_PrintData.ScreenWidth / PRINT_W(g_PrintData.UnitX);

	return true;
}

//-----------------------------------------------------------------------------
///
void Print_SetMode(u8 mode) __FASTCALL
{
	g_PrintData.SourceMode = mode;

	switch(g_PrintData.SourceMode)
	{
	#if (USE_PRINT_BITMAP)
		case PRINT_MODE_BITMAP:
		{
			switch(VDP_GetMode()) // Screen mode specific initialization
			{
			#if (USE_VDP_MODE_G5)
				case VDP_MODE_GRAPHIC5:
					g_PrintData.DrawChar = DrawChar_2B;
					break;
			#endif
			#if (USE_VDP_MODE_G4 || USE_VDP_MODE_G6)
				case VDP_MODE_GRAPHIC4:
				case VDP_MODE_GRAPHIC6:
					g_PrintData.DrawChar = DrawChar_4B;
					break;
			#endif
			#if (USE_VDP_MODE_G7)
				case VDP_MODE_GRAPHIC7:
					g_PrintData.DrawChar = DrawChar_8B; 
					break;
			#endif
			}
			break;
		}
		case PRINT_MODE_BITMAP_TRANS:
		{
			g_PrintData.DrawChar = DrawChar_Trans;
			break;
		}
	#endif
	#if (USE_PRINT_VRAM)
		case PRINT_MODE_BITMAP_VRAM:
		{
			switch(VDP_GetMode()) // Screen mode specific initialization
			{
			#if (USE_VDP_MODE_G4 || USE_VDP_MODE_G7)
				case VDP_MODE_GRAPHIC4:
				case VDP_MODE_GRAPHIC7:
					g_PrintData.DrawChar = DrawChar_VRAM256;
					break;
			#endif
			#if (USE_VDP_MODE_G5 || USE_VDP_MODE_G6)
				case VDP_MODE_GRAPHIC5:
				case VDP_MODE_GRAPHIC6:
					g_PrintData.DrawChar = DrawChar_VRAM512;
					break;
			#endif
			};
			break;
		}
	#endif
	#if (USE_PRINT_SPRITE)
		case PRINT_MODE_SPRITE:
		{
			g_PrintData.DrawChar = DrawChar_Sprite;
			break;
		}
	#endif
	#if (USE_PRINT_TEXT)
		case PRINT_MODE_TEXT:
		{
			g_PrintData.DrawChar = DrawChar_Layout;
			break;
		}
	#endif
	};
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
/// Initialize color buffer
void Print_InitColorBuffer(u8 t, u8 b)
{
	// Pre-compute colors combinaison for 2-bits of a character pattern line (used to quick drawing in DrawChar_GX functions)
	switch(VDP_GetMode())
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

//-----------------------------------------------------------------------------
/// Set the draw color
/// @param		text		Text color (format depend of current screen mode)
/// @param		bg			Background color (format depend of current screen mode)
void Print_SetColor(u8 text, u8 bg)
{
	if(VDP_IsBitmapMode(VDP_GetMode())) // Bitmap mode
	{
		#if (USE_PRINT_BITMAP)
			u8 t = text;
			#if (USE_PRINT_VALIDATOR)
				t = Print_SplitColor(t);
			#endif
			#if (PRINT_COLOR_NUM == 1)
				g_PrintData.TextColor = t;
			#else // if (PRINT_COLOR_NUM > 1)
				for(u8 i = 0; i < PRINT_COLOR_NUM; ++i)
				{
					g_PrintData.TextColor[i] = t;
				}
			#endif
			
			u8 b = bg;
			#if (USE_PRINT_VALIDATOR)
				b = Print_SplitColor(b);
			#endif
			g_PrintData.BGColor = b;

			Print_InitColorBuffer(t, b);
		#endif
	}
	else // Text mode
	{
		#if (USE_PRINT_TEXT)
			u8 col = text << 4 | bg;
			switch(VDP_GetMode())
			{
			#if (USE_VDP_MODE_T1)
				case VDP_MODE_TEXT1:		// 40 characters per line of text, one colour for each character
			#endif
			#if (USE_VDP_MODE_T2)
				case VDP_MODE_TEXT2:		// 80 characters per line of text, character blinkable selection
			#endif
			#if (USE_VDP_MODE_T1 || USE_VDP_MODE_T2)
					VDP_SetColor(col);
					break;
			#endif
			#if (USE_VDP_MODE_G1)
				case VDP_MODE_GRAPHIC1:		// 32 characters per one line of text, the COLOURed character available
					VDP_FillVRAM_64K(col, g_ScreenColorLow, 32);
					break;
			#endif
			#if (USE_VDP_MODE_G2)
				case VDP_MODE_GRAPHIC2:		// 256 x 192, the colour is specififed for each 8 dots
			#endif
			#if (USE_VDP_MODE_G3)
				case VDP_MODE_GRAPHIC3:		// GRAPHIC 2 which can use sprite mode 2
			#endif
			#if (USE_VDP_MODE_G2 || USE_VDP_MODE_G3)
				{
					u16 dst = (u16)g_ScreenColorLow + g_PrintData.PatternOffset * 8;
					VDP_FillVRAM_64K(col, dst, g_PrintData.CharCount * 8);
					dst += 256 * 8;
					VDP_FillVRAM_64K(col, dst, g_PrintData.CharCount * 8);
					dst += 256 * 8;
					VDP_FillVRAM_64K(col, dst, g_PrintData.CharCount * 8);
					break;
				}
			#endif
			};
		#endif
	}
}

#if (PRINT_COLOR_NUM > 1)
//-----------------------------------------------------------------------------
/// Set color shade
void Print_SetColorShade(const u8* shade) __FASTCALL
{
	if(VDP_IsBitmapMode(VDP_GetMode())) // Bitmap mode
	{
		#if (USE_PRINT_BITMAP)
			for(u8 i = 0; i < PRINT_COLOR_NUM; ++i)
			{
				u8 t = shade[i];
				#if (USE_PRINT_VALIDATOR)
					t = Print_SplitColor(t);
				#endif
				g_PrintData.TextColor[i] = t;
			}
		#endif
	}
	else // Text mode
	{
		#if (USE_PRINT_TEXT)
			switch(VDP_GetMode())
			{
			#if (USE_VDP_MODE_G2)
				case VDP_MODE_GRAPHIC2:		// 256 x 192, the colour is specififed for each 8 dots
			#endif
			#if (USE_VDP_MODE_G3)
				case VDP_MODE_GRAPHIC3:		// GRAPHIC 2 which can use sprite mode 2
			#endif
			#if (USE_VDP_MODE_G2 || USE_VDP_MODE_G3)
				{
					u16 dst = (u16)g_ScreenColorLow + g_PrintData.PatternOffset * 8;
					for(u8 i = 0; i < g_PrintData.CharCount; ++i)
					{
						VDP_WriteVRAM_64K(shade, dst,           8);
						VDP_WriteVRAM_64K(shade, dst + 256 * 8, 8);
						VDP_WriteVRAM_64K(shade, dst + 512 * 8, 8);
						dst += 8;
					}
					break;
				}
			#endif
			};
		#endif
	}
}
#endif


//-----------------------------------------------------------------------------
//
// VALIDATOR
//
//-----------------------------------------------------------------------------

#if (USE_PRINT_VALIDATOR)
//-----------------------------------------------------------------------------
/// Validate character. Try to convert invalid letter to their upper/lower case conterpart or use default invalid character
/// @param		chr			Address of the character to check
void Print_ValidateChar(u8* chr)
{
	if((*chr < g_PrintData.CharFirst) || (*chr > g_PrintData.CharLast))
	{
		if((*chr >= 'a') && (*chr <= 'z') && (g_PrintData.CharFirst <= 'A') && (g_PrintData.CharLast >= 'Z')) // try to remap to upper case letter
		{
			*chr = *chr - 'a' + 'A';
		}
		else if((*chr >= 'A') && (*chr <= 'Z') && (g_PrintData.CharFirst <= 'a') && (g_PrintData.CharLast >= 'z')) // try to remap to lower case letter
		{
			*chr = *chr - 'A' + 'a';
		}
		else
			*chr = g_PrintData.CharFirst;
	}
}
//-----------------------------------------------------------------------------
/// Validate character. Try to convert invalid letter to their upper/lower case conterpart or use default invalid character
/// @param		chr			Address of the character to check
/// @param		patterns	Address of the font data to check
void Print_ValidatePattern(u8* chr, const c8** patterns)
{
	if((*chr < g_PrintData.CharFirst) || (*chr > g_PrintData.CharLast))
	{
		if((*chr >= 'a') && (*chr <= 'z') && (g_PrintData.CharFirst <= 'A') && (g_PrintData.CharLast >= 'Z')) // try to remap to upper case letter
		{
			*chr = *chr - 'a' + 'A';
			*patterns = g_PrintData.FontPatterns + g_PrintData.PatternY * (*chr - g_PrintData.CharFirst);
		}
		else if((*chr >= 'A') && (*chr <= 'Z') && (g_PrintData.CharFirst <= 'a') && (g_PrintData.CharLast >= 'z')) // try to remap to lower case letter
		{
			*chr = *chr - 'A' + 'a';
			*patterns = g_PrintData.FontPatterns + g_PrintData.PatternY * (*chr - g_PrintData.CharFirst);
		}
		else
			*patterns = g_PrintInvalid;
	}
}
#endif // USE_PRINT_VALIDATOR


//-----------------------------------------------------------------------------
//
// BITMAP FONT RAM
//
//-----------------------------------------------------------------------------

#if (USE_PRINT_BITMAP)
//-----------------------------------------------------------------------------
/// Initialize print module. Must be called after VDP_SetMode()
/// @param		font		Pointer to font data to use (null=use Main-ROM font)
bool Print_SetBitmapFont(const u8* font) __FASTCALL
{
	Print_SetFont(font);
	Print_SetMode(PRINT_MODE_BITMAP);
	Print_Initialize();
}

#if (USE_VDP_MODE_G7)
//-----------------------------------------------------------------------------
/// Graphic 7 (Screen mode 8) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void DrawChar_8B(u8 chr) __FASTCALL
{
	const u8* patterns = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.PatternY); // Get character patterns' base address
	#if (USE_PRINT_VALIDATOR)
		Print_ValidatePattern(&chr, &patterns);
	#endif
	u16* l = (u16*)g_HeapStartAddress;
	for(u8 j = 0; j < PRINT_H(g_PrintData.PatternY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		#if (PRINT_COLOR_NUM > 1)
			Print_InitColorBuffer(g_PrintData.TextColor[j], g_PrintData.BGColor);
		#endif // (PRINT_COLOR_NUM > 1)
		u8 f = patterns[j];
		*l++ = ((u16*)g_PrintData.Buffer)[f >> 6];
		*l++ = ((u16*)g_PrintData.Buffer)[(f >> 4) & 0x03];
		*l++ = ((u16*)g_PrintData.Buffer)[(f >> 2) & 0x03];
		#if (PRINT_WIDTH != PRINT_WIDTH_6)
			*l++ = ((u16*)g_PrintData.Buffer)[f & 0x03];
		#endif
	}
	VDP_CommandHMMC((u8*)g_HeapStartAddress, g_PrintData.CursorX, g_PrintData.CursorY, DATA_LEN, PRINT_H(g_PrintData.PatternY));
}
#endif // USE_VDP_MODE_G7


#if (USE_VDP_MODE_G4 || USE_VDP_MODE_G6)
//-----------------------------------------------------------------------------
/// Graphic 4/6 (Screen mode 5/7) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void DrawChar_4B(u8 chr) __FASTCALL
{
	const u8* patterns = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.PatternY); // Get character patterns' base address
	#if (USE_PRINT_VALIDATOR)
		Print_ValidatePattern(&chr, &patterns);
	#endif
	u8* l = (u8*)g_HeapStartAddress;
	for(u8 j = 0; j < PRINT_H(g_PrintData.PatternY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		#if (PRINT_COLOR_NUM > 1)
			Print_InitColorBuffer(g_PrintData.TextColor[j], g_PrintData.BGColor);
		#endif // (PRINT_COLOR_NUM > 1)
		u8 f = patterns[j];
		*l++ = g_PrintData.Buffer[f >> 6];
		*l++ = g_PrintData.Buffer[(f >> 4) & 0x03];
		*l++ = g_PrintData.Buffer[(f >> 2) & 0x03];
		#if (PRINT_WIDTH != PRINT_WIDTH_6)
			*l++ = g_PrintData.Buffer[f & 0x03];
		#endif
	}
	VDP_CommandHMMC((u8*)g_HeapStartAddress, g_PrintData.CursorX, g_PrintData.CursorY, DATA_LEN, PRINT_H(g_PrintData.PatternY));
}
#endif // (USE_VDP_MODE_G4 || USE_VDP_MODE_G6)


#if (USE_VDP_MODE_G5)
//-----------------------------------------------------------------------------
/// Graphic 5 (Screen mode 6) low-level function to draw a character in VRAM 
/// @param		chr			The character to draw
void DrawChar_2B(u8 chr) __FASTCALL
{
	const u8* patterns = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.PatternY); // Get character patterns' base address
	#if (USE_PRINT_VALIDATOR)
		Print_ValidatePattern(&chr, &patterns);
	#endif
	u8* l = (u8*)g_HeapStartAddress;
	for(u8 j = 0; j < PRINT_H(g_PrintData.PatternY); ++j) // Unpack each 6/8-bits line to buffer and send it to VRAM
	{
		#if (PRINT_COLOR_NUM > 1)
			Print_InitColorBuffer(g_PrintData.TextColor[j], g_PrintData.BGColor);
		#endif // (PRINT_COLOR_NUM > 1)
		u8 f = patterns[j];
		*l++ = g_PrintData.Buffer[f >> 4];
		*l++ = g_PrintData.Buffer[f & 0x0F];
	}
	VDP_CommandHMMC((u8*)g_HeapStartAddress, g_PrintData.CursorX, g_PrintData.CursorY, 8, PRINT_H(g_PrintData.PatternY));
}
#endif // USE_VDP_MODE_G5

//-----------------------------------------------------------------------------
///
void DrawChar_Trans(u8 chr) __FASTCALL
{
	const u8* patterns = g_PrintData.FontAddr + chr * PRINT_H(g_PrintData.PatternY); // Get character patterns' base address
	#if (USE_PRINT_VALIDATOR)
		Print_ValidatePattern(&chr, &patterns);
	#endif
	#if (USE_PRINT_FX_SHADOW)
		if(g_PrintData.Shadow)
		{
			g_VDP_Command.DY = g_PrintData.CursorY + g_PrintData.ShadowOffsetY - 3;
			g_VDP_Command.CLR = g_PrintData.ShadowColor;
			g_VDP_Command.ARG = 0;
			g_VDP_Command.CMD = VDP_CMD_PSET + 0;
			for(u8 j = 0; j < PRINT_H(g_PrintData.PatternY); ++j)
			{
				g_VDP_Command.DX = g_PrintData.CursorX + g_PrintData.ShadowOffsetX - 3;
				
				u8 f = patterns[j];
				if(f & BIT_7) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_6) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_5) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_4) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_3) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_2) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_1) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_0) VPD_CommandSetupR36();
				g_VDP_Command.DY++;
			}
		}
	#endif
	#if (USE_PRINT_FX_OUTLINE)
		if(g_PrintData.Outline)
		{
			g_VDP_Command.DY = g_PrintData.CursorY - 1;
			g_VDP_Command.NX = 3;
			g_VDP_Command.NY = 3;
			g_VDP_Command.CLR = g_PrintData.OutlineColor;
			g_VDP_Command.ARG = 0;
			g_VDP_Command.CMD = (u8)(VDP_CMD_LMMV + 0);
			for(u8 j = 0; j < PRINT_H(g_PrintData.PatternY); ++j)
			{
				g_VDP_Command.DX = g_PrintData.CursorX - 1;
				
				u8 f = patterns[j];
				if(f & BIT_7) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_6) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_5) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_4) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_3) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_2) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_1) VPD_CommandSetupR36(); g_VDP_Command.DX++;
				if(f & BIT_0) VPD_CommandSetupR36();
				g_VDP_Command.DY++;
			}
		}
	#endif
	
	g_VDP_Command.DY = g_PrintData.CursorY;
	g_VDP_Command.ARG = 0;
	g_VDP_Command.CMD = VDP_CMD_PSET + 0;
	#if (PRINT_COLOR_NUM == 1)
		g_VDP_Command.CLR = g_PrintData.TextColor;
	#endif
	for(u8 j = 0; j < PRINT_H(g_PrintData.PatternY); ++j)
	{
		#if (PRINT_COLOR_NUM > 1)
			g_VDP_Command.CLR = g_PrintData.TextColor[j];
		#endif
		g_VDP_Command.DX = g_PrintData.CursorX;
		
		u8 f = patterns[j];
		if(f & BIT_7) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_6) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_5) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_4) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_3) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_2) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_1) VPD_CommandSetupR36(); g_VDP_Command.DX++;
		if(f & BIT_0) VPD_CommandSetupR36();
		g_VDP_Command.DY++;
	}
}

#endif // (USE_PRINT_BITMAP)


//-----------------------------------------------------------------------------
//
// BITMAP FONT VRAM
//
//-----------------------------------------------------------------------------

#if (USE_PRINT_VRAM)
//-----------------------------------------------------------------------------
/// Set the current font and upload it to VRAM
void Print_SetFontVRAM(const u8* font, UY y)
{
	Print_SetFont(font);
	Print_Initialize();
	
	UX cx = g_PrintData.CursorX;
	UY cy = g_PrintData.CursorY;

	// Load font to VRAM	
	Print_SetMode(PRINT_MODE_BITMAP_TRANS); // Activate default mode to write font data into VRAM
	g_PrintData.FontVRAMY = y;
	// @todo To optimize (pre-compute + fixed width/height cases
	u8 nx = g_PrintData.ScreenWidth / PRINT_W(g_PrintData.UnitX);
	for(u16 chr = g_PrintData.CharFirst; chr <= g_PrintData.CharLast; ++chr)
	{
		u16 idx = chr - g_PrintData.CharFirst;
		// @todo To optimize (pre-compute + fixed width/height cases
		g_PrintData.CursorX = (idx % nx) * PRINT_W(g_PrintData.UnitX);		
		g_PrintData.CursorY = (idx / nx) * PRINT_H(g_PrintData.PatternY) + y;
		VDP_CommandHMMV(g_PrintData.CursorX, g_PrintData.CursorY, PRINT_W(g_PrintData.UnitX), PRINT_H(g_PrintData.PatternY), 0);
		g_PrintData.DrawChar(chr);
	}

	g_PrintData.CursorX = cx;
	g_PrintData.CursorY = cy;
	
	g_PrintData.CharPerLine = g_PrintData.ScreenWidth / PRINT_W(g_PrintData.UnitX);
	
	Print_SetMode(PRINT_MODE_BITMAP_VRAM);
}

#if (USE_VDP_MODE_G4 || USE_VDP_MODE_G7)
//-----------------------------------------------------------------------------
/// 
/// @param		chr			The character to draw
void DrawChar_VRAM256(u8 chr) __FASTCALL
{
	#if (USE_PRINT_VALIDATOR)
		Print_ValidateChar(&chr);
	#endif
	u8 idx = chr - g_PrintData.CharFirst;
	#if (PRINT_WIDTH == PRINT_WIDTH_6)
		u16 sx = (idx % 42) * PRINT_W(g_PrintData.UnitX);		
		u16 sy = (idx / 42) * PRINT_H(g_PrintData.PatternY) + g_PrintData.FontVRAMY;
	#elif (PRINT_WIDTH == PRINT_WIDTH_8)
		u16 sx = (idx % 32) * PRINT_W(g_PrintData.UnitX);		
		u16 sy = (idx / 32) * PRINT_H(g_PrintData.PatternY) + g_PrintData.FontVRAMY;
	#else
		u16 sx = (idx % g_PrintData.CharPerLine) * PRINT_W(g_PrintData.UnitX);		
		u16 sy = (idx / g_PrintData.CharPerLine) * PRINT_H(g_PrintData.PatternY) + g_PrintData.FontVRAMY;
	#endif

	VDP_CommandHMMM(sx, sy, g_PrintData.CursorX, g_PrintData.CursorY, PRINT_W(g_PrintData.UnitX), PRINT_H(g_PrintData.PatternY));
}
#endif // (USE_VDP_MODE_G4 || USE_VDP_MODE_G7)

#if (USE_VDP_MODE_G5 || USE_VDP_MODE_G6)
//-----------------------------------------------------------------------------
/// 
/// @param		chr			The character to draw
void DrawChar_VRAM512(u8 chr) __FASTCALL
{
	#if (USE_PRINT_VALIDATOR)
		Print_ValidateChar(&chr);
	#endif
	u8 idx = chr - g_PrintData.CharFirst;
	#if (PRINT_WIDTH == PRINT_WIDTH_6)
		u16 sx = (idx % 85) * PRINT_W(g_PrintData.UnitX);		
		u16 sy = (idx / 85) * PRINT_H(g_PrintData.PatternY) + g_PrintData.FontVRAMY;
	#elif (PRINT_WIDTH == PRINT_WIDTH_8)
		u16 sx = (idx % 64) * PRINT_W(g_PrintData.UnitX);		
		u16 sy = (idx / 64) * PRINT_H(g_PrintData.PatternY) + g_PrintData.FontVRAMY;
	#else
		u16 sx = (idx % g_PrintData.CharPerLine) * PRINT_W(g_PrintData.UnitX);		
		u16 sy = (idx / g_PrintData.CharPerLine) * PRINT_H(g_PrintData.PatternY) + g_PrintData.FontVRAMY;
	#endif

	VDP_CommandHMMM(sx, sy, g_PrintData.CursorX, g_PrintData.CursorY, PRINT_W(g_PrintData.UnitX), PRINT_H(g_PrintData.PatternY));
}
#endif // (USE_VDP_MODE_G5 || USE_VDP_MODE_G6)

#endif // (USE_PRINT_VRAM)


//-----------------------------------------------------------------------------
//
// TEXT FONT
//
//-----------------------------------------------------------------------------

#if (USE_PRINT_TEXT)
#if (USE_VDP_MODE_T1 || USE_VDP_MODE_T2 || USE_VDP_MODE_G1 || USE_VDP_MODE_G2 || USE_VDP_MODE_G3)

//-----------------------------------------------------------------------------
/// Set the current font and upload it to VRAM
void Print_SetTextFont(const u8* font, u8 offset)
{
	g_PrintData.PatternOffset = offset;

	// Initialize font attributes
	Print_SetFontEx(8, 8, 1, 1, font[2], font[3], font+4);
	Print_Initialize();
	Print_SetMode(PRINT_MODE_TEXT);

	// Load font data to VRAM
	const u8* src = font + 4;
	u16 dst = (u16)g_ScreenPatternLow + (offset * 8);
	VDP_WriteVRAM_64K(src, dst, g_PrintData.CharCount * 8);
	
	switch(VDP_GetMode())
	{
	#if (USE_VDP_MODE_G2)
		case VDP_MODE_GRAPHIC2:		// 256 x 192, the colour is specififed for each 8 dots
	#endif
	#if (USE_VDP_MODE_G3)
		case VDP_MODE_GRAPHIC3:		// GRAPHIC 2 which can use sprite mode 2
	#endif
	#if (USE_VDP_MODE_G2 || USE_VDP_MODE_G3)
		dst += 256 * 8;
		VDP_WriteVRAM_64K(src, dst, g_PrintData.CharCount * 8);
		dst += 256 * 8;
		VDP_WriteVRAM_64K(src, dst, g_PrintData.CharCount * 8);
		break;
	#endif
	};
}

//-----------------------------------------------------------------------------
/// Draw characters as pattern name
/// @param		chr			The character to draw
void DrawChar_Layout(u8 chr) __FASTCALL
{
	#if (USE_PRINT_VALIDATOR)
		Print_ValidateChar(&chr);
	#endif
	u8 shape = chr - g_PrintData.CharFirst + g_PrintData.PatternOffset;
	u16 dst = (u16)g_ScreenLayoutLow + (g_PrintData.CursorY * g_PrintData.ScreenWidth) + g_PrintData.CursorX;
	VDP_FillVRAM_64K(shape, dst, 1);
}

#endif
#endif


//-----------------------------------------------------------------------------
//
// SPRITE FONT
//
//-----------------------------------------------------------------------------

#if (USE_PRINT_SPRITE)

extern u16 g_SpritePatternLow;
extern u8  g_SpritePatternHigh;

//-----------------------------------------------------------------------------
/// 
void Print_SetFontSprite(const u8* font, u8 patIdx, u8 sprtIdx)
{
	g_PrintData.SpritePattern = patIdx;
	g_PrintData.SpriteID = sprtIdx;

	Print_SetFont(font);
	Print_SetMode(PRINT_MODE_SPRITE);
	Print_Initialize();

	#if (PRINT_HEIGHT == PRINT_HEIGHT_8)
		VDP_LoadSpritePattern(g_PrintData.FontPatterns, patIdx, g_PrintData.CharCount);
	#else // (PRINT_HEIGHT == PRINT_HEIGHT_X)
		u16 ram = (u16)g_PrintData.FontPatterns;
		u16 vram = g_SpritePatternLow;
		for(u16 chr = g_PrintData.CharFirst; chr <= g_PrintData.CharLast; ++chr)
		{
			u16 idx = chr - g_PrintData.CharFirst;
			if(PRINT_H(g_PrintData.PatternY) < 8)
			{
				VDP_WriteVRAM((u8*)ram, vram, g_SpritePatternHigh, PRINT_H(g_PrintData.PatternY));
				vram += PRINT_H(g_PrintData.PatternY);
				VDP_FillVRAM(0, vram, g_SpritePatternHigh, 8 - PRINT_H(g_PrintData.PatternY));
				vram += 8 - PRINT_H(g_PrintData.PatternY);
			}
			else
			{
				VDP_WriteVRAM((u8*)ram, vram, g_SpritePatternHigh, 8);
				vram += 8;
			}
			ram += PRINT_H(g_PrintData.PatternY);
		}
	#endif
}


//-----------------------------------------------------------------------------
/// 
/// @param		chr			The character to draw
void DrawChar_Sprite(u8 chr) __FASTCALL
{
	u16 shape = chr - g_PrintData.CharFirst + g_PrintData.SpritePattern;
	#if (PRINT_COLOR_NUM == 1)
		VDP_SetSpriteExUniColor(g_PrintData.SpriteID++, g_PrintData.CursorX, g_PrintData.CursorY, shape, g_PrintData.TextColor);
	#else // (PRINT_COLOR_NUM > 1)
		VDP_SetSpriteExMultiColor(g_PrintData.SpriteID++, g_PrintData.CursorX, g_PrintData.CursorY, shape, g_PrintData.TextColor);
	#endif
}

#endif // (USE_PRINT_SPRITE)


//-----------------------------------------------------------------------------
//
// FX
//
//-----------------------------------------------------------------------------

#if (USE_PRINT_FX_SHADOW)
//-----------------------------------------------------------------------------
/// Set shadow effect
/// @param		activate	Activate/deactivate shadow
/// @param		offsetX		Shadow offset on X axis (can be from -3 to +4)
/// @param		offsetY		Shadow offset on Y axis (can be from -3 to +4)
/// @param		color		Shadow color (depend of the screen mode)
void Print_SetShadow(bool enable, i8 offsetX, i8 offsetY, u8 color)
{
	Print_EnableShadow(enable);
	g_PrintData.ShadowOffsetX = 3 + offsetX; // Math_Clamp(offsetX, (i8)-3, 4);
	g_PrintData.ShadowOffsetY = 3 + offsetY; // Math_Clamp(offsetY, (i8)-3, 4);
	g_PrintData.ShadowColor   = color;
}

//-----------------------------------------------------------------------------
/// Activate/desactivate shadow effect
/// @param		activate	Activate/deactivate shadow
void Print_EnableShadow(bool enable) __FASTCALL
{
	g_PrintData.Shadow = enable;
	Print_SetMode(enable ? PRINT_MODE_BITMAP_TRANS : PRINT_MODE_BITMAP); // enable default mode to write font data into VRAM
}
#endif // USE_PRINT_FX_SHADOW	

#if (USE_PRINT_FX_OUTLINE)	
//-----------------------------------------------------------------------------
/// Set shadow effect
void Print_SetOutline(bool enable, u8 color)
{
	Print_EnableOutline(enable);
	g_PrintData.OutlineColor = color;
}

//-----------------------------------------------------------------------------
/// Activate/desactivate shadow effect
void Print_EnableOutline(bool enable) __FASTCALL
{
	g_PrintData.Outline = enable;
	Print_SetMode(enable ? PRINT_MODE_BITMAP_TRANS : PRINT_MODE_BITMAP); // enable default mode to write font data into VRAM
}
#endif // USE_PRINT_FX_OUTLINE


//-----------------------------------------------------------------------------
//
// DRAW FUNCTION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Clear screen on the current page
void Print_Clear()
{
	if(VDP_IsBitmapMode(VDP_GetMode())) // Bitmap mode
	{
		#if (USE_PRINT_BITMAP)
			u8 color = Print_MergeColor(g_PrintData.BGColor);
			VDP_CommandHMMV(0, 0/*g_PrintData.Page * 256*/, g_PrintData.ScreenWidth, 212, color); // @todo Check the 192/212 lines parameter
			VDP_CommandWait();
		#endif
	}
	else // Text mode
	{
		#if (USE_PRINT_TEXT)
			VDP_FillVRAM(0, g_ScreenLayoutLow, g_ScreenLayoutHigh, 24 * g_PrintData.ScreenWidth);
		#endif
	}
}

//-----------------------------------------------------------------------------
/// Clear X character back from current cursor position
/// @param		num			Number of characters to remove
void Print_Backspace(u8 num) __FASTCALL
{
	if(VDP_IsBitmapMode(VDP_GetMode())) // Bitmap mode
	{
		#if (USE_PRINT_BITMAP)
			u16 x = PRINT_W(g_PrintData.UnitX) * num;
			if(x >  g_PrintData.CursorX)
				x = 0;
			else
				x = g_PrintData.CursorX - x;

			u8 color = Print_MergeColor(g_PrintData.BGColor);
			VDP_CommandHMMV(x, g_PrintData.CursorY, x - g_PrintData.CursorX, PRINT_H(g_PrintData.UnitY), color); // @todo Check the 192/212 lines parameter
			g_PrintData.CursorX = x;
			VDP_CommandWait();	
		#endif
	}
	else // Text mode
	{
		#if (USE_PRINT_TEXT)
			u16 dst = g_ScreenLayoutLow + (g_PrintData.CursorY * g_PrintData.ScreenWidth) + g_PrintData.CursorX - num;
			VDP_FillVRAM(0, dst, g_ScreenLayoutHigh, num);
		#endif
	}
}
//-----------------------------------------------------------------------------
/// Print a single character
/// @param		chr			The character to draw
void Print_DrawChar(u8 chr) __FASTCALL
{
	#if (USE_PRINT_VALIDATOR)
		if(g_PrintData.CursorX + PRINT_W(g_PrintData.UnitX) > g_PrintData.ScreenWidth) // Handle automatic new-line when 
			Print_Return();
		#if (MSX_VERSION > MSX_1)
			VDP_CommandWait();
		#endif
	#endif
	g_PrintData.DrawChar(chr);

	// g_PrintData.DrawChar(chr);
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
	#if (USE_PRINT_UNIT)
		Print_DrawChar('b');
	#endif
}

//-----------------------------------------------------------------------------
/// Print a 8-bits hexadecimal value
/// @param		value		Value to print
void Print_DrawHex8(u8 value) __FASTCALL
{
	Print_DrawChar(hexChar[(value >> 4) & 0x000F]);
	Print_DrawChar(hexChar[value & 0x000F]);
	#if (USE_PRINT_UNIT)
		Print_DrawChar('h');
	#endif
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
