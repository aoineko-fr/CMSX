//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "cmsx.h"

//-----------------------------------------------------------------------------
// Define
#define DIGIT_Y		24
#define DIGIT_DATE	2
#define DIGIT_TIME	206

#define ANALOG_X	128
#define ANALOG_Y	120
#define ANALOG_R	50


//-----------------------------------------------------------------------------
// Data

#include "mathtable\mt_trigo_360.inc"

// Fonts
#include "font\font_cmsx_std0.h"
#include "font\font_cmsx_digit2.h"

// Animation characters
const u8 g_ChrAnim[] = { '|', '\\', '-', '/' };

const c8* g_DayOfWeek[] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",	
};

const c8* g_Month[] = {
	"January",
	"February",
	"March",            
	"April",
	"May",
	"June",
	"July",
	"August",       
	"September",
	"October",
	"November",
	"December",
};

u8 g_Sec;
u8 g_Min;
u8 g_Hour;
u8 g_PrevSec = 0xFF;
u8 g_PrevMin = 0xFF;
u8 g_PrevHour = 0xFF;

//-----------------------------------------------------------------------------
// Functions

///
void UpdateHour()
{
	Print_SetPosition(DIGIT_TIME+6*0, DIGIT_Y);
	Print_SetColor(0x8, 0x1);
	Print_DrawText("@@");
	
	Print_SetPosition(DIGIT_TIME+6*0, DIGIT_Y);
	Print_SetColor(0x9, 0x1);
	if(g_Hour < 10)
		Print_DrawChar('0');
	Print_DrawInt(g_Hour);		

	u16 idx = ((g_PrevHour + 45) * 6) % 360;	
	i16 dx = g_Cosinus360[idx] >> 8;
	i16 dy = g_Sinus360[idx] >> 8;
	Draw_Line(ANALOG_X, ANALOG_Y, ANALOG_X + dx, ANALOG_Y + dy, 0x0, 0);

	idx = ((g_Hour + 45) * 6) % 360;	
	dx = g_Cosinus360[idx] >> 8;
	dy = g_Sinus360[idx] >> 8;
	Draw_Line(ANALOG_X, ANALOG_Y, ANALOG_X + dx, ANALOG_Y + dy, 0xFF, 0);
}

///
void UpdateMinute()
{
	Print_SetPosition(DIGIT_TIME+6*3, DIGIT_Y);
	Print_SetColor(0x8, 0x1);
	Print_DrawText("@@");
	
	Print_SetPosition(DIGIT_TIME+6*3, DIGIT_Y);
	Print_SetColor(0x9, 0x1);
	if(g_Min < 10)
		Print_DrawChar('0');
	Print_DrawInt(g_Min);		

	u16 idx = ((g_PrevMin + 45) * 6) % 360;	
	i16 dx = g_Cosinus360[idx] >> 7;
	i16 dy = g_Sinus360[idx] >> 7;
	Draw_Line(ANALOG_X, ANALOG_Y, ANALOG_X + dx, ANALOG_Y + dy, 0x0, 0);

	idx = ((g_Min + 45) * 6) % 360;	
	dx = g_Cosinus360[idx] >> 7;
	dy = g_Sinus360[idx] >> 7;
	Draw_Line(ANALOG_X, ANALOG_Y, ANALOG_X + dx, ANALOG_Y + dy, 0xFF, 0);
	
	UpdateHour();
}

///
void UpdateSecond()
{
	Print_SetPosition(DIGIT_TIME+6*6, DIGIT_Y);
	Print_SetColor(0x8, 0x1);
	Print_DrawText("@@");
	
	Print_SetPosition(DIGIT_TIME+6*6, DIGIT_Y);
	Print_SetColor(0x9, 0x1);
	if(g_Sec < 10)
		Print_DrawChar('0');
	Print_DrawInt(g_Sec);
	
	u16 idx = ((g_PrevSec + 45) * 6) % 360;	
	i16 dx = g_Cosinus360[idx] >> 7;
	i16 dy = g_Sinus360[idx] >> 7;
	Draw_Line(ANALOG_X, ANALOG_Y, ANALOG_X + dx, ANALOG_Y + dy, 0x0, 0);

	idx = ((g_Sec + 45) * 6) % 360;	
	dx = g_Cosinus360[idx] >> 7;
	dy = g_Sinus360[idx] >> 7;
	Draw_Line(ANALOG_X, ANALOG_Y, ANALOG_X + dx, ANALOG_Y + dy, 0x9, 0);
	
	UpdateMinute();
}

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_EnableSprite(false);
	VDP_SetColor(0x1);
	VDP_SetPaletteEntry(8, RGB16(2, 0, 0));
	VDP_SetPaletteEntry(9, RGB16(7, 0, 0));

	Print_Initialize(g_Font_CMSX_Std0);
	Print_SetColor(0xFF, 0x11);
	Print_Clear();
	Print_SetPosition(4, 4);
	Print_DrawText("RTC SAMPLE");
	Draw_Box(0, 0, 255, 14, 0xF, 0);
	
	loop(i, 12)
	{
		u16 idx = (((i * 5) + 45) * 6) % 360;
		i16 dx = g_Cosinus360[idx] >> 6;
		i16 dy = g_Sinus360[idx] >> 6;
		Print_SetPosition(ANALOG_X + dx - 3, ANALOG_Y + dy - 4);
		Print_DrawInt((i == 0) ? 12 : i);
	}
	
	Clock_Initialize();

	Print_SetFont(g_Font_CMSX_Digit2);
	Print_SetMode(PRINT_MODE_TRANSPARENT);

	Print_SetColor(0x8, 0x1);
	Print_SetPosition(DIGIT_DATE, DIGIT_Y);
	Print_DrawCharX('@', 42);

	Print_SetColor(0x9, 0x1);
	Print_SetPosition(DIGIT_DATE, DIGIT_Y);
	Print_DrawText(g_DayOfWeek[Clock_GetDayOfWeek()]);
	Print_Space();
	Print_DrawText(g_Month[Clock_GetMonth() - 1]);
	Print_Space();
	Print_DrawInt(Clock_GetDay());
	Print_Space();
	Print_DrawInt(1980 + Clock_GetYear());

	Print_SetPosition(DIGIT_TIME+6*2, DIGIT_Y);
	Print_DrawChar(':');		
	Print_SetPosition(DIGIT_TIME+6*5, DIGIT_Y);
	Print_DrawChar(':');		

	u8 prevSec = 0xFF;
	u8 prevMin = 0xFF;
	u8 prevHour = 0xFF;
	
	Draw_Circle(ANALOG_X, ANALOG_Y, ANALOG_R, 0xF, 0);

	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		g_Hour = Clock_GetHour();
		if(g_Hour != g_PrevHour)
			UpdateHour();
		g_PrevHour = g_Hour;

		g_Min = Clock_GetMinute();
		if(g_Min != g_PrevMin)
			UpdateMinute();
		g_PrevMin = g_Min;

		g_Sec = Clock_GetSecond();
		if(g_Sec != g_PrevSec)
			UpdateSecond();
		g_PrevSec = g_Sec;
	}

	Bios_Exit(0);
}