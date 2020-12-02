//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------

#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "print.h"
#include "mutex.h"
#include "input.h"
#include "memory.h"
#include "vdp.h"
#include "bios_main.h"
#include "msxi/msxi_unpack.h"


//-----------------------------------------------------------------------------
// D E F I N E S
#define VERSION "V0.1.0"

//-----------------------------------------------------------------------------
// R A M   D A T A

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


//-----------------------------------------------------------------------------
/** Main loop */
void MainLoop()
{
	MutexInit();
	
	Bios_Beep();
	
	VDP_SetModeText1();
	//Bios_ChangeMode(SCREEN_5);
	//Bios_ChangeColor(COLOR_WHITE, COLOR_DARK_GREEN, COLOR_DARK_GREEN);
	Bios_ClearScreen();

	Bios_TransfertRAMtoVRAM(g_CGTABL, 0x0000, 256*8);
	//Bios_FillVRAM(0x0800, 40*24, 'X');
	
	//Bios_TransfertRAMtoVRAM((u16)g_PlayerSprite_palette, 0x7680 + 2, 15*2);	

	//MSXi_UnpackToVRAM((u16)g_PlayerSprite, 0, 32, 16, 16, 11, 8, COMPRESS_CropLine16, SCREEN_5);
	
	//IPM_Initialize(null);
	//IPM_RegisterEvent(0, IPM_INPUT_BUTTON_A, IPM_EVENT_CLICK, 0, JoystickEvent);

	//PrintInit(COLOR_LIGHT_YELLOW);
		
	u8 count = 0;
	while(1)
	{
		//IPM_Update();

		count++;
		SetPrintPos(8, 8);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		//PrintChar(chrAnim[count & 0x03]);
		//Bios_TextPrintChar(chrAnim[count & 0x03]);
		Bios_FillVRAM(0x0800, 40*24, chrAnim[count & 0x03]);
	}
}