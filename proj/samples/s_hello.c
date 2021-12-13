// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
//  Hello world sample

#include "cmsx.h"

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_SCREEN0);
	VDP_ClearVRAM();

	Print_SetTextFont(PRINT_DEFAULT_FONT, 1);
	Print_DrawText("Hello MSX world!");

	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
	}
}