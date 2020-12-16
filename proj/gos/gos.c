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
#include "math.h"


//=============================================================================
//
//   C O D E
//
//=============================================================================

void MainLoop();

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	MainLoop();
}

//-----------------------------------------------------------------------------
// DEFINES
#define VERSION "V0.1.0"

#define PLAYER_NB 	14

#define LINE_NB		192	// 192 or 212
#define FIELD_SIZE	384 // 256 or 384
#define TEAM_BMP_X	0
#define TEAM_BMP_Y	384
#define BACK_X		0
#define BACK_Y		960

enum ACTION
{
	ACTION_IDLE = 0,
	ACTION_RUN,
};

//-----------------------------------------------------------------------------
// TYPES

typedef struct
{
	i16		x, y;	
} Vector16;

typedef struct
{
	u8	x, y;	
} VectorU8;

typedef struct
{
	Vector16	pos;
	u8			dir;
	u8			action; // 0=idle, 1=run
} Player;

//-----------------------------------------------------------------------------
// DATA
#include "data/player.data.h"

u8 g_DisplayPage = 0;
u8 g_WritePage = 1;
u8 g_VBlank = 0;
u8 g_Frame = 0;
Player g_Player[2][PLAYER_NB];

static const u8 DirToIdx[16] = 
{
	0xFF,	// 0 : No Direction
	0,		// 1 : Up
	4,		// 2 : Down
	0xFF,	// 3
	6,		// 4 : Left
	7,		// 5 : Up + Left
	5,		// 6 : Down + Left
	0xFF,	// 7
	2,		// 8 : Right
	1,		// 9 : Up + Right
	3,		// A : Down + Right
	0xFF,	// B
	0xFF,	// C
	0xFF,	// D
	0xFF,	// E
	0xFF,	// F
};

static const Vector16 RunMove[8] = 
{
	{ (i16)0,  (i16)-1 },	// Up
	{ (i16)1,  (i16)-1 },	// Up + Right
	{ (i16)1,  (i16)0 },	// Right
	{ (i16)1,  (i16)1 },	// Down + Right
	{ (i16)0,  (i16)1 },	// Down
	{ (i16)-1, (i16)1 },	// Down + Left
	{ (i16)-1, (i16)0 },	// Left
	{ (i16)-1, (i16)-1 },	// Up + Left
};

static const u8 RunFrames[] = { 0, 1, 0, 2 };

static const Vector16 FormationDef[] =
{
	{ 128, 368 }, // G
	{  48, 320 }, // DL
	{ 128, 320 }, // DC
	{ 218, 320 }, // DR
	{  80, 272 }, // ML
	{ 176, 272 }, // MR
	{ 128, 192 }, // AT
};

static const Vector16 FormationAtt[] =
{
	{ 128, 184/*336*/ }, // G
	{  80, 144 }, // DL
	{ 128, 224 }, // DC
	{ 176, 144 }, // DR
	{  64,  64 }, // ML
	{ 192,  64 }, // MR
	{ 128,  64 }, // AT
};

//-----------------------------------------------------------------------------
// FUNCTIONS

#define V8(a) (*((u8*)(&a)))
#define V16(a) (*((u16*)(&a)))

/// H-Blank interrupt hook
void HBlankHook()
{
	VDP_SetPage((V8(g_DisplayPage) * 2) + 1);
}

/// H_KEYI interrupt hook
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

/// H_TIMI interrupt hook
void VBlankHook() //__preserves_regs(a)
{
	__asm__("push	af");
	VDP_SetPage(V8(g_DisplayPage) * 2);
	g_VBlank = 1;
	__asm__("pop	af");
}

/// Wait for V-Blank period
void WaitVBlank()
{
	while(g_VBlank == 0) {}
	g_WritePage = 1 - g_WritePage;
	g_DisplayPage = 1 - g_DisplayPage;
	VDP_SetPage(V8(g_DisplayPage) * 2);
	g_Frame++;
}

/// Backup sprite background
void SpriteBackup(u8 id) __FASTCALL
{
	Player* p = &g_Player[g_WritePage][id];
	VDP_HMMM(p->pos.x, p->pos.y + (512 * g_WritePage), BACK_X + (id * 16), BACK_Y + (16 * g_WritePage), 16, 16);
}

/// Restore sprite background
void SpriteRestore(u8 id) __FASTCALL
{
	Player* p = &g_Player[g_WritePage][id];
	VDP_HMMM(BACK_X + (id * 16), BACK_Y + (16 * g_WritePage), p->pos.x, p->pos.y + (512 * g_WritePage), 16, 16);
}

/// Draw game field
void DrawField(u16 y) __FASTCALL
{
	// Field
	for(i8 j = 0; j < 24; j++)
	{
		u8 col = (j & 1) ? 0xDD : 0xEE;
		VDP_HMMV(0, (j * 16) + y, 256, 16, col);
	}
	// Lines
	VDP_HMMV(8,   8 + y, 240, 2, 0xFF); // Top
	VDP_HMMV(8, (FIELD_SIZE/2 - 1) + y, 240, 2, 0xFF); // Mid
	VDP_HMMV(8, FIELD_SIZE-8 + y, 240, 2, 0xFF); // Bot
	VDP_HMMV(8,   8 + y, 2, FIELD_SIZE-16, 0xFF); // Left
	VDP_HMMV(246, 8 + y, 2, FIELD_SIZE-16, 0xFF); // Right
	// Goal area
	VDP_HMMV(64,  8 + y, 2,  48, 0xFF);
	VDP_HMMV(190, 8 + y, 2,  48, 0xFF);
	VDP_HMMV(64, 56 + y, 128, 2, 0xFF);
}

//=============================================================================
//
//   C O D E
//
//=============================================================================

//-----------------------------------------------------------------------------
/// Main loop
void MainLoop()
{
	Bios_SetHookCallback(H_KEYI, InterruptHook);
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	VDP_SetScreen(VDP_MODE_SCREEN5);
	VDP_SetFrequency(VDP_FREQ_50HZ);
	VDP_SetLineCount(LINE_NB == 192 ? VDP_LINE_192 : VDP_LINE_212);
	VDP_SetColor(0x00);
	VDP_EnableSprite(false);
	VDP_SetPage(0);
	
	// Field
	DrawField(0);					// Page 0-1
	DrawField(512);					// Page 2-3
	// Buffer 	
	VDP_HMMV(0, 384, 256, 128, 0);	// Page 1
	VDP_HMMV(0, 896, 256, 128, 0);	// Page 3
	
	// Import player sprites
	MSXi_UnpackToVRAM(g_PlayerSprite, TEAM_BMP_X, TEAM_BMP_Y, 16, 16, 11, 8, COMPRESS_CropLine16, 0);
	VDP_SetPalette(g_PlayerSprite_palette);
	VDP_SetPaletteEntry(3,  RGB16(0, 0, 7)); // Arm
	VDP_SetPaletteEntry(11, RGB16(0, 0, 7)); // Chest
	VDP_SetPaletteEntry(6,  RGB16(7, 7, 7)); // Short
	VDP_SetPaletteEntry(5,  RGB16(7, 7, 7)); // Socks
	VDP_SetPaletteEntry(13, RGB16(0, 6, 0)); 
	VDP_SetPaletteEntry(14, RGB16(0, 4, 0)); 
	VDP_SetPaletteEntry(15, RGB16(7, 7, 7)); 

	// VDP_SetPage(2);
	//while(!Keyboard_IsKeyPressed(KEY_SPACE)) {}
	
	//-------------------------------------------------------------------------
	// Initialize players
	for(i8 j = 0; j < 2; j++)
	{
		Player* p = &g_Player[j][0];
		for(i8 i = 0; i < PLAYER_NB; i++)
		{
			if(i < PLAYER_NB / 2)
			{
				p->pos = FormationAtt[i % 7];
				p->dir = 0;
			}
			else
			{
				p->pos.x = 256 - FormationDef[i % 7].x;
				p->pos.y = 384 - FormationDef[i % 7].y;
				p->dir = 4;
			}
			p->action = ACTION_IDLE;
			p++;
			g_WritePage = j;
			SpriteBackup(i);
		}
	}
	
	u8  scrollSpeed = 2;
	i16 scrollOffset = 0;
	i16 scrollPrevious = 0;
	VDP_SetHBlankLine(253);			

	g_DisplayPage = 0;
	g_WritePage = 1;

	Player* p;			
	u8 dir;
	while(1)
	{
		// VDP_SetColor(0x22); //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUG
		//---------------------------------------------------------------------
		// SCROLLING
		//---------------------------------------------------------------------
		// follow the player
		scrollOffset = g_Player[g_WritePage/*g_DisplayPage*/][0].pos.y - (LINE_NB / 2);
		// Clamp scrolling value
		if(scrollOffset < 0)
			scrollOffset = 0;
		else if(scrollOffset > FIELD_SIZE - LINE_NB)
			scrollOffset = FIELD_SIZE - LINE_NB;
		// Handle scroll events
		if((scrollPrevious < 256 - LINE_NB) && (scrollOffset >= 256 - LINE_NB))
			VDP_EnableHBlank(true);
		else if((scrollPrevious >= 256 - LINE_NB) && (scrollOffset < 256 - LINE_NB))
			VDP_EnableHBlank(false);
		// Set the screen scrolling offset
		VDP_SetVerticalOffset(scrollOffset & 0x00FF);
		// Backup previous scrolling offset
		scrollPrevious = scrollOffset;

		// VDP_SetColor(0x33); //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUG
		//---------------------------------------------------------------------
		// RESTORE BACKGROUND
		//---------------------------------------------------------------------
		for(i8 i = 0; i < PLAYER_NB; i++)
		{
			SpriteRestore(i);
			g_Player[g_WritePage][i] = g_Player[g_DisplayPage][i];
		}

		// VDP_SetColor(0x44); //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUG
		//---------------------------------------------------------------------
		// UPDATE PLAYERS & BACKUP BACKGROUND
		//---------------------------------------------------------------------
		p = &g_Player[g_WritePage][0];			
		for(i8 i = 0; i < PLAYER_NB; i++)
		{
			if(i == 0) // controller 1
			{
				u8 row = Keyboard_Read(KEY_ROW(KEY_DOWN));
				dir = 0;
				if((row & KEY_FLAG(KEY_UP)) == 0)
					dir |= JOY_INPUT_DIR_UP;
				else if((row & KEY_FLAG(KEY_DOWN)) == 0)
					dir |= JOY_INPUT_DIR_DOWN;
				if((row & KEY_FLAG(KEY_RIGHT)) == 0)
					dir |= JOY_INPUT_DIR_RIGHT;
				else if((row & KEY_FLAG(KEY_LEFT)) == 0)
					dir |= JOY_INPUT_DIR_LEFT;

				if(dir != 0)
				{
					p->dir = DirToIdx[dir];
					p->action = ACTION_RUN;
					p->pos.x += RunMove[p->dir].x;
					p->pos.y += RunMove[p->dir].y;
				}
				else
					p->action = ACTION_IDLE;	
			}
			/*else // AI
			{
				Player* t = &g_Player[g_WritePage][i-1];
								
				dir = 0;
				if(p->pos.x < t->pos.x)
					dir += JOY_INPUT_DIR_RIGHT;
				else if(p->pos.x > t->pos.x)
					dir += JOY_INPUT_DIR_LEFT;
				if(p->pos.y > t->pos.y)
					dir += JOY_INPUT_DIR_UP;
				else if(p->pos.y < t->pos.y)
					dir += JOY_INPUT_DIR_DOWN;

				u16 dx = Abs16(p->pos.x - t->pos.x);
				u16 dy = Abs16(p->pos.y - t->pos.y);
				u16 sqrDist = dx*dx + dy*dy;
				if (sqrDist > 16*16)
				{
					p->dir = DirToIdx[dir];
					p->action = ACTION_RUN;
					p->pos.x += RunMove[p->dir].x;
					p->pos.y += RunMove[p->dir].y;
				}
				else
					p->action = ACTION_IDLE;			
			}*/
			if(p->pos.x < 0)
				p->pos.x = 0;
			else if(p->pos.x > 256-16)
				p->pos.x = 256-16;
			if(p->pos.y < 0)
				p->pos.y = 0;
			else if(p->pos.y > FIELD_SIZE-16)
				p->pos.y = FIELD_SIZE-16;
		
			//if((p->pos.y + 16 >= scrollOffset) && (p->pos.y <= scrollOffset + LINE_NB))
			{
				SpriteBackup(i);
			}
			p++;
		}
			
		// VDP_SetColor(0x55); //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUG
		//---------------------------------------------------------------------
		// DRAW PLAYERS
		//---------------------------------------------------------------------
		p = &g_Player[g_WritePage][0];			
		for(i8 i = 0; i < PLAYER_NB; i++)
		{
			if((p->pos.y + 16 >= scrollOffset) && (p->pos.y <= scrollOffset + LINE_NB))
			{
				u8 sx;
				if(p->action == ACTION_IDLE)
					sx = 0;
				else //if(p->action == ACTION_RUN)
					sx = (16 * RunFrames[(g_Frame >> 2) & 0x03]);
					
				VDP_LMMM(TEAM_BMP_X + sx, TEAM_BMP_Y + (16 * p->dir), p->pos.x, p->pos.y + (512 * g_WritePage), 16, 16, VDP_OP_TIMP); // Draw
			}
			p++;
		}	

		// VDP_SetColor(0x00); //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEBUG
		g_VBlank = 0;
		WaitVBlank();
	}
}