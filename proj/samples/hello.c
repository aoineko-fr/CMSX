//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "bios_main.h"
#include "input.h"
#include "print.h"

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	Bios_Beep();
	
	g_LINL40 = 40;
	Bios_ChangeMode(SCREEN_0);
	Bios_ChangeColor(COLOR_WHITE, COLOR_DARK_BLUE, COLOR_DARK_BLUE);
	Bios_ClearScreen();

	PrintBox(1, 1, 20, 3);
	SetPrintPos(3, 2);
	PrintText("HELLO WORLD");

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		SetPrintPos(g_LINL40 / 2, 12);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		u8 chr = count++ & 0x03;
		PrintChar(chrAnim[chr]);

		//VDP_WaitRetrace();*/
	}

	Bios_ChangeMode(SCREEN_0);
	Bios_ClearScreen();
}