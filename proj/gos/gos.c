//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------

#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "bios_main.h"
#include "video.h"
#include "font.h"
#include "mutex.h"
#include "msxi/msxi_unpack.h"

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


void VBlankHook() __naked
{	
	__asm
		di
		// push	af
		// push	bc
		// push	de
		// push	hl
		// push	ix
		// push	iy
	__endasm;
		
	// // DisableInterrupt();
	
	// if(timer < 255)
		// timer++;
	// else
		// timer = 0;

	// if(MutexGate(0))
	// {
		// MutexLock(0);
		// Bios_Beep();
		// Bios_ChangeColor(COLOR_GRAY, COLOR_DARK_GREEN, timer & 0x0F);
		// Bios_GraphicPrint(timer & 0xEF);
		// MutexRelease(0);
	// }

	// vdp_status = g_PortVDPStat;
	// // EnableInterrupt();
	__asm
		// pop		iy
		// pop		ix
		// pop		hl
		// pop		de
		// pop		bc
		// pop		af
		ei
		reti
	__endasm;
}

void PrintString(c8* str)
{
	i8 i = 0;
	while(str[i] != 0)
		Bios_GraphicPrint(str[i++]);
}

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
	DrawText("ACE 0-0 SUB", 1, 1, COLOR_BLACK, COLOR_DARK_GREEN, SCREEN_5);
	DrawText("ACE 0-0 SUB", 0, 0, COLOR_WHITE, COLOR_DARK_GREEN, SCREEN_5);
	
	Bios_TransfertRAMtoVRAM((u16)g_PlayerSprite_palette, 0x7680 + 2, 15*2);	

	MSXi_UnpackToVRAM((u16)g_PlayerSprite, 0, 32, 16, 16, 11, 8, COMPRESS_CropLine16, SCREEN_5);

	// Bios_InitPSG();
	// SetHookCallback(H_TIMI, VBlankHook);

	u8 count = 32;

	while(1)
	{
		Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, count & 0x0F);

		DrawText("ACE", count, 200, COLOR_DARK_GREEN, COLOR_DARK_GREEN, SCREEN_5);
		count++;
		if(count > 128)
		{
			count = 32;
			//Bios_Beep();
		}
		DrawText("ACE", count, 200, COLOR_WHITE, COLOR_DARK_GREEN, SCREEN_5);

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
		VDP_WaitRetrace();
	}
}