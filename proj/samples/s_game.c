// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
//  Game module sample

//=============================================================================
// INCLUDES
//=============================================================================
#include "cmsx.h"
#include "game.h"

//=============================================================================
// DEFINES
//=============================================================================

#define FORCE		8
#define GRAVITY		1

// Prototype
bool State_Initialize();
bool State_Game();
bool State_Pause();

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Fonts
#include "font\font_cmsx_symbol1.h"
// Sprites
#include "data\data_sprt_16il.h"
// Sinus & cosinus table
#include "mathtable\mt_trigo_64.inc"

//=============================================================================
// MEMORY DATA
//=============================================================================

u8 X = 16;
u8 Y = 88;

bool bMoving = false;
bool bJumping = false;
u8 JumpForce;

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
//
bool State_Initialize()
{
	// Initialize display
	VDP_EnableDisplay(false);
	VDP_SetColor(1);
	
	// Initialize background
	VDP_FillVRAM(0xFF, g_ScreenPatternLow, 0, 256*8); // pattern
	VDP_FillVRAM(0x51, g_ScreenColorLow + 0, 0, 1); // color
	VDP_FillVRAM(0x41, g_ScreenColorLow + 1, 0, 1); // color
	for(u8 i = 0; i < 24; ++i)
		VDP_FillVRAM(i < 13 ? 8 : 0, g_ScreenLayoutLow + i * 32, 0, 32); // name

	// Initialize sprite
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);
	u8 chrSprt = 0;
	for(u8 j = 0; j < 6; j++) // Pattern 0-95
	{
		for(u8 i = 0; i < 6; i++)
		{
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 +  0 + 24 * j) * 8, chrSprt++, 1);
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 + 12 + 24 * j) * 8, chrSprt++, 1);
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 +  1 + 24 * j) * 8, chrSprt++, 1);
			VDP_LoadSpritePattern(g_DataSprt16il + (i * 2 + 13 + 24 * j) * 8, chrSprt++, 1);	
		}
	}
	VDP_SetSpriteSM1(0, 16, 16,  0, 0x01);
	VDP_SetSpriteSM1(1, 16, 16, 24, 0x0F);
	VDP_SetSpriteSM1(2, 16, 16, 48, 0x09);
	VDP_SetSpriteSM1(3, 0, 208, 0, 0); // hide

	VDP_EnableDisplay(true);

	Game_SetState(State_Game);
	return false; // Frame finished
}

//-----------------------------------------------------------------------------
//
bool State_Game()
{
	u8 animId = bJumping ? 16 : bMoving ? ((g_GameFrame >> 2) % 6) * 4 : 16;
	bool bOdd = (g_GameFrame & 1) == 1;

	VDP_SetSpriteSM1(0, X, Y, 72 * bOdd + animId, 0x01);
	VDP_SetSpriteSM1(1, X, Y, 24 + animId, 0x0F);
	VDP_SetSpriteSM1(2, X, Y, 48 + animId, 0x09);
	
	if(Keyboard_IsKeyPressed(KEY_ESC))
		Game_Exit();

	u8 row = Keyboard_Read(KEY_ROW(KEY_SPACE));
	if(IS_KEY_PRESSED(row, KEY_RIGHT))
	{
		X++;
		bMoving = true;
	}
	else if(IS_KEY_PRESSED(row, KEY_LEFT))
	{
		X--;
		bMoving = true;
	}
	else
		bMoving = false;
	if(bJumping)
	{
		Y -= JumpForce;
		JumpForce -= GRAVITY;
		if(Y > 88)
		{
			Y = 88;
			bJumping = false;
		}
	}
	else if(IS_KEY_PRESSED(row, KEY_SPACE))
	{
		bJumping = true;
		JumpForce = FORCE;
	}


		
	return true; // Frame finished
}

//-----------------------------------------------------------------------------
//
bool State_Pause()
{
	return true; // Frame finished
}

//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
// Programme entry point
void main()
{
	Game_SetState(State_Initialize);
	Game_MainLoop(VDP_MODE_GRAPHIC1);
}