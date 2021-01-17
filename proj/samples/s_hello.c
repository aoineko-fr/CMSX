//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "cmsx.h"

//-----------------------------------------------------------------------------
// Data

// Fonts
#include "font\font_cmsx_std0.h"
// Animation characters
const u8 g_ChrAnim[] = { '|', '\\', '-', '/' };

const u8 TestByte = 0xFF;
const u16 TestWord = 0xFFFF;
const c8 TestString[] = "ABCDEFG";

__at(0x7000) const u8 TestData[] = { 13, 24, 75, 96 };

__at(0xC000) u8 TestRAM[4];

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetScreen(VDP_MODE_SCREEN5);
	VDP_SetColor(0x4);
	VDP_CommandHMMV(0, 0, 256, 212, 0x44);

	Print_Initialize(VDP_MODE_SCREEN5, g_Font_CMSX_Std0);
	Print_SetPosition(3, 2);
	Print_DrawText("HELLO WORLD");
	Draw_Box(0, 0, 255, 16, 0, 0x0F);

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		Print_SetPosition(256/2-4, 212/2-4);
		u8 chr = count++ & 0x03;
		Print_DrawChar(g_ChrAnim[chr]);
	}

	Bios_Exit(0);
}