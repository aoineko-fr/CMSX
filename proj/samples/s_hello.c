//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "cmsx.h"

#include "bios.h"
#include "input.h"

//-----------------------------------------------------------------------------
// Data

// Fonts
#include "font\font_cmsx_std0.h"
// Animation characters
const u8 g_ChrAnim[] = { '|', '\\', '-', '/' };

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
//	TestFunc();

	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_SetColor(0x4);
	VDP_CommandHMMV(0, 0, 256, 212, 0x44);

	Print_SetBitmapFont(g_Font_CMSX_Std0);
	Print_SetPosition(4, 4);
	Print_DrawText("HELLO WORLD");
	Draw_Box(0, 0, 255, 16, 0x0F, 0);

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		Print_SetPosition(256/2-4, 212/2-4);
		u8 chr = count++ & 0x03;
		Print_DrawChar(g_ChrAnim[chr]);
	}

	Bios_Exit(0);
}