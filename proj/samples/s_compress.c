//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "cmsx.h"

#include "bios.h"
#include "input.h"
#include "compress.h"
#include "memory.h"

#include "data/rlep/lvl1.dat.h"
#include "data/rlep/lvl3.dat.h"
#include "data/rlep/lvl5.dat.h"

//-----------------------------------------------------------------------------
// Data

// Fonts
#include "font\font_cmsx_std0.h"
// Animation characters
const u8 g_ChrAnim[] = { '|', '\\', '-', '/' };

struct Entry
{
	const u8* data;
	const c8* name;
};

const struct Entry g_Tab[] = 
{ 
	{ lvl1_dat, "lvl1_dat" },
	{ lvl3_dat, "lvl3_dat" }, 
	{ lvl5_dat, "lvl5_dat" },
};

//
void DisplayRLEp(const struct Entry* entry)
{
	VDP_CommandHMMV(0, 0, 256, 212, 0x44);

	Print_SetPosition(4, 4);
	Print_DrawText("COMPRESS SAMPLE");
	Draw_Box(0, 0, 255, 14, 0x0F, 0);
	
	u8* dst = (u8*)Mem_HeapAlloc(72);	
	u16 size = UnpackRLEpToRAM(entry->data, dst, 72);
	
	Print_SetPosition(4, 24);
	Print_DrawText(entry->name);

	for(i8 i = 0; i < 36; ++i)
	{
		Print_SetPosition(8 + 20 * (i % 6), 40 + 9 * (i / 6));
		Print_DrawInt(dst[i]);

		Print_SetPosition(8 + 20 * (i % 6), 102 + 9 * (i / 6));
		Print_DrawInt(dst[36 + i]);
	}	

	Print_SetPosition(4, 200);
	Print_DrawInt((i16)size);
}


//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_SetColor(0x4);

	Print_SetBitmapFont(g_Font_CMSX_Std0);

	u8 tabIdx = 0;
	DisplayRLEp(&g_Tab[tabIdx]);

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		if(Keyboard_IsKeyPressed(KEY_SPACE))
		{
			tabIdx = (tabIdx + 1) % 3;
			DisplayRLEp(&g_Tab[tabIdx]);
		}
		
		Print_SetPosition(256-4-6, 4);
		u8 chr = count++ & 0x03;
		Print_DrawChar(g_ChrAnim[chr]);
	}

	Bios_Exit(0);
}