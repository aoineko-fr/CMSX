//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "input.h"
#include "print.h"
#include "vdp.h"
#include "memory.h"
#include "bios.h"
#include "bios_mainrom.h"
#include "math.h"
#include "draw.h"

//-----------------------------------------------------------------------------
// DEFINES

// Screen setting
struct ScreenSetting
{
	// Setup
	const c8*  Name;
	u8         Mode;
	u16        Width;
	u8         BPC;
	// Colors
	u8         Text;
	u8         Background;
	u8         Red;
	u8         White;
	u8         Gray;
	u8         Black;
	// Data
	const u8*  Font;
	const u8*  Data;
	const u8*  DataLMMC;
	const u16* Palette;
};

//
struct DrawData
{
	u16 x1, y1, x2, y2;
	u8 color;
};

//-----------------------------------------------------------------------------
// DATA

// Fonts
#include "font\font_cmsx_std0.h"
#include "font\font_ibm.h"

// Bitmaps
#include "data\data_bmp_2b.h"
#include "data\data_bmp_4b.h"
#include "data\data_bmp_8b.h"

// Screen mode setting index
u8 g_LMMC2b[16*16];
u8 g_LMMC4b[16*16];

// Screen mode settings
const struct ScreenSetting g_Settings[] =
{ //  Name              Mode              Width BPC Txt   BG    Red   White Gray  Black Font              Data      DataLMMC  Palette 
	{ "Screen 5 (G4)",	VDP_MODE_SCREEN5, 256,	4,	0xFF, 0x44, 0x88, 0xFF, 0x11, 0x11, g_Font_CMSX_Std0, g_DataBmp4b, g_LMMC4b, null }, // 0
	{ "Screen 6 (G5)",	VDP_MODE_SCREEN6, 512,	2,	0xFF, 0xAA, 0x55, 0xFF, 0xAA, 0x55, g_Font_IBM,       g_DataBmp2b, g_LMMC2b, null }, // 1
	{ "Screen 7 (G6)",	VDP_MODE_SCREEN7, 512,	4,	0xFF, 0x44, 0x88, 0xFF, 0x11, 0x11, g_Font_IBM,       g_DataBmp4b, g_LMMC4b, null }, // 2
	{ "Screen 8 (G7)",	VDP_MODE_SCREEN8, 256,	8,	0xFF, 0x47, 0x1C, 0xFF, 0x6D, 0x00, g_Font_CMSX_Std0, g_DataBmp8b, g_DataBmp8b, null }, // 3
};

const u8 chrAnim[] = { '|', '\\', '-', '/' };

// Screen mode setting index
u8 g_SrcModeIndex;
static u8 g_VBlank = 0;
static u8 g_Frame = 0;

struct DrawData g_Data;

//-----------------------------------------------------------------------------
//
void RandomizeData(u16 minX, u16 minY)
{
	u16 rnd = Math_GetRandom();
	g_Data.x1 = rnd >> 8;
	g_Data.x2 = rnd & 0x00FF;
	if(g_Settings[g_SrcModeIndex].Width == 256)
	{
		g_Data.x1 >>= 1;
		g_Data.x2 >>= 1;
	}
	g_Data.x1 += minX;
	g_Data.x2 += minX;
	
	rnd = Math_GetRandom();
	g_Data.y1 = rnd >> 8;
	g_Data.y2 = rnd & 0x00FF;
	while(g_Data.y1 > 98)
		g_Data.y1 -= 98;
	while(g_Data.y2 > 98)
		g_Data.y2 -= 98;
	g_Data.y1 += minY;
	g_Data.y2 += minY;
	
	rnd = Math_GetRandom();
	g_Data.color = rnd & 0x00FF;
	if(g_Data.color == 0)
		g_Data.color++;
}

//-----------------------------------------------------------------------------
//
void DisplayPage()
{
	const struct ScreenSetting* src = &g_Settings[g_SrcModeIndex];
	
	u8* buffer = Mem_HeapAlloc(256);
	u16 blockWidth = 16 * 256 / src->Width;
	u16 blockBytes = 16 / 8 * src->BPC;
	u16 lineBytes = src->Width / 8 * src->BPC;
	u16 X, Y;
	u8 scale = 8 / src->BPC;
	
	//-------------------------------------------------------------------------
	// Init
	
	VDP_SetScreen(src->Mode);
	VDP_SetColor(src->Black);
	VDP_CommandHMMV(0, 0, src->Width, 212, src->Background);
	
	VDP_SetPaletteEntry(1, RGB16(0, 0, 0));
	VDP_SetPaletteEntry(2, RGB16(1, 1, 4));
	VDP_SetPaletteEntry(3, RGB16(2, 2, 7));

	VDP_EnableSprite(false);

	Print_Initialize(src->Mode, src->Font);
	Print_SetColor(src->Text, src->Background);
	
	Print_SetPosition(4, 2);
	Print_DrawText("DRAW SAMPLE - ");
	Print_DrawText(src->Name);
	Draw_HLine(0, src->Width - 1, 12, src->White, 0);
	Draw_HLine(0, src->Width - 1, 114, src->White, 0);
	Draw_VLine(src->Width / 2, 16, 212, src->White, 0);
	
	// Draw lines
	for(u8 i = 0; i < 16; ++i)
	{
		RandomizeData(0, 16);
		Draw_Line(g_Data.x1, g_Data.y1, g_Data.x2, g_Data.y2, g_Data.color, 0);
	}
	// Draw vertical/harizontal lines
	for(u8 i = 0; i < 8; ++i)
	{
		RandomizeData(src->Width / 2, 16);
		Draw_HLine(g_Data.x1, g_Data.x2, g_Data.y2, g_Data.color, 0);
		RandomizeData(src->Width / 2, 16);
		Draw_VLine(g_Data.x1, g_Data.y1, g_Data.y2, g_Data.color, 0);
	}
	// Draw boxes
	for(u8 i = 0; i < 16; ++i)
	{
		RandomizeData(0, 114);
		Draw_Box(g_Data.x1, g_Data.y1, g_Data.x2, g_Data.y2, g_Data.color, 0);
	}
	// Draw circles
	for(u8 i = 0; i < 16; ++i)
	{
		RandomizeData(src->Width / 2, 114);
		Draw_Circle(
			(g_Data.x1 + g_Data.x2) / 2, 
			(g_Data.y1 + g_Data.y2) / 2, 
			Min(Abs16(g_Data.y1 - g_Data.y2) / 2, Abs16(g_Data.x1 - g_Data.x2) / 2),
			g_Data.color, 0);
	}

	Print_SetPosition(8, 20);
	Print_DrawText("Lines");
	Print_SetPosition((src->Width / 2) + 8 + 8, 20);
	Print_DrawText("Hori./Verti. lines");	
	Print_SetPosition(8, 114 + 4);
	Print_DrawText("Boxes");
	Print_SetPosition((src->Width / 2) + 8, 114 + 4);
	Print_DrawText("Circles");
}

/// H_TIMI interrupt hook
void VBlankHook()
{
	g_VBlank = 1;
}

/// Wait for V-Blank period
void WaitVBlank()
{
	while(g_VBlank == 0) {}
	g_VBlank = 0;
	g_Frame++;
}
//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	// Precalc
	for(u16 i = 0; i < 256; ++i)
	{
		u8 c, b = g_DataBmp4b[i >> 1];
		if((i & 0x1) == 0)
			c = b >> 4;
		else // if((i & 0x1) == 1)
			c = b;		
		g_LMMC4b[i] = c & 0x0F;
	}
	for(u16 i = 0; i < 256; ++i)
	{
		u8 c, b = g_DataBmp2b[i >> 2];
		if((i & 0x3) == 0)
			c = b >> 6;
		else if((i & 0x3) == 1)
			c = b >> 4;
		else if((i & 0x3) == 2)
			c = b >> 2;
		else // if((i & 0x3) == 3)
			c = b;
		g_LMMC2b[i] = c  & 0x03;
	}

	// Init	
	Bios_SetHookCallback(H_TIMI, VBlankHook);
	g_SrcModeIndex = 3;
	DisplayPage();

	bool bContinue = true;
	while(bContinue)
	{
		const struct ScreenSetting* src = &g_Settings[g_SrcModeIndex];

		Print_SetPosition(src->Width - 8, 2);
		Print_DrawChar(chrAnim[g_Frame & 0x03]);

		u8 row = Keyboard_Read(KEY_ROW(KEY_DOWN));
		if(((row & KEY_FLAG(KEY_UP)) == 0) || ((row & KEY_FLAG(KEY_LEFT)) == 0))
		{
			if(g_SrcModeIndex > 0)
				g_SrcModeIndex--;
			else
				g_SrcModeIndex = numberof(g_Settings) - 1;
			DisplayPage();
		}
		else if(((row & KEY_FLAG(KEY_DOWN)) == 0) || ((row & KEY_FLAG(KEY_RIGHT)) == 0))
		{
			if(g_SrcModeIndex < numberof(g_Settings) - 1)
				g_SrcModeIndex++;
			else
				g_SrcModeIndex = 0;
			DisplayPage();
		}
		
		if(Keyboard_IsKeyPressed(KEY_ESC))
			bContinue = false;

		WaitVBlank();
	}

	Bios_ClearHook(H_TIMI);
	Bios_Exit(0);
}