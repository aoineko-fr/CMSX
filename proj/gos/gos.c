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
/// Program entry point
void main()
{
	MainLoop();
}

//-----------------------------------------------------------------------------
// DEFINES
#define VERSION "V0.2.0"

#define PLAYER_NB 		14

#define LINE_NB			192	// 192 or 212
#define FIELD_SIZE		384 // 256 or 384
#define TEAM_BMP_X		128
#define TEAM_BMP_Y		384
#define BACK_X			0
#define BACK_Y			960
#define SPRT_OFS_X		(-7)
#define SPRT_OFS_Y		(-15)
#define MIN_DIFF		8

/// Player action ID
enum ACTION_ID
{
	ACTION_IDLE = 0,
	ACTION_RUN,
	ACTION_CHARGE,
	ACTION_SHOOT,
	ACTION_HEAD,
	ACTION_TACKLE,
	ACTION_JUMP,
	ACTION_MAX = 15,
};

//-----------------------------------------------------------------------------
// TYPES

/// Vector tructure
typedef struct
{
	i16		x, y;	
} Vector16;

/// Player information structure
typedef struct
{
	Vector16	pos;			///< Position
	Vector16	target;			///< Position
	u8			dir     : 3;	///< Direction (0-7)
	u8			id      : 4;	///< Player team ID (0-15)
	u8			nearest : 1;	///< Is the nearest player from the ball
	u8			action  : 4;	///< Action ID (0-15): 0=idle, 1=run, 2=tackle @see ACTION_ID
	u8			team    : 1;	///< Team ID (0 or 1)
	u8			hasball : 1;	///< Is having ball?
	u8			moving  : 1;	///< Is moving?: 0=false, 1=true
	u8			display : 1;	///< Should be displayed: 0=false, 1=true
} Player;

//-----------------------------------------------------------------------------
// DATA
//#define D_g_PlayerSprite __at(0x0000)
#include "data/player.data.h"

/// Index of the current VDP display page (will be switch after V-Blank)
u8 g_DisplayPage = 0;
/// Index of the current VDP work page (will be switch after V-Blank)
u8 g_WritePage = 1;
/// Flag to inform game thread that a V-Blank occured
u8 g_VBlank = 0;
/// Frame counter
u8 g_Frame = 0;
/// Players information
Player g_Player[2][PLAYER_NB];

/// Table to convert direction flag to direction index (from 0 to 7)
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

/// Movement offset for each direction
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

/// Run animation frames ID
static const u8 Anim_RunFrames[] = { 0, 1, 0, 2 };

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
	{ 128, 336 }, // G
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
	VDP_HMMM(p->pos.x + SPRT_OFS_X, p->pos.y + SPRT_OFS_Y + (512 * g_WritePage), BACK_X + (id * 16), BACK_Y + (16 * g_WritePage), 16, 16);
}

/// Restore sprite background
void SpriteRestore(u8 id) __FASTCALL
{
	Player* p = &g_Player[g_WritePage][id];
	VDP_HMMM(BACK_X + (id * 16), BACK_Y + (16 * g_WritePage), p->pos.x + SPRT_OFS_X, p->pos.y + SPRT_OFS_Y + (512 * g_WritePage), 16, 16);
}

/// Compute target position.
/// @param		p		The player to compute
/// @param		level	The ball position (from 0 to 5)
void Player_ComputeTaget(Player* p, u8 level)
{
	if(p->team == 0)
	{
		switch(level)
		{
		// case 5:
		// case 4:
		default:
			p->target = FormationDef[p->id];
			break;
		case 3:
			p->target.x = (FormationDef[p->id].x * 3 + FormationAtt[p->id].x) / 4;
			p->target.y = (FormationDef[p->id].y * 3 + FormationAtt[p->id].y) / 4;
			break;
		case 2:
			p->target.x = (FormationDef[p->id].x + FormationAtt[p->id].x) / 2;
			p->target.y = (FormationDef[p->id].y + FormationAtt[p->id].y) / 2;
			break;
		case 1:
			p->target.x = (FormationDef[p->id].x + FormationAtt[p->id].x * 3) / 4;
			p->target.y = (FormationDef[p->id].y + FormationAtt[p->id].y * 3) / 4;
			break;
		case 0:
			p->target = FormationAtt[p->id];
			break;
		};
	}
	else // team == 1
	{
		switch(level)
		{
		// case 0:
		// case 1:
		default:
			p->target.x = 255 - FormationDef[p->id].x;
			p->target.y = 383 - FormationDef[p->id].y;
			break;
		case 2:
			p->target.x = 255 - (FormationDef[p->id].x * 3 + FormationAtt[p->id].x) / 4;
			p->target.y = 383 - (FormationDef[p->id].y * 3 + FormationAtt[p->id].y) / 4;
			break;
		case 3:
			p->target.x = 255 - (FormationDef[p->id].x + FormationAtt[p->id].x) / 2;
			p->target.y = 383 - (FormationDef[p->id].y + FormationAtt[p->id].y) / 2;
			break;
		case 4:
			p->target.x = 255 - (FormationDef[p->id].x + FormationAtt[p->id].x * 3) / 4;
			p->target.y = 383 - (FormationDef[p->id].y + FormationAtt[p->id].y * 3) / 4;
			break;
		case 5:
			p->target.x = 255 - FormationAtt[p->id].x;
			p->target.y = 383 - FormationAtt[p->id].y;
			break;
		};
	}
}

/// Draw game field
void DrawField(u16 y) __FASTCALL
{
	// Field
	for(u8 j = 0; j < 24; ++j)
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
	// Goal area
	VDP_HMMV(64,  384-56 + y, 2,  48, 0xFF);
	VDP_HMMV(190, 384-56 + y, 2,  48, 0xFF);
	VDP_HMMV(64,  384-56 + y, 128, 2, 0xFF);
}

///
u16 GetSqrDistance(const Vector16* from, const Vector16* to)
{
	u16 dx = Abs16(from->x - to->x);
	u16 dy = Abs16(from->y - to->y);
	return dx*dx + dy*dy;
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
	VDP_SetScreen(VDP_MODE_SCREEN5);
	VDP_SetFrequency(VDP_FREQ_50HZ);
	VDP_SetLineCount(LINE_NB == 192 ? VDP_LINE_192 : VDP_LINE_212);
	VDP_SetColor(0x00);
	VDP_EnableSprite(false);
	VDP_SetPage(0);
	VDP_EnableDisplay(false);
	
	// Field
	DrawField(0);					// Page 0-1
	DrawField(512);					// Page 2-3
	// Buffer 	
	VDP_HMMV(0, 384, 256, 128, 0);	// Page 1
	VDP_HMMV(0, 896, 256, 128, 0);	// Page 3
	
	const static u8 ReplaceColorT0[] = { 1, 6, 15 };
	const static u8 ReplaceColorT1[] = { 3, 6, 15, 11, 6, 3, 6 };
	
	// Import player sprites
	MSXi_UnpackToVRAM(g_PlayerSprite, TEAM_BMP_X*0, TEAM_BMP_Y, 16, 16, 8, 8, ReplaceColorT0, CropLine16, 4, 4);
	MSXi_UnpackToVRAM(g_PlayerSprite, TEAM_BMP_X*1, TEAM_BMP_Y, 16, 16, 8, 8, ReplaceColorT1, CropLine16, 4, 4);
	VDP_SetPalette(g_PlayerSprite_palette);
	VDP_SetPaletteEntry(3,  RGB16(0, 0, 7)); // Arm
	VDP_SetPaletteEntry(11, RGB16(0, 0, 7)); // Chest T0
	VDP_SetPaletteEntry(6,  RGB16(7, 0, 0)); // Chest T1
	VDP_SetPaletteEntry(5,  RGB16(7, 7, 7)); // Socks
	VDP_SetPaletteEntry(13, RGB16(0, 6, 0)); 
	VDP_SetPaletteEntry(14, RGB16(0, 4, 0)); 
	VDP_SetPaletteEntry(15, RGB16(7, 7, 7)); 

	// [ 1] #000000		Black
	// [ 2] #AA4400
	// [ 3] #FF0000		Player Arms
	// [ 4] #884400
	// [ 5] #FFFF00		Player Socks
	// [ 6] #00DD00		Player Short
	// [ 7] #CC8800
	// [ 8] #FFBB00
	// [ 9] #FF7711
	// [10] #772211
	// [11] #0000FF		Player Chest
	// [12] #FFFFFF		White
	// [13] #999999		Gray
	// [14] #0D0D0D
	// [15] #0E0E0E
	
	// VDP_EnableDisplay(true);
	// VDP_SetPage(1);
	// while(!Keyboard_IsKeyPressed(KEY_SPACE)) {}
	
	//-------------------------------------------------------------------------
	// Initialize players
	for(u8 j = 0; j < 2; ++j)
	{
		Player* p = &g_Player[j][0];
		for(u8 i = 0; i < PLAYER_NB; ++i)
		{			
			p->id = i % 7;
			if(i < PLAYER_NB / 2)
			{
				p->dir = 0;
				p->team = 0;
			}
			else
			{
				p->dir = 4;
				p->team = 1;
			}

			Player_ComputeTaget(p, 5);
			p->pos = p->target;
			
			p->action = ACTION_IDLE;
			p->hasball = false;
			p->moving = false;
			p->display = true;
			p->nearest = false;
			p++;
			g_WritePage = j;
			SpriteBackup(i);
		}
	}
	
	u8  scrollSpeed = 2;
	i16 scrollOffset = 0;
	i16 scrollPrevious = 0;
	VDP_SetHBlankLine(253);
	u8 ballLevel = 0;
	Vector16 ballPosition;
	u8 playerChara = 6;

	Player* p;			
	u8 dir;
	
	Bios_SetHookCallback(H_KEYI, InterruptHook);
	Bios_SetHookCallback(H_TIMI, VBlankHook);
	VDP_EnableDisplay(true);

	while(1)
	{
		ballPosition = g_Player[g_DisplayPage][playerChara].pos;
		ballLevel = ballPosition.y >> 6;
		
		//---------------------------------------------------------------------
		// SCROLLING
		//---------------------------------------------------------------------
		// follow the ball
		scrollOffset = ballPosition.y - (LINE_NB / 2);
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

		//---------------------------------------------------------------------
		// RESTORE BACKGROUND
		//---------------------------------------------------------------------
		u8 nearestPly = 0xFF;
		u16 nearestDist = 0xFFFF;
		for(u8 i = 0; i < PLAYER_NB; ++i)
		{
			SpriteRestore(i);
			g_Player[g_WritePage][i] = g_Player[g_DisplayPage][i]; // swap player data
			
			if(p->team == 1)
			{
				p = &g_Player[g_WritePage][i];
				u16 sqrDist = GetSqrDistance(&(p->pos), &ballPosition);
				if((sqrDist < nearestDist) || (nearestDist == 0xFFFF))
				{
					nearestDist = sqrDist;
					nearestPly = i;				
				}
			}
			p->nearest = false;
		}
		if(nearestPly != 0xFF)
			g_Player[g_WritePage][nearestPly].nearest = true;
		
		//---------------------------------------------------------------------
		// UPDATE PLAYERS & BACKUP BACKGROUND
		//---------------------------------------------------------------------
		p = &g_Player[g_WritePage][0];			
		for(u8 i = 0; i < PLAYER_NB; ++i)
		{
			if(i == playerChara) // controller 1
			{
				// Player change
				u8 row = Keyboard_Read(KEY_ROW(KEY_GRAPH));
				if((row & KEY_FLAG(KEY_GRAPH)) == 0)
					playerChara = (playerChara + 1) % (PLAYER_NB / 2);

				// Move & tackle
				row = Keyboard_Read(KEY_ROW(KEY_DOWN));
				dir = 0;
				if((row & KEY_FLAG(KEY_SPACE)) == 0)
					dir = p->dir;
				else if((row & KEY_FLAG(KEY_UP)) == 0)
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
			else // AI
			{
				Vector16* t = &ballPosition;
				if(p->nearest == 0)
				{
					Player_ComputeTaget(p, ballLevel);
					t = &p->target;
				}
				
				u16 sqrDist = GetSqrDistance(&(p->pos), t);
				if (sqrDist > 16*16) // Move
				{
					p->action = ACTION_RUN;
					// Turn toward movement
					dir = 0;
					if(p->pos.x < t->x)
						dir += JOY_INPUT_DIR_RIGHT;
					else if(p->pos.x > t->x)
						dir += JOY_INPUT_DIR_LEFT;
					if(p->pos.y > t->y)
						dir += JOY_INPUT_DIR_UP;
					else if(p->pos.y < t->y)
						dir += JOY_INPUT_DIR_DOWN;
					p->dir = DirToIdx[dir];
					// Move
					p->pos.x += RunMove[p->dir].x;
					p->pos.y += RunMove[p->dir].y;
				}
				else // Idle
				{
					Vector16* b = &ballPosition;
					p->action = ACTION_IDLE;			
					// Turn toward ball
					dir = 0;
					i16 dx = p->pos.x - b->x;
					if(dx < -MIN_DIFF)
						dir += JOY_INPUT_DIR_RIGHT;
					else if(dx > MIN_DIFF)
						dir += JOY_INPUT_DIR_LEFT;
					i16 dy = p->pos.y - b->y;
					if(dy > MIN_DIFF)
						dir += JOY_INPUT_DIR_UP;
					else if(dy < -MIN_DIFF)
						dir += JOY_INPUT_DIR_DOWN;
					p->dir = DirToIdx[dir];
				}
			}
			if(p->pos.x < 0)
				p->pos.x = 0;
			else if(p->pos.x > 255)
				p->pos.x = 255;
			if(p->pos.y < 0)
				p->pos.y = 0;
			else if(p->pos.y > FIELD_SIZE-1)
				p->pos.y = FIELD_SIZE-1;
		
			//if((p->pos.y + 16 >= scrollOffset) && (p->pos.y <= scrollOffset + LINE_NB))
			{
				SpriteBackup(i);
			}
			p++;
		}

		//---------------------------------------------------------------------
		// DRAW PLAYERS
		//---------------------------------------------------------------------
		p = &g_Player[g_WritePage][0];			
		for(u8 i = 0; i < PLAYER_NB; ++i)
		{
			if((p->pos.y + SPRT_OFS_Y + 16 >= scrollOffset) && (p->pos.y + SPRT_OFS_Y <= scrollOffset + LINE_NB))
			{
				u8 sx;
				if(p->action == ACTION_IDLE)
					sx = 0;
				else //if(p->action == ACTION_RUN)
					sx = (16 * Anim_RunFrames[(g_Frame >> 2) & 0x03]);
					
				VDP_LMMM(TEAM_BMP_X * p->team + sx, TEAM_BMP_Y + (16 * p->dir), p->pos.x + SPRT_OFS_X, p->pos.y + SPRT_OFS_Y + (512 * g_WritePage), 16, 16, VDP_OP_TIMP); // Draw
			}
			p++;
		}	

		//---------------------------------------------------------------------
		g_VBlank = 0;
		WaitVBlank();
	}
}