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

// Data in bank 2 & 3
__at(0x8000) const c8 g_DataBank2[] = "Segment #2 (default in Bank #2)";
__at(0xA000) const c8 g_DataBank3[] = "Segment #3 (default in Bank #3)";

// Character animation data
const c8 g_ChrAnim[] = { '|', '\\', '-', '/' };

// Hexadecimal characters
const c8 hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };


//=============================================================================
// MEMORY DATA
//=============================================================================

u8 g_Buffer[128];
u8 biosSlot = 0xFF;
u8 cartSlot = 0xFF;
u8 pageSlot[4];
u8 g_DisplayASCII = false;
u16 g_Address = 0xA000;

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

//-----------------------------------------------------------------------------
//
const c8* GetTargetType(u8 target)
{
	switch(target)
	{
	case TYPE_BIN: return "BASIC";
	case TYPE_ROM: return "ROM";
	case TYPE_DOS: return "DOS";
	};
	return "Unknow";
}

//-----------------------------------------------------------------------------
//
const c8* GetROMMapper(u8 mapper)
{
	switch(mapper)
	{
	case ROM_PLAIN:      return "Plain";
	case ROM_ASCII8:     return "ASCII8";
	case ROM_ASCII16:    return "ASCII16";
	case ROM_KONAMI:     return "Konami";
	case ROM_KONAMI_SCC: return "Konami SCC";
	};
	return "Unknow";
}

//-----------------------------------------------------------------------------
//
const c8* GetROMSize(u8 size)
{
	switch(size)
	{
	case ROM_8K:	return "8K";
	case ROM_16K:	return "16K";
	case ROM_32K:	return "32K";
	case ROM_48K:	return "48K";
	case ROM_64K:	return "64K";
	case ROM_128K:	return "128K";
	case ROM_256K:	return "256K";
	case ROM_512K:	return "512K";
	case ROM_1M:	return "1M";
	case ROM_2M:	return "2M";
	case ROM_4M:	return "4M";
	};
	return "Unknow";
}

//-----------------------------------------------------------------------------
//
void DiplayPage()
{
	// VDP_ClearVRAM();
	Print_Clear();

	Print_SetTextFont(g_Font_CMSX_Sample6, 1);
	Print_DrawText(MSX_GL "   ASCII-8 Cartridge Sample\n");
	Print_DrawLineH(0, 1, 40);

	Print_SetPosition(0, 2);
	Print_DrawText("Target: ");
	Print_DrawInt(TARGET);

	Print_SetPosition(0, 3);
	Print_DrawText("Type:   ");
	Print_DrawInt(TARGET_TYPE);
	Print_DrawText("-");
	Print_DrawText(GetTargetType(TARGET_TYPE));
	
	Print_SetPosition(20, 2);
	Print_DrawText("Mapper: ");
	Print_DrawInt(ROM_MAPPER);
	Print_DrawText("-");
	Print_DrawText(GetROMMapper(ROM_MAPPER));

	Print_SetPosition(20, 3);
	Print_DrawText("Size:   ");
	Print_DrawInt(ROM_SIZE);
	Print_DrawText("-");
	Print_DrawText(GetROMSize(ROM_SIZE));
	
	Print_SetPosition(0, 5);
	Print_DrawText("Segments in Bank #3     \x81 ");
	Print_DrawHex16(g_Address);
	Print_DrawText(" \x80\n");
	for(u8 i = 0; i < 16; i++)
	{
		Print_DrawText(" [");
		if(i < 10)
			Print_DrawChar(' ');
		Print_DrawInt(i);
		Print_DrawText("] ");
		SET_BANK_SEGMENT(3, i);
		u8 max = g_DisplayASCII ? 34 : 11;
		for(u8 j = 0; j < max; j++)
		{
			u8 chr = ((const u8*)(g_Address))[j];
			if(g_DisplayASCII)
				Print_DrawChar(chr);
			else
			{
				Print_DrawChar(hexChar[(chr >> 4) & 0x000F]);
				Print_DrawChar(hexChar[chr & 0x000F]);
				Print_DrawChar(' ');
			}
		}
		Print_Return();
	}

	Print_DrawLineH(0, 22, 40);

	Print_SetPosition(0, 23);
	Print_DrawText("F1:Hexa F2:ASCII \x81\x80:Addr");
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
	
	DiplayPage();

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		Print_SetPosition(39, 0);
		Print_DrawChar(g_ChrAnim[count++ & 0x03]);
		
		// Handle user input
		u8 row6 = Keyboard_Read(6);
		if(IS_KEY_PRESSED(row6, KEY_F1))
		{
			g_DisplayASCII = false;
			DiplayPage();
		}
		if(IS_KEY_PRESSED(row6, KEY_F2))
		{
			g_DisplayASCII = true;
			DiplayPage();
		}

		// Handle user input
		u8 row8 = Keyboard_Read(8);
		if(IS_KEY_PRESSED(row8, KEY_RIGHT))
		{
			g_Address++;
			DiplayPage();
		}
		if(IS_KEY_PRESSED(row8, KEY_LEFT))
		{
			g_Address--;
			DiplayPage();
		}


		Halt();
	}
}