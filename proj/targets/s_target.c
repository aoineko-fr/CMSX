// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
//  48K ROM sample (page 0 still pointing to Main-ROM)

//=============================================================================
// INCLUDES
//=============================================================================
#include "cmsx.h"

//=============================================================================
// DEFINES
//=============================================================================

// Library's logo
#define MSX_GL "\x01\x02\x03\x04\x05\x06"

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Fonts data
#include "font\font_cmsx_sample6.h"

// Sample data in page 0
// __at(0x0000) const c8 g_Page0Data[] = "You got Page #0 data!";

// Character animation data
const c8 g_ChrAnim[] = { '|', '\\', '-', '/' };

//=============================================================================
// MEMORY DATA
//=============================================================================

u8 g_Buffer[128];
u8 biosSlot = 0xFF;
u8 cartSlot = 0xFF;
u8 pageSlot[4];

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
//
void Print_Slot(u8 slot)
{
	Print_DrawInt(Sys_SlotGetPrimary(slot));
	if(Sys_SlotIsExpended(slot))
	{
		Print_DrawChar('-');
		Print_DrawInt(Sys_SlotGetSecondary(slot));
	}
}

//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_SCREEN0);
	VDP_EnableVBlank(true);
	VDP_ClearVRAM();

	Print_SetTextFont(g_Font_CMSX_Sample6, 1);
	Print_DrawText(MSX_GL "   Target sample\n");
	Print_DrawLineH(0, 1, 40);
	
	Print_SetPosition(0, 2);
	Print_DrawText("\nTraget: ");
	Print_DrawText(TARGET_NAME);
	Print_Return();
	Print_Return();

	Print_DrawText("\nPage #0 -> Slot ");
	Print_Slot(Sys_GetPageSlot(0));
	Print_DrawText("\nPage #1 -> Slot ");
	Print_Slot(Sys_GetPageSlot(1));
	Print_DrawText("\nPage #2 -> Slot ");
	Print_Slot(Sys_GetPageSlot(2));
	Print_DrawText("\nPage #3 -> Slot ");
	Print_Slot(Sys_GetPageSlot(3));



	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		Print_SetPosition(39, 0);
		Print_DrawChar(g_ChrAnim[count++ & 0x03]);
		
		Halt();
	}
}