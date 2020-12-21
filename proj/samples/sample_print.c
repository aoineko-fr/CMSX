//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "input.h"
#include "print.h"
#include "vdp.h"
#include "memory.h"

// Inclide font data
#include "fonts/font_acme.h"
#include "fonts/font_carwar.h"
#include "fonts/font_cmsx_big1.h"
#include "fonts/font_cmsx_curs1.h"
#include "fonts/font_cmsx_curs1b.h"
#include "fonts/font_cmsx_mini1.h"
#include "fonts/font_cmsx_neon1.h"
#include "fonts/font_cmsx_neon2.h"
#include "fonts/font_cmsx_std1.h"
#include "fonts/font_cmsx_std2.h"
#include "fonts/font_cmsx_std3.h"
#include "fonts/font_darkrose.h"
#include "fonts/font_ibm.h"
#include "fonts/font_oxygene.h"

const u8 g_ScreenMode = VDP_MODE_SCREEN5;
const u8* g_SampleText =
	" !\"#$%&'()*+,-./0123456789:;<=>?\n"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\n"
	"`abcdefghijklmnopqrstuvwxyz{|}~\n\n"
	"Equation: (x+7)*42=(10h>>x)^2-3\n\n"
	"\"Nous sommes au 21e siecle ; toute la Gaule est occupee par les PC, Mac, Xbox, Switch et autres Playstation..."
	"Toute ? Non ! Car une association de fanatiques resiste encore et toujours a l'envahisseur, "
	"en proposant un site qui fleure bon les annees 80, "
	"entierement consacre au culte d'un standard fabuleux : le MSX !\"";

struct FontEntry
{
	const c8* Name;
	const u8* Font;
};

struct FontEntry g_Fonts[] =
{
	{ "Main-ROM [6*8]",			null },
	{ "ACME [8*8]",				g_Font_Acme },
	{ "CARWAR [8*8]",			g_Font_Carwar },
	{ "C-MSX Big 1 [8*11]",		g_Font_CMSX_Big1 },
	{ "C-MSX Cursive 1 [8*8]",	g_Font_CMSX_Curs1 },
	{ "C-MSX Cursive 1B [8*8]",	g_Font_CMSX_Curs1B },
	{ "C-MSX Mini 1 [4*6]",		g_Font_CMSX_Mini1 },
	{ "C-MSX Neon 1 [8*8]",		g_Font_CMSX_Neon1 },
	{ "C-MSX Neon 2 [8*8]",		g_Font_CMSX_Neon2 },
	{ "C-MSX Standard 1 [6*8]",	g_Font_CMSX_Std1 },
	{ "C-MSX Standard 2 [6*8]",	g_Font_CMSX_Std2 },
	{ "C-MSX Standard 3 [6*8]",	g_Font_CMSX_Std3 },
	{ "Darkrose [8*8]",			g_Font_Darkrose },
	{ "IBM VGA [8*8]",			g_Font_IBM },
	{ "OXYGENE [8*8]",			g_Font_Oxygene },
};
static u8 g_FontIndex = 0;

void PrintHeader()
{
	Print_Clear();
	Print_SetCharSize(6, 8);
	Print_SetPosition(0, 0);
	Print_DrawText("PRINT SAMPLE\n");
	Print_DrawCharX('-', 42);
}

void PrintFooter()
{
	Print_SetPosition(0, 204);
	Print_SetFont(null);
	Print_DrawText("F1:List  F2:Text  F3:FX  <>:Prev/next ");
}

void PrintList()
{
	PrintHeader();

	Print_SetPosition(0, 24);
	Print_SetFont(null);
	Print_DrawText("List:\n\n");
	for(i8 i = 0; i < numberof(g_Fonts); i++)
	{
		Print_SetFont(g_Fonts[i].Font);
		Print_DrawText(g_Fonts[i].Name);
		Print_DrawText("\n");
	}

	PrintFooter();
}

void PrintSample()
{
	PrintHeader();

	Print_SetPosition(0, 24);
	Print_SetFont(null);
	Print_DrawText("Sample: ");

	Print_SetFont(g_Fonts[g_FontIndex].Font);
	Print_SetPosition(g_PrintData.CursorX, 24 + 8 - g_PrintData.UnitY);
	Print_DrawText(g_Fonts[g_FontIndex].Name);
	Print_SetPosition(0, 40);
	Print_DrawText(g_SampleText);

	PrintFooter();
}

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetScreen(g_ScreenMode);
	VDP_SetColor(0x00);
	
	Print_Initialize(g_ScreenMode, null);
	Print_SetTabSize(16);
#if USE_PRINT_SHADOW
	Print_SetShadow(true, (i8)2, (i8)2, COLOR8_RED);
#endif

	PrintList();


	Print_SetFont(null);
	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		if(Keyboard_IsKeyPressed(KEY_F1))
			PrintList();
		else if(Keyboard_IsKeyPressed(KEY_F2))
			PrintSample();

		if(Keyboard_IsKeyPressed(KEY_RIGHT))
		{
			if(g_FontIndex < numberof(g_Fonts) - 1)
				g_FontIndex++;
			else
				g_FontIndex = 0;			
			PrintSample();
		}
		else if(Keyboard_IsKeyPressed(KEY_LEFT))
		{
			if(g_FontIndex > 0)
				g_FontIndex--;
			else
				g_FontIndex = numberof(g_Fonts) - 1;			
			PrintSample();
		}

		Print_SetPosition(248, 0);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		Print_DrawChar(chrAnim[count++ & 0x03]);

		//VDP_WaitRetrace();*/
	}
}