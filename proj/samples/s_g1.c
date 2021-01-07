//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "bios.h"
#include "bios_mainrom.h"
#include "vdp.h"
#include "input.h"

//-----------------------------------------------------------------------------
// DATA

// Fonts
#include "data\data_sprt_8.h"
#include "data\data_sprt_16.h"

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
	VDP_SetModeGraphic1();
	VDP_SetColor(1);
	for(u16 i = 0; i < 32*24; ++i)
		VDP_FillVRAM(i % 96, 0x1400 + i, 0, 1); // name
		
	//VDP_FillVRAM(0x01, 0x1400, 0, 32*24); // name
	
	VDP_FillVRAM(0xCC, 0x0800, 0, 256*8); // pattern
	VDP_FillVRAM(0x31, 0x2000, 0, 32); // color
	
	// Bios_SetHookCallback(H_TIMI, VBlankHook);

	// Load 8x8 sprites (Pattern 0~95)
	VDP_EnableDisplay(false);
	VDP_WriteVRAM(g_DataSprt8, 0x0800, 0, 96*8); // pattern
	VDP_EnableDisplay(true);
	
	bool bContinue = true;
	while(bContinue)
	{
		if(Keyboard_IsKeyPressed(KEY_ESC))
			bContinue = false;


		// WaitVBlank();
	}

	// Bios_ClearHook(H_TIMI);
	Bios_Exit(0);
}