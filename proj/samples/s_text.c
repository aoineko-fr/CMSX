// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
#include "cmsx.h"

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

typedef struct 
{
	const c8* name;
	u8        mode;
	const u8* font;
} ModeData;

//-----------------------------------------------------------------------------
// DATA
//-----------------------------------------------------------------------------

// Fonts
#include "font\font_tsm9900.h"
#include "font\font_carwar.h"
#include "font\font_cmsx_std0.h"

// Sample text
const c8* g_SampleText =
	"Equation: (x+7)*42=(10h>>x)^2-3\n\n"
	"\"Nous sommes au 21e siecle ; toute la Gaule est occupee par les PC, Mac, Xbox, Switch et autres Playstation..."
	"Toute ? Non ! Car une association de fanatiques resiste encore et toujours a l'envahisseur, "
	"en proposant un site qui fleure bon les annees 80, "
	"entierement consacre au culte d'un standard fabuleux : le MSX !\"\n"
	"Bienvenue au MSX Village, le site des irreductibles Gaulois du MSX !";

// Animation characters
const u8 g_ChrAnim[] = { '|', '\\', '-', '/' };

// Modes data
const ModeData g_Modes[] =
{
	{ "Text1 (SC0 W40)", VDP_MODE_TEXT1,    g_Font_CMSX_Std0,  },
	{ "Graph1 (SC1)",    VDP_MODE_GRAPHIC1, g_Font_Carwar,     },
	{ "Graph2 (SC2)",    VDP_MODE_GRAPHIC2, g_Font_TMS9900,    },
#if (MSX_VERSION >= MSX_2)
	{ "Text2 (SC0 W80)", VDP_MODE_TEXT2,    g_Font_CMSX_Std0,  },
	{ "Graph3 (SC4)",    VDP_MODE_GRAPHIC3, g_Font_TMS9900,    },
#endif
};
u8 g_CurrentMode = 0;

//-----------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------

void DrawChar_Layout(u8 chr) __FASTCALL;


//-----------------------------------------------------------------------------
// Draw page
void DrawPage()
{
	VDP_SetMode(g_Modes[g_CurrentMode].mode);
	VDP_SetColor(0xF0);
	#if (MSX_VERSION >= MSX_2)
		VDP_EnableSprite(false);
	#endif
	// Set blank character
	VDP_FillVRAM(0, g_ScreenPatternLow, 0, 8);
	switch(VDP_GetMode())
	{
	#if (USE_VDP_MODE_G2)
		case VDP_MODE_GRAPHIC2:		// 256 x 192, the colour is specififed for each 8 dots
	#endif
	#if (USE_VDP_MODE_G3)
		case VDP_MODE_GRAPHIC3:		// GRAPHIC 2 which can use sprite mode 2
	#endif
	#if (USE_VDP_MODE_G2 || USE_VDP_MODE_G3)
		VDP_FillVRAM(0, g_ScreenColorLow, 0, 8);
		VDP_FillVRAM(0, g_ScreenColorLow + (256 * 8), 0, 8);
		VDP_FillVRAM(0, g_ScreenColorLow + (512 * 8), 0, 8);

		VDP_FillVRAM(0, g_ScreenPatternLow + (256 * 8), 0, 8);
		VDP_FillVRAM(0, g_ScreenPatternLow + (512 * 8), 0, 8);
		break;
	#endif
	};

	Print_SetTextFont(g_Modes[g_CurrentMode].font, 1);
	Print_SetColor(0xF, 0x0);

	Print_Clear();
	Print_SetPosition(0, 0);
	Print_DrawText("TEXT MODE SAMPLE - ");
	Print_DrawText(g_Modes[g_CurrentMode].name);
	Print_SetPosition(0, 1);
	Print_DrawCharX('-', g_PrintData.ScreenWidth);
	Print_SetPosition(0, 3);
	Print_DrawText(g_SampleText);

	Print_SetPosition(0, 23);
	Print_DrawText("<> Scr Mode ");
}

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	DrawPage();

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		if(Keyboard_IsKeyPressed(KEY_RIGHT))
		{
			g_CurrentMode++;
			g_CurrentMode %= numberof(g_Modes);
			DrawPage();
		}
		else if(Keyboard_IsKeyPressed(KEY_LEFT))
		{
			g_CurrentMode += numberof(g_Modes) - 1;
			g_CurrentMode %= numberof(g_Modes);
			DrawPage();
		}
		
		Print_SetPosition(g_PrintData.ScreenWidth-1, 0);
		u8 chr = count++ & 0x03;
		Print_DrawChar(g_ChrAnim[chr]);
	}

	Bios_Exit(0);
}