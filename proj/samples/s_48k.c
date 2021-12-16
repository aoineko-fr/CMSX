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
__at(0x0000) const c8 g_Page0Data[] = "You got Page #0 data!";

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
	Print_DrawText(MSX_GL "   48K Cartridge Sample\n");
	Print_DrawLineH(0, 1, 40);
	
	u8 biosSlot = Sys_GetPageSlot(0); // Get slot in page 0, the Main-ROM
	u8 cartSlot = Sys_GetPageSlot(1); // Get slot in page 1, the cartridge
	u8 ramSlot = Sys_GetPageSlot(3); // Get slot in page 1, the cartridge
	
	Print_SetPosition(0, 2);
	Print_DrawText("\nPage #0: ");
	Print_Slot(biosSlot);
	Print_DrawText(" (BIOS)\nPage #1: ");
	Print_Slot(cartSlot);
	Print_DrawText(" (Cart)\nPage #2: ");
	Print_Slot(Sys_GetPageSlot(2));
	Print_DrawText(" (Cart)\nPage #3: ");
	Print_Slot(ramSlot);
	Print_DrawText(" (RAM)");
	
	
	Print_DrawText("\n\nSwitching page 0 and copy data to RAM...\n");
	DisableInterrupt();
	Sys_SetPage0Slot(cartSlot);
	////////// Catridge Slot in Page 0 //////////
	{
		// pageSlot[0] = Sys_GetPageSlot(0);
		// pageSlot[1] = Sys_GetPageSlot(1);
		// pageSlot[2] = Sys_GetPageSlot(2);
		// pageSlot[3] = Sys_GetPageSlot(3);
		Mem_Copy(g_Page0Data, g_Buffer, sizeof(g_Page0Data)); // Copy page 0 cartridge data to RAM

		Sys_SetPage0Slot(biosSlot);
	}
	////////// BIOS Slot in Page 0 //////////
	EnableInterrupt();	
	Print_DrawText("\n... page 0 restored!");

	// Print_DrawText("\n\nPage #0: ");
	// Print_Slot(pageSlot[0]);
	// Print_DrawText("\nPage #1: ");
	// Print_Slot(pageSlot[1]);
	// Print_DrawText("\nPage #2: ");
	// Print_Slot(pageSlot[2]);
	// Print_DrawText("\nPage #3: ");
	// Print_Slot(pageSlot[3]);

	Print_DrawText("\n\nData: ");
	Print_DrawText(g_Buffer);
	
	Bios_Beep();

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		Print_SetPosition(39, 0);
		Print_DrawChar(g_ChrAnim[count++ & 0x03]);
		
		Halt();
	}
}