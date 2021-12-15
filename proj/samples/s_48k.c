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

//=============================================================================
// READ-ONLY DATA
//=============================================================================

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

	Print_SetTextFont(PRINT_DEFAULT_FONT, 1);
	Print_DrawText("MGL - 48K Cartridge Sample\n");
	Print_DrawLineH(0, 1, 40);
	
	u8 biosSlot = Sys_GetPageSlot(0); // Get slot in page 0, the Main-ROM
	u8 cartSlot = Sys_GetPageSlot(1); // Get slot in page 1, the cartridge
	
	Print_SetPosition(0, 2);
	Print_DrawText("\nBIOS slot: ");
	Print_Slot(biosSlot);
	Print_DrawText("\nCart slot: ");
	Print_Slot(cartSlot);
	
	Print_DrawText("\n\nSwitching page 0 and copy data to RAM...\n");
	DisableInterrupt();
	Sys_SetPage0Slot(cartSlot);
	// Catridge Slot in Page 0
	{
		Mem_Copy(g_Page0Data, g_Buffer, sizeof(g_Page0Data)); // Copy page 0 cartridge data to RAM

		Sys_SetPage0Slot(biosSlot);
	}
	// EnableInterrupt();	
	Print_DrawText("\n... page 0 restored\n\nData: ");
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