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
#include "bios.h"
#include "bios_mainrom.h"
#include "draw.h"

// Inclide font data
#include "font/font_acme.h"
#include "font/font_carwar.h"
#include "font/font_cmsx_big1.h"
#include "font/font_cmsx_curs1.h"
#include "font/font_cmsx_curs1b.h"
#include "font/font_cmsx_mini1.h"
#include "font/font_cmsx_neon1.h"
#include "font/font_cmsx_neon1b.h"
#include "font/font_cmsx_neon2.h"
#include "font/font_cmsx_rune2.h"
#include "font/font_cmsx_rune2b.h"
#include "font/font_cmsx_std0.h"
#include "font/font_cmsx_std1.h"
#include "font/font_cmsx_std2.h"
#include "font/font_cmsx_std3.h"
#include "font/font_darkrose.h"
#include "font/font_ibm.h"
#include "font/font_oxygene.h"

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
	{ "C-MSX Neon 1B [8*8]",	g_Font_CMSX_Neon1B },
	{ "C-MSX Neon 2 [8*8]",		g_Font_CMSX_Neon2 },
	{ "C-MSX Rune 2 [8*8]",		g_Font_CMSX_Rune2 },
	{ "C-MSX Rune 2B [8*8]",	g_Font_CMSX_Rune2B },
	{ "C-MSX Standard 0 [6*8]",	g_Font_CMSX_Std0 },
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
	Print_SetColor(0xF, 0x0);
	Print_Clear();
	Print_SetFont(g_Font_CMSX_Std0);
	Print_SetCharSize(6, 8);
	Print_SetPosition(0, 0);
	Print_DrawText("PRINT SAMPLE\n");
	Draw_HLine(0, 255, 12, 0xFF, 0);
}

void PrintFooter()
{
	Print_SetColor(0xE, 0x0);
	Print_SetFont(g_Font_CMSX_Std0);
	Print_SetPosition(0, 204);
	Print_DrawText("F1:List  F2:Text  F3:FX  <>:Prev/Next ");
}

void PrintList()
{
	PrintHeader();

	Print_SetPosition(0, 24);
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
	Print_DrawText("Font: ");
	Print_DrawText(g_Fonts[g_FontIndex].Name);
	Print_SetFont(g_Fonts[g_FontIndex].Font);
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
	VDP_SetFrequency(VDP_FREQ_50HZ);
	VDP_EnableSprite(false);
	
	Print_Initialize(g_ScreenMode, null);
	Print_SetTabSize(16);
#if USE_PRINT_SHADOW
	Print_SetShadow(true, (i8)2, (i8)2, COLOR8_RED);
#endif

	//////////////////////////
    // static const char text1[]="BONJOUR LE FUTUR\n";
    // static const char text2[]="ICI LE MSX QUI VOUS PARLE\n";
    // static const char text3[]="DEPUIS L'ANNEE 1985\n";
    // static const char text4[]="IL PARAIT QUE LES VIEUX\n";
    // static const char text5[]="JOUENT ENCORE AVEC MOI\n";
    // static const char text6[]="DANS LES ANNEES 2020 ?\n";
    // static const char text7[]="Incroyable !\n";

	// Print_Clear();
	// // Print_DrawText("B");
	// // Print_SetPosition(0, 0);
	// u8 startTime = g_JIFFY;
	// Print_DrawText(text1);
	// Print_DrawText(text2);
	// Print_DrawText(text3);
	// Print_DrawText(text4);
	// Print_DrawText(text5);
	// Print_DrawText(text6);
	// Print_DrawText(text7);
	// u8 elapsedTime = g_JIFFY - startTime;
	// Print_DrawInt(elapsedTime);

	// while(!Keyboard_IsKeyPressed(KEY_SPACE)) {}
	//////////////////////////

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

	Bios_Exit(0);
}