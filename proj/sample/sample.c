//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "bios_main.h"
#include "keyboard.h"
#include "print.h"

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	Bios_Beep();
	
	Bios_ChangeMode(SCREEN_5);
	Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_BLACK);
	Bios_ClearScreen();

	SetPrintPos(8, 8);
	PrintText("CMSX SAMPLE");

	u8 count = 0;
	while(Bios_GetKeyboardMatrix(7) & KEY_ESC)
	{
		SetPrintPos(124, 102);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		u8 chr = count & 0x03;
		PrintChar(chrAnim[chr]);
		
		Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, count & 0x0F);
		count++;

		//VDP_WaitRetrace();
	}

	Bios_ChangeMode(SCREEN_0);
	Bios_ClearScreen();
}