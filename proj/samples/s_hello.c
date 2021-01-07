//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma sdcc_hash +

#include "core.h"
#include "vdp.h"
#include "draw.h"
#include "print.h"
#include "input.h"

//-----------------------------------------------------------------------------
// Data

// Fonts
#include "font\font_cmsx_std0.h"
// Animation characters
static const u8 g_ChrAnim[] = { '|', '\\', '-', '/' };

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
}