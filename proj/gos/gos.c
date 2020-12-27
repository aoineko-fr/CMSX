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

#define LINE_NB			212	// 192 or 212
#define FIELD_SIZE		384 // 256 or 384
#define TEAM_BMP_X		128
#define TEAM_BMP_Y		384
#define BACK_X			0
#define BACK_Y			(1024-32)
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

/// 8-bits vector structure
typedef struct
{
	u8			x, y;	
} Vector8;

/// 8/16-bits vector structure
typedef struct
{
	u8			x;	
	u16			y;	
} Vector816;

/// Vector tructure
typedef struct
{
	u16			x, y;	
} Vector16;

typedef struct
{
	Vector816	sprtPos;		///< 
	Vector8		sprtSize;		///<
	
} PlyRender;

/// Player information structure
typedef struct
{
	Vector816	pos;			///< Position
	Vector816	target;			///< Position
	u8			dir     : 3;	///< Direction (0-7)
	u8			id      : 4;	///< Player team ID (0-15)
	u8			nearest : 1;	///< Is the nearest player from the ball
	
	u8			action  : 4;	///< Action ID (0-15): 0=idle, 1=run, 2=tackle @see ACTION_ID
	u8			team    : 1;	///< Team ID (0 or 1)
	u8			hasball : 1;	///< Is having ball?
	u8			moving  : 1;	///< Is moving?: 0=false, 1=true
	u8			display : 1;	///< Should be displayed: 0=false, 1=true
	
	u8			visible : 1;	///< Is player visible
	u8			drawn   : 1;	///< Has player been drawn
	u8			sprtIdX : 3;	///< 
	u8			sprtIdY : 3;	///< 

	u8			minX    : 4;	///< 
	u8			minY    : 4;	///< 
	Vector816	sprtPos;		///< 
	Vector8		sprtSize;		///<
	PlyRender	render[2];
	
} PlyInfo;

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
PlyInfo g_Player[2][PLAYER_NB];

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

/// Movement offset for each direction (x-1)
static const Vector8 RunMove[8] = 
{
	{ 1, 0 },	// Up
	{ 2, 0 },	// Up + Right
	{ 2, 1 },	// Right
	{ 2, 2 },	// Down + Right
	{ 1, 2 },	// Down
	{ 0, 2 },	// Down + Left
	{ 0, 1 },	// Left
	{ 0, 0 },	// Up + Left
};

/// Run animation frames ID
static const u8 Anim_RunFrames[] = { 0, 1, 0, 2 };

static const Vector816 FormationDef[] =
{
	{ 128, 368 }, // G
	{  48, 320 }, // DL
	{ 128, 320 }, // DC
	{ 218, 320 }, // DR
	{  80, 272 }, // ML
	{ 176, 272 }, // MR
	{ 128, 192 }, // AT
};

static const Vector816 FormationAtt[] =
{
	{ 128, 336 }, // G
	{  80, 144 }, // DL
	{ 128, 224 }, // DC
	{ 176, 144 }, // DR
	{  64,  64 }, // ML
	{ 192,  64 }, // MR
	{ 128,  64 }, // AT
};

// u8 HookBackup_KEYI[5];
// u8 HookBackup_TIMI[5];

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
	_no_hblank:
		// Reset R#15 to S#0
		xor		a           		
		out		(#P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(#P_VDP_ADDR),a
	__endasm;

	// Call((u16)HookBackup_KEYI);
}

/// H_TIMI interrupt hook
void VBlankHook() //__preserves_regs(a)
{
	__asm__("push	af");
	VDP_SetPage(V8(g_DisplayPage) * 2);
	g_VBlank = 1;
	__asm__("pop	af");
	
	// Call((u16)HookBackup_KEYI);
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

/// Compute target position.
/// @param		p		The player to compute
/// @param		area	The ball position (from 0 to 5)
void Player_ComputeTaget(PlyInfo* p, u8 area)
{
	if(p->team == 0)
	{
		/*switch(area)
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
		};*/
		switch(area)
		{
		case 5:
		case 4:
		case 3:
			p->target = FormationDef[p->id];
			break;
		case 2:
			p->target.x = (FormationDef[p->id].x + FormationAtt[p->id].x) / 2;
			p->target.y = (FormationDef[p->id].y + FormationAtt[p->id].y) / 2;
			break;
		case 1:
		case 0:
			p->target = FormationAtt[p->id];
			break;
		};
	}
	else // team == 1
	{
		switch(area)
		{
		case 0:
		case 1:
		case 2:
			p->target.x = 255 - FormationDef[p->id].x;
			p->target.y = 383 - FormationDef[p->id].y;
			break;
		case 3:
			p->target.x = 255 - (FormationDef[p->id].x + FormationAtt[p->id].x) / 2;
			p->target.y = 383 - (FormationDef[p->id].y + FormationAtt[p->id].y) / 2;
			break;
		case 4:
		case 5:
			p->target.x = 255 - FormationAtt[p->id].x;
			p->target.y = 383 - FormationAtt[p->id].y;
			break;
		};

		/*switch(area)
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
		};*/
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
u16 GetSqrDistance(const Vector816* from, const Vector816* to)
{
	u8 dx = Abs8(from->x - to->x);
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
	VDP_EnableSprite(true);
	VDP_SetSpriteTables(VRAM16b(0x1C000), VRAM16b(0x1C600));
	VDP_SetPage(0);
	VDP_EnableDisplay(false);
	
	// Field
	DrawField(0);					// Page 0-1
	DrawField(512);					// Page 2-3
	// Buffer 	
	VDP_HMMV(0, 384, 256, 128, 0);	// Page 1
	VDP_HMMV(0, 896, 256, 128, 0);	// Page 3
	VDP_WaitReady();
	const static u8 ReplaceColorT0[] = { 1, 6, 15 };
	const static u8 ReplaceColorT1[] = { 3, 6, 15, 11, 6, 3, 6 };
	
	// Import player sprites
	MSXi_UnpackToVRAM(g_PlayerSprite, TEAM_BMP_X*0, TEAM_BMP_Y, 16, 16, 8, 8, ReplaceColorT0, Crop16, 4, 4);
	MSXi_UnpackToVRAM(g_PlayerSprite, TEAM_BMP_X*1, TEAM_BMP_Y, 16, 16, 8, 8, ReplaceColorT1, Crop16, 4, 4);
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
		PlyInfo* p = &g_Player[j][0];
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
			p->drawn = 0;
		}
	}
	
	u8 scrollOffset = 0;
	u8 scrollPrevious = 0;
	VDP_SetHBlankLine(253);
	u8 ballArea = 0;
	Vector816 ballPosition;
	u8 playerChara = 6;

	PlyInfo* p;			
	u8 dir;
	
	//Mem_Copy((void*)H_KEYI, (void*)HookBackup_KEYI, 5);
	Bios_SetHookCallback(H_KEYI, InterruptHook);
	//Mem_Copy((void*)H_TIMI, (void*)HookBackup_TIMI, 5);
	Bios_SetHookCallback(H_TIMI, VBlankHook);
	VDP_EnableDisplay(true);

	while(1)
	{
		ballPosition = g_Player[g_DisplayPage][playerChara].pos;
		ballArea = ballPosition.y >> 6;
		
		//---------------------------------------------------------------------
		// SCROLLING
		//---------------------------------------------------------------------
		
		// Follow the ball Y position
		if(ballPosition.y < (LINE_NB / 2))
			scrollOffset = 0;
		else if(ballPosition.y > FIELD_SIZE - (LINE_NB / 2))
			scrollOffset = (u8)(FIELD_SIZE - LINE_NB);
		else
			scrollOffset = (u8)ballPosition.y - (LINE_NB / 2);
		// Handle scroll events
		if((scrollPrevious < 256 - LINE_NB) && (scrollOffset >= 256 - LINE_NB))
			VDP_EnableHBlank(true);
		else if((scrollPrevious >= 256 - LINE_NB) && (scrollOffset < 256 - LINE_NB))
			VDP_EnableHBlank(false);
		// Set the screen scrolling offset
		VDP_SetVerticalOffset(scrollOffset);
		// Backup previous scrolling offset
		scrollPrevious = scrollOffset;

		//---------------------------------------------------------------------
		// RESTORE BACKGROUND
		//---------------------------------------------------------------------
		u8 nearestPly = 0xFF;
		u16 nearestDist = 0xFFFF;
		p = &g_Player[g_WritePage][0];
		u8 i;		
		for(i = 0; i < PLAYER_NB; ++i)
		{
			// Do restore
			if(p->drawn)
			{
				VDP_HMMM(
					BACK_X + (i * 16), 
					BACK_Y + (16 * g_WritePage), 
					p->sprtPos.x,
					p->sprtPos.y,
					p->sprtSize.x + 2, 
					p->sprtSize.y);
				p->drawn = 0;
			}

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
			p++;
		}
		if(nearestPly != 0xFF)
			g_Player[g_WritePage][nearestPly].nearest = true;
		
		//---------------------------------------------------------------------
		// UPDATE PLAYERS & BACKUP BACKGROUND
		//---------------------------------------------------------------------
		p = &g_Player[g_WritePage][0];			
		for(i = 0; i < PLAYER_NB; ++i)
		{
			if(i == playerChara) // controller 1
			{
				// Move & tackle
				u8 row = Keyboard_Read(KEY_ROW(KEY_DOWN));
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
					p->pos.x += RunMove[p->dir].x - 1;
					p->pos.y += RunMove[p->dir].y - 1;
				}
				else
					p->action = ACTION_IDLE;	
			}
			else // AI
			{
				Vector816* t = &ballPosition;
				if(p->nearest == 0)
				{
					Player_ComputeTaget(p, ballArea);
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
					p->pos.x += RunMove[p->dir].x - 1;
					p->pos.y += RunMove[p->dir].y - 1;
				}
				else // Idle
				{
					Vector816* b = &ballPosition;
					p->action = ACTION_IDLE;			
					// Turn toward ball
					dir = 0;
					u8 dx = 128 + p->pos.x - b->x;
					if(dx < 128 + -MIN_DIFF)
						dir += JOY_INPUT_DIR_RIGHT;
					else if(dx > 128 + MIN_DIFF)
						dir += JOY_INPUT_DIR_LEFT;
					u8 dy = 128 + p->pos.y - b->y;
					if(dy > 128 + MIN_DIFF)
						dir += JOY_INPUT_DIR_UP;
					else if(dy < 128 + -MIN_DIFF)
						dir += JOY_INPUT_DIR_DOWN;
					p->dir = DirToIdx[dir];
				}
			}
			if(p->pos.x < 8)
				p->pos.x = 8;
			else if(p->pos.x > (u8)(255 - 8))
				p->pos.x = (u8)(255 - 8);
			if(p->pos.y < 16)
				p->pos.y = 16;
			else if(p->pos.y > FIELD_SIZE-1)
				p->pos.y = FIELD_SIZE-1;
		
			if(p->action == ACTION_IDLE)
				p->sprtIdX = 0;
			else //if(p->action == ACTION_RUN)
				p->sprtIdX = Anim_RunFrames[(g_Frame >> 2) & 0x03];
			p->sprtIdY = p->dir;

			const u8* ptr = g_PlayerSprite + g_PlayerSprite_index[p->sprtIdX + (p->sprtIdY << 3)];
			u8 minX = *ptr >> 4;
			u8 maxX = *ptr & 0x0F;
			++ptr;
			u8 minY = *ptr >> 4;
			u8 maxY = *ptr & 0x0F;

			p->minX = minX;
			p->minY = minY;
			p->sprtPos.x = p->pos.x + SPRT_OFS_X + minX;
			p->sprtPos.y = p->pos.y + SPRT_OFS_Y + (512 * g_WritePage) + minY;
			p->sprtSize.x = (maxX - minX + 1);
			p->sprtSize.y = (maxY - minY + 1);

			// Do backup
			p->visible = 0;
			//if((p->pos.y + SPRT_OFS_Y + 16 >= scrollOffset) && (p->pos.y + SPRT_OFS_Y <= scrollOffset + LINE_NB))
			{
				p->visible = 1;
				VDP_HMMM(
					p->sprtPos.x,
					p->sprtPos.y,
					BACK_X + (i * 16), 
					BACK_Y + (16 * g_WritePage), 
					p->sprtSize.x + 2, 
					p->sprtSize.y);
			}
			p++;
		}

		// Player change
		u8 row = Keyboard_Read(KEY_ROW(KEY_GRAPH));
		if((row & KEY_FLAG(KEY_GRAPH)) == 0)
		{
			playerChara = (playerChara + 1) % (PLAYER_NB / 2);
		}
			
		//---------------------------------------------------------------------
		// DRAW PLAYERS
		//---------------------------------------------------------------------
		p = &g_Player[g_WritePage][0];
		for(i = 0; i < PLAYER_NB; ++i)
		{
			if(p->visible)
			{
				// Do draw
				VDP_LMMM(
					(TEAM_BMP_X * p->team) + (p->sprtIdX * 16) + p->minX, 
					TEAM_BMP_Y + (p->sprtIdY * 16) + p->minY, 
					p->sprtPos.x,
					p->sprtPos.y,
					p->sprtSize.x, 
					p->sprtSize.y, 
					VDP_OP_TIMP);
				/*VDP_HMMV(
					p->sprtPos.x,
					p->sprtPos.y,
					p->sprtSize.x, 
					p->sprtSize.y,
					(p->dir + 2) << 4 | (p->dir + 2));*/
				/*VDP_HMMM(
					(TEAM_BMP_X * p->team) + (p->sprtIdX * 16) + p->minX, 
					TEAM_BMP_Y + (p->sprtIdY * 16) + p->minY, 
					p->sprtPos.x,
					p->sprtPos.y,
					p->sprtSize.x, 
					p->sprtSize.y);*/
					
				p->drawn = 1;

				// VDP_LMMM(TEAM_BMP_X * p->team + (sx * 16), TEAM_BMP_Y + (sy * 16), p->pos.x + SPRT_OFS_X, p->pos.y + SPRT_OFS_Y + (512 * g_WritePage), 16, 16, VDP_OP_TIMP); // Draw
			}
			p++;
		}	

		//---------------------------------------------------------------------
		g_VBlank = 0;
		WaitVBlank();
	}
}