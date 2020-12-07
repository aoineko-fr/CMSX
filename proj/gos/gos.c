//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------

#include "core.h"
#include "color.h"
#include "input.h"
#include "memory.h"
#include "vdp.h"
#include "msxi/msxi_unpack.h"
#include "bios_hook.h"
#include "ports.h"


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

void HBlankHook()
{
	VDP_SetPage(1);
	//VDP_SetColor(0x66);
}

void InterruptHook()
{
	__asm
		// Get S#1
		ld		a, #1
		out		(#P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(#P_VDP_ADDR), a
		in		a, (#P_VDP_STAT)
		//  Call H-Blank if bit #0 of S#1 is set 
		rrca
		jp		nc, _no_hblank
		call	_HBlankHook // call to C function HBlankHook() 
		// Reset R#15 to S#0
	_no_hblank:
		xor		a           		
		out		(#P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(#P_VDP_ADDR),a
	__endasm;
}

u8 g_VBlank = 0;
u8 g_Frame = 0;

void VBlankHook() __preserves_regs(a)
{
	g_VBlank = 1;
}

void WaitVBlank()
{
	while(g_VBlank == 0) {}
	g_VBlank = 0;
	g_Frame++;
}

u8 g_ScrollOffset = 0;

//-----------------------------------------------------------------------------
/** Main loop */
void MainLoop()
{
	Bios_SetHookCallback(H_KEYI, InterruptHook);
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	VDP_SetScreen(VDP_MODE_SCREEN5);
	VDP_SetColor(0xCC);
	
	VDP_FillVRAM(0x22, 0x0000, 0, 0x8000);

	VDP_HMMV(0, 0, 256, 1, 0xFF);
	VDP_HMMV(0, 255, 256, 1, 0xFF);

	VDP_FillVRAM(0x44, 0x8000, 0, 0x8000);
	VDP_HMMV(0, 256, 256, 1, 0x66);
	VDP_HMMV(0, 511, 256, 1, 0x66);

	MSXi_UnpackToVRAM((u16)g_PlayerSprite, 0, 32, 16, 16, 11, 8, COMPRESS_CropLine16, 0);

	IPM_Initialize(null);
	//IPM_RegisterEvent(0, IPM_INPUT_BUTTON_A, IPM_EVENT_CLICK, 0, JoystickEvent);

	u8 count = 0;
	while(1)
	{
		//VDP_SetColor(0x00);
		VDP_SetPage(0);

		//VDP_SetColor(0x66);
		//IPM_Update();

		if(Keyboard_IsKeyPressed(KEY_DOWN))
		{
			if(g_ScrollOffset < 255)
			{
				g_ScrollOffset++;
				if(g_ScrollOffset + 212 > 256)
				{
					VDP_EnableHBlank(true);
					VDP_SetHBlankLine(253);
				}
			}
		}
		else if(Keyboard_IsKeyPressed(KEY_UP))
		{
			if(g_ScrollOffset > 0)
			{
				g_ScrollOffset--;
				if(g_ScrollOffset + 212 <= 256)
				{
					VDP_EnableHBlank(true);
					VDP_SetHBlankLine(253);
				}	
			}
		}
		VDP_SetVerticalOffset(g_ScrollOffset);
		
		count++;

		// VDP_YMMM(32 + (16 * ((count >> 3) & 0x07)), 0, 212-16, 16, VDP_ARG_DIY_DOWN + VDP_ARG_DIX_RIGHT);
		
		// static const u8 RunFrames[] = { 0, 1, 0, 2 };
		
		// VDP_HMMM((16 * RunFrames[(count >> 3) & 0x03]), 64, 64 + (count & 0x7F), 0, 16, 16);
		// VDP_LMMM((16 * RunFrames[(count >> 3) & 0x03]), 64, 64 + (count & 0x7F), 16, 16, 16, VDP_OP_TIMP);

		//VDP_FillVRAM(((count & 0x0F) << 4) | (count & 0x0F), 0x0000, 0, 128*1);
		//VDP_HMMV(0, 0, 32, 32, ((count & 0x0F) << 4) | (count & 0x0F));
		
		// VDP_SetGrayScale(true);

		//VDP_SetColor(0xCC);
		// VDP_EnableDisplay(true);

		//VDP_WaitVBlank();
		WaitVBlank();

		// VDP_EnableDisplay(false);
	}
}