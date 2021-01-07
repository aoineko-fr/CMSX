//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// Joystick sample program
//-----------------------------------------------------------------------------

#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "bios_main.h"
#include "print.h"
#include "input.h"

void MainLoop();

void main()
{
	MainLoop();
}

typedef struct
{
	u8 code;
	u8 posX, posY;
	const c8* text;
} KeyEntry;

const KeyEntry keys[] =
{
	{ KEY_F1,		3,		5,	"F1" },
	{ KEY_F2,		6,		5,	"F2" },
	{ KEY_F3,		9,		5,	"F3" },
	{ KEY_F4,		12,		5,	"F4" },
	{ KEY_F5,		15,		5,	"F5" },
	{ KEY_SELECT,	20,		5,	"SEL" },
	{ KEY_STOP,		24,		5,	"STP" },
	{ KEY_HOME,		28,		5,	"HOM" },
	{ KEY_INS,		32,		5,	"INS" },
	{ KEY_DEL,		36,		5,	"DEL" },
	
	{ KEY_ESC,		3,		6,	"ESC" },
	{ KEY_1,		7,		6,	"1" },
	{ KEY_2,		9,		6,	"2" },
	{ KEY_3,		11,		6,	"3" },
	{ KEY_4,		13,		6,	"4" },
	{ KEY_5,		15,		6,	"5" },
	{ KEY_6,		17,		6,	"6" },
	{ KEY_7,		19,		6,	"7" },
	{ KEY_8,		21,		6,	"8" },
	{ KEY_9,		23,		6,	"9" },
	{ KEY_0,		25,		6,	"0" },
	{ KEY_BS,		33,		6,	"BS" },

	{ KEY_TAB,		3,		7,	"TAB" },
	{ KEY_Q,		8,		7,	"Q" },
	{ KEY_W,		10,		7,	"W" },
	{ KEY_E,		12,		7,	"E" },
	{ KEY_R,		14,		7,	"R" },
	{ KEY_T,		16,		7,	"T" },
	{ KEY_Y,		18,		7,	"Y" },
	{ KEY_U,		20,		7,	"U" },
	{ KEY_I,		22,		7,	"I" },
	{ KEY_O,		24,		7,	"O" },
	{ KEY_P,		26,		7,	"P" },
	{ KEY_RET,		33,		7,	"RET" },
	
	{ KEY_CTRL,		3,		8,	"CTRL" },
	{ KEY_A,		9,		8,	"A" },
	{ KEY_S,		11,		8,	"S" },
	{ KEY_D,		13,		8,	"D" },
	{ KEY_F,		15,		8,	"F" },
	{ KEY_G,		17,		8,	"G" },
	{ KEY_H,		19,		8,	"H" },
	{ KEY_J,		21,		8,	"J" },
	{ KEY_K,		23,		8,	"K" },
	{ KEY_L,		25,		8,	"L" },
	
	{ KEY_SHIFT,	3,		9,	"SHIFT" },
	{ KEY_Z,		10,		9,	"Z" },
	{ KEY_X,		12,		9,	"X" },
	{ KEY_C,		14,		9,	"C" },
	{ KEY_V,		16,		9,	"V" },
	{ KEY_B,		18,		9,	"B" },
	{ KEY_N,		20,		9,	"N" },
	{ KEY_M,		22,		9,	"M" },
	{ KEY_SHIFT,	32,		9,	"SHIFT" },

	{ KEY_CAPS,		6,		10,	"CAPS" },
	{ KEY_GRAPH,	11,		10,	"GRPH" },
	{ KEY_SPACE,	19,		10,	"SPACE" },
	{ KEY_CODE,		28,		10,	"CODE" },

	{ KEY_UP,		16,		13,	"^" },
	{ KEY_LEFT,		14,		14,	"<" },
	{ KEY_RIGHT,	18,		14,	">" },
	{ KEY_DOWN,		16,		15,	"v" },
};

u8 keyPressed[numberof(keys)];

u8 LogX, LogY;
void InputEvent(u8 dev, u8 in, u8 evt)
{
	if(LogY >= 24)
	{
		for(i8 i = 18; i < 24; i++) // clear
		{
			SetPrintPos(1, i);
			PrintCharX(' ', 40);
		}
		LogX = 1;
		LogY = 18;
	}

	SetPrintPos(LogX, LogY);
	
	PrintChar('D');
	PrintInt(dev);
	PrintChar('.');
	switch(in)
	{
	case IPM_INPUT_STICK:    PrintText("S"); break;
	case IPM_INPUT_BUTTON_A: PrintText("A"); break;
	case IPM_INPUT_BUTTON_B: PrintText("B"); break;
	}
	PrintChar(':');
	PrintText(IPM_GetEventName(evt));
	PrintChar(' ');
	
	LogX = g_CSRX;
	LogY = g_CSRY;
}

//-----------------------------------------------------------------------------
/** Program entry point */
void MainLoop()
{
	// HeapAddress = _HEAPADDR;
	
	Bios_Beep();
	
	g_LINL40 = 40;
	Bios_ChangeMode(SCREEN_0);
	Bios_ChangeColor(COLOR_WHITE, COLOR_DARK_BLUE, COLOR_DARK_BLUE);
	Bios_ClearScreen();

	IPM_Initialize(null);
	IPM_RegisterEvent(IPM_DEVICE_ANY, IPM_INPUT_ANY, IPM_EVENT_CLICK, InputEvent);
	IPM_RegisterEvent(IPM_DEVICE_ANY, IPM_INPUT_ANY, IPM_EVENT_HOLD, InputEvent);
	IPM_RegisterEvent(IPM_DEVICE_ANY, IPM_INPUT_ANY, IPM_EVENT_DOUBLE_CLICK, InputEvent);
	IPM_RegisterEvent(IPM_DEVICE_ANY, IPM_INPUT_ANY, IPM_EVENT_DOUBLE_CLICK_HOLD, InputEvent);

	PrintBox(1, 1, 40, 3);
	SetPrintPos(3, 2);
	PrintText("KEYBOARD SAMPLE");

	PrintBox(1, 4, 40, 11);
	SetPrintPos(3, 5);
	PrintText("F1 F2 F3 F4 F5   SEL STO HOM INS DEL");
	SetPrintPos(3, 6);
	PrintText("ESC 1 2 3 4 5 6 7 8 9 0 x x x BS");
	SetPrintPos(3, 7);
	PrintText("TAB  Q W E R T Y U I O P x x  RET");
	SetPrintPos(3, 8);
	PrintText("CTRL  A S D F G H J K L x x x");
	SetPrintPos(3, 9);
	PrintText("SHIFT  Z X C V B N M x x x x SHIFT");
	SetPrintPos(3, 10);
	PrintText("   CAPS GRPH    SPACE    CODE");

	PrintBox(1, 12, 11, 17);
	SetPrintPos(3, 13);
	PrintText("7 8 9 -");
	SetPrintPos(3, 14);
	PrintText("4 5 6 /");
	SetPrintPos(3, 15);
	PrintText("1 2 3 *");
	SetPrintPos(3, 16);
	PrintText("0 , . +");

	PrintBox(12, 12, 20, 16);
	SetPrintPos(14, 13);
	PrintText("  ^");
	SetPrintPos(14, 14);
	PrintText("<   >");
	SetPrintPos(14, 15);
	PrintText("  v");

	LogX = 1;
	LogY = 18;

	i8 i;
	for(i = 0; i < numberof(keys); i++)
		keyPressed[i] = 1;

	u8 count = 0;
	u8 loop = 1;
	c8 start, end;
	while(loop)
	{
		IPM_Update();
		
		for(i = 0; i < numberof(keys); i++)
		{
			u8 pressed = Keyboard_IsKeyPressed(keys[i].code);
			if(keyPressed[i] != pressed)
			{
				keyPressed[i] = pressed;
				start = end = ' ';
				if(pressed)
				{
					start = '[';
					end = ']';
				}
				SetPrintPos(keys[i].posX-1, keys[i].posY);
				PrintChar(start);
				PrintText(keys[i].text);
				PrintChar(end);
			}
		}

		for(i = 0; i < 11; i++)
		{
			if((i % 4) == 0)
				SetPrintPos(22, 13 + (i / 4));
			PrintHex8(Keyboard_Read(i));
			PrintChar(' ');
		}
		
		if(Keyboard_IsKeyPressed(KEY_ESC) && Keyboard_IsKeyPressed(KEY_DEL))
		{
			loop = 0;
		}


		SetPrintPos(g_LINL40 - 2, 2);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		u8 chr = count++ & 0x03;
		PrintChar(chrAnim[chr]);
		
		//VDP_WaitRetrace();
	}

	Bios_ClearScreen();
}