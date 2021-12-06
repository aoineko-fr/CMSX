// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
#include "core.h"
#include "bios.h"
#include "bios_mainrom.h"
#include "vdp.h"
#include "input.h"

//-----------------------------------------------------------------------------
// DATA

// Fonts
#include "font\font_cmsx_symbol1.h"
//#include "data\data_sprt_8.h"
//#include "data\data_sprt_16.h"
#include "data\data_sprt_16il.h"

u8 g_VBlank = 0;
u8 g_Frame = 0;

//-----------------------------------------------------------------------------
// FUNCTIONS

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
// PROGRAM ENTRY POINT
void main()
{
	// Address of Name Table in VRAM = 1400h
	// Address of Color Table in VRAM = 2000h
	// Address of Patern Table in VRAM = 0800h
	// Address of Sprite Attribute Table in VRAM = 1000h
	// Address of Sprite Pattern Table in VRAM = 0000h
	//VDP_SetModeGraphic1();
	VDP_SetMode(VDP_MODE_GRAPHIC1);
	VDP_EnableDisplay(false);
	VDP_SetColor(1);
	
	u8 chrSprt = 0;
	
	// Draw background
	VDP_FillVRAM(0xFF, 0x0800, 0, 256*8); // pattern
	VDP_FillVRAM(0x51, 0x2000 + 0, 0, 1); // color
	VDP_FillVRAM(0x41, 0x2000 + 1, 0, 1); // color
	for(u8 i = 0; i < 24; ++i)
		VDP_FillVRAM(i < 8 ? 0 : 8, 0x1400 + i * 32, 0, 32); // name

	// Sprite
	VDP_SetSpritePatternTable(0x0000);
	VDP_SetSpriteAttributeTable(0x1000);
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);

	for(u8 j = 0; j < 6; j++) // Pattern 0-95
	{
		for(u8 i = 0; i < 6; i++)
		{
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 +  0 + 24 * j) * 8, chrSprt++, 1);
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 + 12 + 24 * j) * 8, chrSprt++, 1);
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 +  1 + 24 * j) * 8, chrSprt++, 1);
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 + 13 + 24 * j) * 8, chrSprt++, 1);	
		}
	}
	VDP_SetSpriteSM1(0, 16, 16,  0, 0x01);
	VDP_SetSpriteSM1(1, 16, 16, 24, 0x0F);
	VDP_SetSpriteSM1(2, 16, 16, 48, 0x09);
	VDP_SetSpriteSM1(3, 0, 208, 0, 0); // hide

	VDP_EnableDisplay(true);

	g_VBlank = 0;
	g_Frame = 0;
	VDP_EnableVBlank(true);
	Bios_SetHookCallback(H_TIMI, VBlankHook);
	
	u8 X = 16;
	u8 Y = 88;
	
	bool bMoving = false;
	bool bContinue = true;
	while(bContinue)
	{
		u8 animId = bMoving ? ((g_Frame >> 2) % 6) * 4 : 16;
		bool bOdd = (g_Frame & 1) == 1;

		WaitVBlank();
		
		VDP_SetSpriteSM1(0, X, Y, 72 * bOdd + animId, 0x01);
		VDP_SetSpriteSM1(1, X, Y, 24 + animId, 0x0F);
		VDP_SetSpriteSM1(2, X, Y, 48 + animId, 0x09);
		
		if(Keyboard_IsKeyPressed(KEY_ESC))
			bContinue = false;

		u8 row = Keyboard_Read(KEY_ROW(KEY_DOWN));
		if((row & KEY_FLAG(KEY_RIGHT)) == 0)
		{
			X++;
			bMoving = true;
		}
		else if((row & KEY_FLAG(KEY_LEFT)) == 0)
		{
			X--;
			bMoving = true;
		}
		else
			bMoving = false;
			
		row = Keyboard_Read(KEY_ROW(KEY_1));
		if((row & KEY_FLAG(KEY_1)) == 0)
			VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
		else if((row & KEY_FLAG(KEY_2)) == 0)
			VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 + VDP_SPRITE_SCALE_2);
	}

	Bios_ClearHook(H_TIMI);
	Bios_Exit(0);
}