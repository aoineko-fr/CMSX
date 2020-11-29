//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------

#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "bios_main.h"
#include "video.h"
#include "print.h"
#include "mutex.h"
#include "msxi/msxi_unpack.h"
#include "joystick.h"

//-----------------------------------------------------------------------------
// D E F I N E S
#define VERSION "V0.1.0"

//-----------------------------------------------------------------------------
// R A M   D A T A

// Game data
extern VDP vdp;

//=============================================================================
//
//   C O D E
//
//=============================================================================

void MainLoop();

//-----------------------------------------------------------------------------
/** Program entry point */
void main()
{
	MainLoop();
}

#include "data/player.data.h"

u8 timer;

u8 vdp_status;

u8 g_Mutex;

void PrintString(c8* str)
{
	i8 i = 0;
	while(str[i] != 0)
		Bios_GraphPrintChar(str[i++]);
}

JSM_ALLOC_DATA()

void JoystickEvent(const JSM_Event* event)
{
	if(event->InputId == JSM_INPUT_BUTTON_A)
	{
		SetPrintPos(8, 8);
		PrintChar((event->EventId == JSM_EVENT_CLICK) ? 'O' : 'X');
	}
	else if(event->InputId == JSM_INPUT_BUTTON_B)
	{
		SetPrintPos(16, 8);
		PrintChar((event->EventId == JSM_EVENT_CLICK) ? 'O' : 'X');
	}
}

/*void JoystickEvent(u8 id, u8 event, u8 value)
{	
	if(id == 0) // player 1
	{
		if(event == JSM_EVENT_TRIGGER_A)
		{
			SetPrintPos(8, 8);
			PrintChar((value == JSM_TRIGGER_PRESSED) ? 'O' : 'X');
		}
		else if(event == JSM_EVENT_TRIGGER_B)
		{
			SetPrintPos(16, 8);
			PrintChar((value == JSM_TRIGGER_PRESSED) ? 'O' : 'X');
		}
	}
	else // player 2
	{
		if(event == JSM_EVENT_TRIGGER_A)
		{
			SetPrintPos(8, 16);
			PrintChar((value == JSM_TRIGGER_PRESSED) ? 'O' : 'X');
		}
		else if(event == JSM_EVENT_TRIGGER_B)
		{
			SetPrintPos(16, 16);                                       
			PrintChar((value == JSM_TRIGGER_PRESSED) ? 'O' : 'X');
		}
	}
}*/

//-----------------------------------------------------------------------------
/** Main loop */
void MainLoop()
{
	MutexInit();
	
	Bios_Beep();
	
	Bios_ChangeMode(SCREEN_5);
	Bios_ChangeColor(COLOR_WHITE, COLOR_DARK_GREEN, COLOR_DARK_GREEN);
	Bios_ClearScreen();
	//Bios_FillVRAM(0, 128 * 214, (COLOR_DARK_GREEN << 4) | COLOR_DARK_GREEN);

	//PrintString("-=[ GoS ]=-");
	//DrawText("ACE 0-0 SUB", 1, 1, COLOR_BLACK, COLOR_DARK_GREEN, SCREEN_5);
	//DrawText("ACE 0-0 SUB", 0, 0, COLOR_WHITE, COLOR_DARK_GREEN, SCREEN_5);
	
	Bios_TransfertRAMtoVRAM((u16)g_PlayerSprite_palette, 0x7680 + 2, 15*2);	

	MSXi_UnpackToVRAM((u16)g_PlayerSprite, 0, 32, 16, 16, 11, 8, COMPRESS_CropLine16, SCREEN_5);


	// Bios_InitPSG();
	// SetHookCallback(H_TIMI, VBlankHook);

	u8 count = 32;
	
	JSM_Initialize();
	JSM_RegisterEvent(0, JSM_INPUT_BUTTON_A, JSM_EVENT_CLICK, 0, JoystickEvent);
	JSM_RegisterEvent(0, JSM_INPUT_BUTTON_A, JSM_EVENT_RELEASE, 0, JoystickEvent);
	JSM_RegisterEvent(0, JSM_INPUT_BUTTON_B, JSM_EVENT_CLICK, 0, JoystickEvent);
	JSM_RegisterEvent(0, JSM_INPUT_BUTTON_B, JSM_EVENT_RELEASE, 0, JoystickEvent);

	//JSM_RegisterEventOld(0, JSM_EVENT_ALL, JoystickEvent);
	//JSM_RegisterEventOld(1, JSM_EVENT_ALL, JoystickEvent);

	PrintInit(COLOR_LIGHT_YELLOW);
	SetPrintPos(8, 8);
	PrintChar('X');
	SetPrintPos(16, 8);
	PrintChar('X');		
	SetPrintPos(8, 16);
	PrintChar('X');
	SetPrintPos(16, 16);                                       
	PrintChar('X');
	
	
	
	while(1)
	{
		JSM_Update();

		/*SetPrintPos(32, 8);
		PrintHex8(JSM_GetTimer(0, JSM_INPUT_BUTTON_A));
		PrintChar(' ');
		PrintText(JSM_GetLastEventText(0));
		SetPrintPos(32, 16);
		PrintHex8(JSM_GetTimer(1, JSM_INPUT_BUTTON_A));
		PrintChar(' ');
		PrintText(JSM_GetLastEventText(1));*/
	


		//DrawText("ACE", count, 200, COLOR_DARK_GREEN, COLOR_DARK_GREEN, SCREEN_5);
		count++;
		if(count > 128)
		{
			count = 32;
			//Bios_Beep();
		}
		//DrawText("ACE", count, 200, COLOR_WHITE, COLOR_DARK_GREEN, SCREEN_5);

		// switch((count++ >> 2) & 0x03)
		// {
			// case 0:
			// case 2:
				// UnpackToVRAM((u16)g_PlayerSprite, 50, 50, 16, 16, 1, 1, COMPRESS_CropLine16, SCREEN_5);
				// break;
			// case 1:
				// UnpackToVRAM((u16)g_PlayerSprite+54, 50, 50, 16, 16, 1, 1, COMPRESS_CropLine16, SCREEN_5);
				// break;
			// case 3:
				// UnpackToVRAM((u16)g_PlayerSprite+107, 50, 50, 16, 16, 1, 1, COMPRESS_CropLine16, SCREEN_5);
				// break;
		// }

		// Input
		
		// Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_DARK_RED);

		// AI
		
		// Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_DARK_GREEN);
		
		// Render - Backup
		
		// Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_CYAN);
		
		// Render - Update

		// Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_MAGENTA);

		// ???
		
		// Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_DARK_YELLOW);
		
		
		// Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_BLACK);
		
		//Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, count & 0x0F);
		VDP_WaitRetrace();
	}
}