// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
//  Real-time clock module sample

//=============================================================================
// INCLUDES
//=============================================================================
#include "cmsx.h"

//=============================================================================
// DEFINES
//=============================================================================

// Graph base position
#define GRAPH_X			16
#define GRAPH_Y			48
#define GRAPH_U			4

// Random sample count
#define RAND_SAMPLE		0x4000 // 16384

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Fonts
#include "font\font_cmsx_sample6.h"

// Character animation
const u8 chrAnim[] = { '|', '\\', '-', '/' };

//=============================================================================
// MEMORY DATA
//=============================================================================

// Screen mode setting index
u8 g_VBlank = 0;
u16 g_Frame = 0;

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
// H_TIMI interrupt hook
void VBlankHook()
{
	g_VBlank = 1;
	g_Frame++;
}

//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{
	while(g_VBlank == 0) {}
	g_VBlank = 0;
}

//-----------------------------------------------------------------------------
// Diplay page header
void DisplayHeader(const c8* title)
{
	// Setup screen
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_SetColor(COLOR_DARK_BLUE);
	VDP_CommandHMMV(0, 0, 256, 1024, COLOR_MERGE2(COLOR_DARK_BLUE)); // Clear VRAM
	VDP_CommandWait();

	// Setup print
	Print_SetBitmapFont(g_Font_CMSX_Sample6);
	Print_SetColor(COLOR_WHITE, COLOR_DARK_BLUE);
	Print_SetPosition(0, 2);
	Print_DrawText("\x1\x2\x3\x4\x5\x6   MATH SAMPLE - ");
	Print_DrawText(title);
	Draw_HLine(0, 255, 12, COLOR_WHITE, 0);
}


//-----------------------------------------------------------------------------
// Diplay page footer
void DisplayFooter()
{
	Draw_HLine(0, 255, 199, COLOR_WHITE, 0);
	Print_SetColor(COLOR_WHITE, COLOR_DARK_BLUE);
	Print_SetPosition(0, 203);
	Print_DrawText("F1:Rnd8 F2:Rnd16");
}

//-----------------------------------------------------------------------------
// Diplay page footer
void DisplayRandom16()
{
	DisplayHeader("Random 16b");
	
	Print_SetPosition(0, 20);
	Print_DrawText("Method: ");
	Print_DrawText(RANDOM_16_NAME);

	Draw_VLine(GRAPH_X, GRAPH_Y - GRAPH_U, 128 + GRAPH_Y + GRAPH_U, COLOR_LIGHT_BLUE, 0);
	Draw_HLine(GRAPH_X - GRAPH_U, 255 - (GRAPH_X - GRAPH_U), 128 + GRAPH_Y, COLOR_LIGHT_BLUE, 0);
	Draw_VLine(GRAPH_X + 128, 128 + GRAPH_Y - GRAPH_U, 128 + GRAPH_Y + GRAPH_U, COLOR_LIGHT_BLUE, 0);
	Draw_HLine(GRAPH_X - GRAPH_U, GRAPH_X + GRAPH_U, GRAPH_Y, COLOR_LIGHT_BLUE, 0);

	Print_SetColor(COLOR_LIGHT_BLUE, COLOR_DARK_BLUE);
	Print_SetPosition(168, GRAPH_Y);
	Print_DrawText("0 hit");
	Print_SetPosition(168, GRAPH_Y + 8 * 5);
	Print_DrawText("5+ hits");

	VDP_CommandHMMV(160, GRAPH_Y + 8 * 1, 2, 8, COLOR_MERGE2(COLOR_LIGHT_BLUE));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 2, 2, 8, COLOR_MERGE2(COLOR_CYAN));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 3, 2, 8, COLOR_MERGE2(COLOR_LIGHT_GREEN));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 4, 2, 8, COLOR_MERGE2(COLOR_LIGHT_YELLOW));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 5, 2, 8, COLOR_MERGE2(COLOR_LIGHT_RED));

	u16 startFrame = g_Frame;
	for(u16 i = 0; i < RAND_SAMPLE; ++i)
	{
		u16 rand = Math_GetRandom16();
		u16 x = GRAPH_X + ((rand % 0x00FF) / 2);
		u16 y = GRAPH_Y + 128 - ((rand >> 8) / 2);
		switch(VDP_CommandPOINT(x, y))
		{
		case COLOR_DARK_BLUE:		VDP_CommandPSET(x, y, COLOR_LIGHT_BLUE, 0); break;
		case COLOR_LIGHT_BLUE:		VDP_CommandPSET(x, y, COLOR_CYAN, 0); break;
		case COLOR_CYAN:			VDP_CommandPSET(x, y, COLOR_LIGHT_GREEN, 0); break;
		case COLOR_LIGHT_GREEN:  	VDP_CommandPSET(x, y, COLOR_LIGHT_YELLOW, 0); break;
		default: 					VDP_CommandPSET(x, y, COLOR_LIGHT_RED, 0); break;
		};
		
	}
	
	Print_SetPosition(0, 189);
	Print_DrawText("t=");
	Print_DrawInt(g_Frame - startFrame);	
	
	DisplayFooter();
}

//-----------------------------------------------------------------------------
// Diplay page footer
void DisplayRandom8()
{
	DisplayHeader("Random 8b");
	
	Print_SetPosition(0, 20);
	Print_DrawText("Method: ");
	Print_DrawText(RANDOM_8_NAME);

	Draw_VLine(GRAPH_X, GRAPH_Y - GRAPH_U, 128 + GRAPH_Y + GRAPH_U, COLOR_LIGHT_BLUE, 0);
	Draw_HLine(GRAPH_X - GRAPH_U, 255 - (GRAPH_X - GRAPH_U), 128 + GRAPH_Y, COLOR_LIGHT_BLUE, 0);
	Draw_VLine(GRAPH_X + 128, 128 + GRAPH_Y - GRAPH_U, 128 + GRAPH_Y + GRAPH_U, COLOR_LIGHT_BLUE, 0);
	Draw_HLine(GRAPH_X - GRAPH_U, GRAPH_X + GRAPH_U, GRAPH_Y, COLOR_LIGHT_BLUE, 0);

	Print_SetColor(COLOR_LIGHT_BLUE, COLOR_DARK_BLUE);
	Print_SetPosition(168, GRAPH_Y);
	Print_DrawText("0 hit");
	Print_SetPosition(168, GRAPH_Y + 8 * 5);
	Print_DrawText("5+ hits");

	VDP_CommandHMMV(160, GRAPH_Y + 8 * 1, 2, 8, COLOR_MERGE2(COLOR_LIGHT_BLUE));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 2, 2, 8, COLOR_MERGE2(COLOR_CYAN));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 3, 2, 8, COLOR_MERGE2(COLOR_LIGHT_GREEN));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 4, 2, 8, COLOR_MERGE2(COLOR_LIGHT_YELLOW));
	VDP_CommandHMMV(160, GRAPH_Y + 8 * 5, 2, 8, COLOR_MERGE2(COLOR_LIGHT_RED));

	u16 startFrame = g_Frame;
	for(u16 i = 0; i < RAND_SAMPLE; ++i)
	{
		u16 x = GRAPH_X + (Math_GetRandom8() / 2);
		u16 y = GRAPH_Y + 128 - (Math_GetRandom8() / 2);
		switch(VDP_CommandPOINT(x, y))
		{
		case COLOR_DARK_BLUE:		VDP_CommandPSET(x, y, COLOR_LIGHT_BLUE, 0); break;
		case COLOR_LIGHT_BLUE:		VDP_CommandPSET(x, y, COLOR_CYAN, 0); break;
		case COLOR_CYAN:			VDP_CommandPSET(x, y, COLOR_LIGHT_GREEN, 0); break;
		case COLOR_LIGHT_GREEN:  	VDP_CommandPSET(x, y, COLOR_LIGHT_YELLOW, 0); break;
		default: 					VDP_CommandPSET(x, y, COLOR_LIGHT_RED, 0); break;
		};
		
	}
	
	Print_SetPosition(0, 187);
	Print_DrawText("t=");
	Print_DrawInt(g_Frame - startFrame);	
	
	DisplayFooter();
}

//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	DisplayRandom16();
	
	bool bContinue = true;
	while(bContinue)
	{
		WaitVBlank();
		
		Print_SetPosition(248, 2);
		Print_DrawChar(chrAnim[g_Frame & 0x03]);

		u8 row6 = Keyboard_Read(6);
		if(IS_KEY_PRESSED(row6, KEY_F1))
			DisplayRandom8();
		else if(IS_KEY_PRESSED(row6, KEY_F2))
			DisplayRandom16();


		if(Keyboard_IsKeyPressed(KEY_ESC))
			bContinue = false;
	}

	Bios_ClearHook(H_TIMI);
	Bios_Exit(0);
}


