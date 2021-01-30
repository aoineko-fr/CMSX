//_____________________________________________________________________________
//   ▄▄▄        ▄▄              ▄▄     ▄▄▄                                     
//  ██   ▄█▀▄  ▄██  ██▀   ▄█▀▄ ██ ▀   ▀█▄  ▄█▀▄ ▄█▀▀ ▄█▀▀ ▄███ ██▄▀            
//  ▀█▄█ ▀█▄▀ ▀▄██ ▄██    ▀█▄▀ ██▀    ▄▄█▀ ▀█▄▀ ▀█▄▄ ▀█▄▄ ▀█▄▄ ██              
//_____________________________________________________________________________

// CMSX
#include "core.h"
#include "color.h"
#include "input.h"
#include "memory.h"
#include "vdp.h"
#include "msxi/msxi_unpack.h"
#include "bios.h"
#include "math.h"
#include "print.h"
#include "draw.h"
#include "pt3/pt3_player.h"
#include "ayfx/ayfx_player.h"
// GoS
#include "gos_define.h" 

//=============================================================================
//
//   D E F I N E S
//
//=============================================================================

//-----------------------------------------------------------------------------
// DEFINES
#define VERSION			"V0.3.9"

#define CHARA_NB 		14
#define BALL_ID			CHARA_NB

#define LINE_NB			212	// 192 or 212
#define FIELD_SIZE		384 // 256 or 384
#define TEAM_BMP_X		128
#define TEAM_BMP_Y		384
#define BALL_BMP_X		240
#define BALL_BMP_Y		(1024-32)
#define BACK_X			0
#define BACK_Y			(1024-32)
#define SPRT_OFS_X		(-7)
#define SPRT_OFS_Y		(-15)
#define MIN_DIFF		8

#if (TARGET_TYPE == TARGET_TYPE_BIN)
	#define HBLANK_LINE		(u8)(255-9)
#elif (TARGET_TYPE == TARGET_TYPE_ROM)
	#define HBLANK_LINE		(u8)(255-5)
#elif (TARGET_TYPE == TARGET_TYPE_DOS)
	#define HBLANK_LINE		(u8)(255-10)
#endif

// 
#define V8(a) (*((u8*)(&a)))
#define V16(a) (*((u16*)(&a)))

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
	
} CharaRender;

/// Character information structure
typedef struct
{
	Vector816	loc;			///< Position
	Vector816	target;			///< Position
	
	u8			dir     : 3;	///< Direction (0-7)
	u8			pos     : 4;	///< Character position team ID (0-15)
	u8			nearest : 1;	///< Is the nearest character from the ball
	
	u8			action  : 4;	///< Action ID (0-15): 0=idle, 1=run, 2=tackle @see ACTION_ID
	u8			team    : 1;	///< Team ID (0 or 1)
	u8			hasball : 1;	///< Is having ball?
	u8			moving  : 1;	///< Is moving?: 0=false, 1=true
	u8			display : 1;	///< Should be displayed: 0=false, 1=true
	
	u8			visible : 1;	///< Is character visible
	u8			drawn   : 1;	///< Has character been drawn
	u8			sprtIdX : 3;	///< 
	u8			sprtIdY : 3;	///< 

	u8			minX    : 4;	///< 
	u8			minY    : 4;	///< 
	
	u8			id;

	CharaRender	render[2];
	
} CharaInfo;


typedef struct
{
	u8			drawn   : 1;	///< Actor have been drawn
	u8			dummy_  : 7;	///< 0
	Vector816	destPos;		///< Bitmap destination coordinate
	Vector8		size;			///< Birmap size
	
} ActorRender;


typedef struct
{
	u8			id;				///< Actor ID
	u8			visible : 1;	///< Actor have to be drawn
	u8			dummy_  : 7;	///< 
	Vector816	pos;			///< Position
	Vector816	srcPos;			///< Bitmap source coordinate	
	ActorRender	render;			///< Bitmap render info
	ActorRender	prevRender;		///< Bitmap render info of frame N-1
} Actor;

typedef struct
{
	Actor		actor;			///< Actor data (must be first strcture member)
} BallActor;

typedef struct
{
	Actor		actor;			///< Actor data (must be first strcture member)

	u8			team    : 1;	///< Team ID (0 or 1)
	u8			hasball : 1;	///< Is having ball?
	u8			__10    : 2;
	u8			role    : 4;	///< Character position team ID (0-15)

	u8			dir     : 3;	///< Player Direction (0-7)
	u8			moving  : 1;	///< Is moving?: 0=false, 1=true
	u8			nearest : 1;	///< Is the nearest player from the ball
	u8			__20    : 3;
	
	u8			sprtIdX : 3;	///< 
	u8			sprtIdY : 3;	///< 
	u8			__30    : 2;

	u8			minX    : 4;	///< 
	u8			minY    : 4;	///< 

	// Behavior
	u8			behavior : 4;	///< Current hebavior ID (@see AIBehavior)
	u8			action   : 4;	///< Current action ID (@see PlayerAction)
	Vector816	target;			///< Target position

	
} PlayerActor;


//=============================================================================
//
//   D A T A
//
//=============================================================================

// Bitmap Sprites
#include "data\player.data.h"
// HW Sprites
#include "data\sprite.data.h"
#include "data\ball.data.h"
#include "font\font_carwar.h"
// Music
#include "pt3\pt3_notetable2.h"
#include "data\music00.h"
// SFX
#include "data\ayfx_bank.h"


u8 g_DisplayPage = 0;	///< Index of the current VDP display page (will be switch after V-Blank)
u8 g_WritePage = 1;		///< Index of the current VDP work page (will be switch after V-Blank)
u8 g_VBlank = 0;		///< Flag to inform game thread that a V-Blank occured
u8 g_Frame = 0;			///< Render Frame counter
u8 g_FrameGP = 0;		///< Gamplay Frame counter

u8 g_ScrollOffset = 0;
u8 g_PrevScrollOffset = 0;

u8 g_PlayerChara = 6;

/// Players information
CharaInfo g_Chara[2][CHARA_NB];
CharaInfo g_Ball[2];

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
	{ 128, 376 }, // G
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
		out		(P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(P_VDP_ADDR), a
		in		a, (P_VDP_STAT)
		//  Call H-Blank if bit #0 of S#1 is set 
		rrca
		jp		nc, _no_hblank
		call	_HBlankHook // call to C function HBlankHook() 
	_no_hblank:
		// Reset R#15 to S#0
		xor		a           		
		out		(P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(P_VDP_ADDR),a
	__endasm;

	// Call((u16)HookBackup_KEYI);
}

/// H_TIMI interrupt hook
void VBlankHook()
{
	VDP_SetPage(V8(g_DisplayPage) * 2);
	g_VBlank = 1;
	g_FrameGP++;
	
	// Call((u16)HookBackup_KEYI);
	CharaInfo* p = &g_Chara[g_DisplayPage][g_PlayerChara];			
	VDP_SetSpritePosition(SPRITE_Player1, p->loc.x - 2, p->loc.y - 19);
	
	PT3_Decode();
	ayFX_Update();
	PT3_UpdatePSG();
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
void Player_ComputeTaget(CharaInfo* p, u8 area)
{
	if(p->team == 0)
	{
		/*switch(area)
		{
		// case 5:
		// case 4:
		default:
			p->target = FormationDef[p->pos];
			break;
		case 3:
			p->target.x = (FormationDef[p->pos].x * 3 + FormationAtt[p->pos].x) / 4;
			p->target.y = (FormationDef[p->pos].y * 3 + FormationAtt[p->pos].y) / 4;
			break;
		case 2:
			p->target.x = (FormationDef[p->pos].x + FormationAtt[p->pos].x) / 2;
			p->target.y = (FormationDef[p->pos].y + FormationAtt[p->pos].y) / 2;
			break;
		case 1:
			p->target.x = (FormationDef[p->pos].x + FormationAtt[p->pos].x * 3) / 4;
			p->target.y = (FormationDef[p->pos].y + FormationAtt[p->pos].y * 3) / 4;
			break;
		case 0:
			p->target = FormationAtt[p->pos];
			break;
		};*/
		switch(area)
		{
		case 5:
		case 4:
		case 3:
			p->target = FormationDef[p->pos];
			break;
		case 2:
			p->target.x = (FormationDef[p->pos].x + FormationAtt[p->pos].x) / 2;
			p->target.y = (FormationDef[p->pos].y + FormationAtt[p->pos].y) / 2;
			break;
		case 1:
		case 0:
			p->target = FormationAtt[p->pos];
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
			p->target.x = 255 - FormationDef[p->pos].x;
			p->target.y = 383 - FormationDef[p->pos].y;
			break;
		case 3:
			p->target.x = 255 - (FormationDef[p->pos].x + FormationAtt[p->pos].x) / 2;
			p->target.y = 383 - (FormationDef[p->pos].y + FormationAtt[p->pos].y) / 2;
			break;
		case 4:
		case 5:
			p->target.x = 255 - FormationAtt[p->pos].x;
			p->target.y = 383 - FormationAtt[p->pos].y;
			break;
		};

		/*switch(area)
		{
		// case 0:
		// case 1:
		default:
			p->target.x = 255 - FormationDef[p->pos].x;
			p->target.y = 383 - FormationDef[p->pos].y;
			break;
		case 2:
			p->target.x = 255 - (FormationDef[p->pos].x * 3 + FormationAtt[p->pos].x) / 4;
			p->target.y = 383 - (FormationDef[p->pos].y * 3 + FormationAtt[p->pos].y) / 4;
			break;
		case 3:
			p->target.x = 255 - (FormationDef[p->pos].x + FormationAtt[p->pos].x) / 2;
			p->target.y = 383 - (FormationDef[p->pos].y + FormationAtt[p->pos].y) / 2;
			break;
		case 4:
			p->target.x = 255 - (FormationDef[p->pos].x + FormationAtt[p->pos].x * 3) / 4;
			p->target.y = 383 - (FormationDef[p->pos].y + FormationAtt[p->pos].y * 3) / 4;
			break;
		case 5:
			p->target.x = 255 - FormationAtt[p->pos].x;
			p->target.y = 383 - FormationAtt[p->pos].y;
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
		VDP_CommandHMMV(0, (j * 16) + y, 256, 16, col);
	}
	// Lines
	VDP_CommandHMMV(8,   8 + y, 240, 2, 0xFF); // Top
	VDP_CommandHMMV(8, (FIELD_SIZE/2 - 1) + y, 240, 2, 0xFF); // Mid
	VDP_CommandHMMV(8, FIELD_SIZE-8 + y, 240, 2, 0xFF); // Bot
	VDP_CommandHMMV(8,   8 + y, 2, FIELD_SIZE-16, 0xFF); // Left
	VDP_CommandHMMV(246, 8 + y, 2, FIELD_SIZE-16, 0xFF); // Right
	// Goal area
	VDP_CommandHMMV(64,  8 + y, 2,  48, 0xFF);
	VDP_CommandHMMV(190, 8 + y, 2,  48, 0xFF);
	VDP_CommandHMMV(64, 56 + y, 128, 2, 0xFF);
	// Goal area
	VDP_CommandHMMV(64,  384-56 + y, 2,  48, 0xFF);
	VDP_CommandHMMV(190, 384-56 + y, 2,  48, 0xFF);
	VDP_CommandHMMV(64,  384-56 + y, 128, 2, 0xFF);
	// Circle
	Draw_Circle(128, 192 + y, 31, 0xF, 0);
	Draw_Circle(128, 192 + y, 32, 0xF, 0);
}

///
u16 GetSqrDistance(const Vector816* from, const Vector816* to)
{
	u8 dx = Abs8(from->x - to->x);
	u16 dy = Abs16(from->y - to->y);
	return dx*dx + dy*dy;
}

//-----------------------------------------------------------------------------
//
// HARDWARE SPRITES
//
//-----------------------------------------------------------------------------

/// Initialize sprite system (load data to VRAM and setup attributes)
void InitializeHWSprite()
{
	VDP_SetSpriteTables(0x1C000, 0x1CA00);	
	VDP_EnableSprite(true);

	// Hide all sprites
	for(u8 i = 0; i < 32; ++i)
		VDP_SetSpritePositionY(i, 212);	
	
	// Text sprites
	Print_SetColor(9, 0);
	Print_SetFontSprite(g_Font_Carwar, 48, 0);
	// Score sprites
	Print_SetSpriteID(SPRITE_Score);
	Print_SetPosition(2, 2);
	Print_DrawText("3-0");
	// Timer sprites
	Print_SetSpriteID(SPRITE_Timer);
	Print_SetPosition(256-2-5*8, 2);
	Print_DrawText("43:25");
	
	// Player selection sprite
	VDP_LoadSpritePattern(g_UISprite, 0, 48);
	VDP_SetSpriteExUniColor(SPRITE_Player1, 0,   0, 32, 3);
	// VDP_SetSpriteExUniColor(SPRITE_Player2, 0, 216, 32, 3);
	// VDP_SetSpriteExUniColor(SPRITE_Player3, 0, 216, 32, 3);
	// VDP_SetSpriteExUniColor(SPRITE_Player4, 0, 216, 32, 3);

	// Goal (horizontal bar)
	VDP_SetSpriteExUniColor(SPRITE_GoalH+0,  96, 216/*352*/, 40, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+1, 104, 352, 41, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+2, 112, 352, 41, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+3, 120, 352, 41, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+4, 128, 352, 41, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+5, 136, 352, 41, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+6, 144, 352, 41, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalH+7, 152, 352, 42, 0xF);

	// Goal (left post)
	VDP_SetSpriteExUniColor(SPRITE_GoalL+0,  96, 360, 43, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalL+1,  96, 368, 43, 0xF);

	// Goal (right post)
	VDP_SetSpriteExUniColor(SPRITE_GoalR+0, 152, 360, 44, 0xF);
	VDP_SetSpriteExUniColor(SPRITE_GoalR+1, 152, 368, 44, 0xF);
}

///
void UpdateHWSprite()
{
	if(g_ScrollOffset != g_PrevScrollOffset)
	{
		loop(i, 8)
			VDP_SetSpritePositionY(SPRITE_Score + i, 2 + g_ScrollOffset);				
	}

	if((g_PrevScrollOffset < 140) && (g_ScrollOffset >= 140))
	{
		VDP_SetSpritePositionY(SPRITE_GoalH+0, 352);
	}
	else if((g_PrevScrollOffset >= 140) && (g_ScrollOffset < 140))
	{
		VDP_SetSpritePositionY(SPRITE_GoalH+0, 216);
	}
}

//-----------------------------------------------------------------------------
// BITMAP SPRITES
//-----------------------------------------------------------------------------

///
void RestaureBG(CharaInfo* p) __FASTCALL
{
	// Do restore
	if(p->drawn)
	{
		VDP_CommandHMMM(
			BACK_X + (p->id * 16),
			BACK_Y + (16 * g_WritePage), 
			p->render[0].sprtPos.x,
			p->render[0].sprtPos.y,
			p->render[0].sprtSize.x + 2, 
			p->render[0].sprtSize.y);
		p->drawn = 0;
	}

	if(p->id == BALL_ID)
		g_Ball[g_WritePage] = g_Ball[g_DisplayPage]; // swap player data
	else
		g_Chara[g_WritePage][p->id] = g_Chara[g_DisplayPage][p->id]; // swap player data
}

///
void ComputeCharaSprite(CharaInfo* p) __FASTCALL
{
	const u8* ptr = g_PlayerSprite + g_PlayerSprite_index[p->sprtIdX + (p->sprtIdY << 3)];
	u8 minX = *ptr >> 4;
	u8 maxX = *ptr & 0x0F;
	++ptr;
	u8 minY = *ptr >> 4;
	u8 maxY = *ptr & 0x0F;

	p->minX = minX;
	p->minY = minY;
	p->render[0].sprtPos.x = p->loc.x + SPRT_OFS_X + minX;
	p->render[0].sprtPos.y = p->loc.y + SPRT_OFS_Y + (512 * g_WritePage) + minY;
	p->render[0].sprtSize.x = (maxX - minX + 1);
	p->render[0].sprtSize.y = (maxY - minY + 1);
}

void ComputeBallSprite()
{
	CharaInfo* p = &g_Ball[g_WritePage];
	p->render[0].sprtPos.x = p->loc.x - 2;
	p->render[0].sprtPos.y = p->loc.y - 2;
	p->render[0].sprtSize.x = 4;
	p->render[0].sprtSize.y = 4;
}

///
void BackupBG(CharaInfo* p) __FASTCALL
{
	// Do backup
	p->visible = 0;
	//if((p->loc.y + SPRT_OFS_Y + 16 >= g_ScrollOffset) && (p->loc.y + SPRT_OFS_Y <= g_ScrollOffset + LINE_NB))
	{
		p->visible = 1;
		VDP_CommandHMMM(
			p->render[0].sprtPos.x,
			p->render[0].sprtPos.y,
			BACK_X + (p->id * 16), 
			BACK_Y + (16 * g_WritePage), 
			p->render[0].sprtSize.x + 2, 
			p->render[0].sprtSize.y);
	}
}

///
void DrawCharaSprite(CharaInfo* p) __FASTCALL
{
	if(p->visible)
	{
		// Do draw
		VDP_CommandLMMM(
			(TEAM_BMP_X * p->team) + (p->sprtIdX * 16) + p->minX, 
			TEAM_BMP_Y + (p->sprtIdY * 16) + p->minY, 
			p->render[0].sprtPos.x,
			p->render[0].sprtPos.y,
			p->render[0].sprtSize.x, 
			p->render[0].sprtSize.y, 
			VDP_OP_TIMP);
			
		p->drawn = 1;
	}
}

///
void DrawBallSprite()
{
	CharaInfo* p = &g_Ball[g_WritePage];
	if(p->visible)
	{
		// Do draw
		VDP_CommandLMMM(
			BALL_BMP_X + 4 * (g_FrameGP & 0x3), 
			BALL_BMP_Y, 
			p->render[0].sprtPos.x,
			p->render[0].sprtPos.y,
			p->render[0].sprtSize.x, 
			p->render[0].sprtSize.y, 
			VDP_OP_TIMP);
			
		p->drawn = 1;
	}
}







/// Restaure N-2 frame background if needed
void Actor_RestaureBG(Actor* a) __FASTCALL
{
	if(a->prevRender.drawn)
	{
		VDP_CommandHMMM(
			BACK_X + (a->id * 16),
			BACK_Y + (16 * g_WritePage), 
			a->prevRender.destPos.x & 0xFE,
			a->prevRender.destPos.y,
			a->prevRender.size.x + 2, // @todo Could be optimized
			a->prevRender.size.y);
	}
	a->prevRender = a->render;
}

/// Update actor
void Actor_Update(Actor* a) __FASTCALL
{
	a->pos.x++;
	a->pos.y++;
	if(a->pos.y >= 384)
		a->pos.y = 0;

	a->srcPos.x = BALL_BMP_X + 4 * ((g_FrameGP >> 2) & 0x3);
	a->srcPos.y = BALL_BMP_Y;

	a->render.destPos.x = a->pos.x;
	a->render.destPos.y = a->pos.y - 2 + (512 * g_WritePage);
	a->render.size.x = 4; 
	a->render.size.y = 4;
	
	a->visible = 1;

	u8 row6 = Keyboard_Read(KEY_ROW(KEY_GRAPH));
	if((row6 & KEY_FLAG(KEY_GRAPH)) == 0)
		a->visible = 0;
}

/// Backup actor background
void Actor_Backup(Actor* a) __FASTCALL
{
	if(a->visible)
	{
		VDP_CommandHMMM(
			a->render.destPos.x & 0xFE,
			a->render.destPos.y,
			BACK_X + (a->id * 16), 
			BACK_Y + (16 * g_WritePage), 
			a->render.size.x + 2,  // @todo Could be optimized
			a->render.size.y);
	}
}

/// Backup actor background
void Actor_Draw(Actor* a) __FASTCALL
{
	a->render.drawn = 0;
	if(a->visible)
	{
		VDP_CommandLMMM(
			a->srcPos.x, 
			a->srcPos.y, 
			a->render.destPos.x,
			a->render.destPos.y,
			a->render.size.x, 
			a->render.size.y, 
			VDP_OP_TIMP);
							
		a->render.drawn = 1;
	}
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
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_SetFrequency(VDP_FREQ_50HZ);
	VDP_SetLineCount(LINE_NB == 192 ? VDP_LINE_192 : VDP_LINE_212);
	VDP_SetColor(0x00);
	VDP_SetPage(0);

	// Speed-up VDP setup
	VDP_EnableSprite(false);
	VDP_EnableDisplay(false);
	
	// Field
	DrawField(0);					// Page 0-1
	DrawField(512);					// Page 2-3
	// Buffer 	
	VDP_CommandHMMV(0, 384, 256, 128, 0);	// Page 1
	VDP_CommandHMMV(0, 896, 256, 128, 0);	// Page 3
	VDP_CommandWait();
	const static u8 ReplaceColorT0[] = { 1, 6, 15 };
	const static u8 ReplaceColorT1[] = { 3, 6, 15, 11, 6, 3, 6 };
	
	// Load ball bitmap to VRAM
	loop(bi, 4)
	{
		VDP_CommandHMMC(g_BallSprite + bi * 8, BALL_BMP_X + bi * 4, BALL_BMP_Y, 4, 4);
	}
	
	// Load players bitmap to VRAM
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
		CharaInfo* p = &g_Chara[j][0];
		for(u8 i = 0; i < CHARA_NB; ++i)
		{	
			p->id = i;
			p->pos = i % 7;
			if(i < CHARA_NB / 2)
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
			p->loc = p->target;
			
			p->action = ACTION_Idle;
			p->hasball = false;
			p->moving = false;
			p->display = true;
			p->nearest = false;
			p->drawn = 0;
			p++;
		}

		p = &g_Ball[j];
		p->id = BALL_ID;
		p->loc.x = 100;
		p->loc.y = 100;
		p->display = true;
		p->drawn = 0;
	}
	
	VDP_SetHBlankLine(HBLANK_LINE);
	u8 ballArea = 0;
	Vector816 ballPosition;

	CharaInfo* p;			
	u8 dir;
		
	// Initialize hardware sprites
	InitializeHWSprite();

	// INIT PT3
	PT3_Init();
	PT3_SetNoteTable(PT3_NT2);
	PT3_SetLoop(true);
	PT3_InitSong(g_Music00);
	// PT3_Play();
	
	// INIT ayFX
	ayFX_InitBank(g_ayfx_bank);
	ayFX_SetChannel(PSG_CHANNEL_C);
	ayFX_SetMode(AYFX_MODE_FIXED);
	ayFX_PlayBank(0, 0);

	Bios_SetHookCallback(H_KEYI, InterruptHook);
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	u8 prevRow6 = 0xFF;


	// Actor aBall;
	// aBall.id = 14;
	// aBall.pos.x = 100;
	// aBall.pos.y = 100;
	// aBall.render.drawn = 0;
	// aBall.prevRender.drawn = 0;


	VDP_EnableDisplay(true);
	while(1)
	{
		//---------------------------------------------------------------------
		g_VBlank = 0;
		WaitVBlank();
		
		
		// Actor_RestaureBG(&aBall);
		// Actor_Update(&aBall);
		// Actor_Backup(&aBall);
		// Actor_Draw(&aBall);
	
		
		
		

		// ballPosition = aBall.pos;
		ballPosition = g_Chara[g_DisplayPage][g_PlayerChara].loc;
		ballArea = ballPosition.y >> 6;
		
		//---------------------------------------------------------------------
		// SCROLLING
		//---------------------------------------------------------------------
		
		// Follow the ball Y position
		if(ballPosition.y < (LINE_NB / 2))
			g_ScrollOffset = 0;
		else if(ballPosition.y > FIELD_SIZE - (LINE_NB / 2))
			g_ScrollOffset = (u8)(FIELD_SIZE - LINE_NB);
		else
			g_ScrollOffset = (u8)ballPosition.y - (LINE_NB / 2);
		// Handle scroll events
		if((g_PrevScrollOffset < 256 - LINE_NB) && (g_ScrollOffset >= 256 - LINE_NB))
			VDP_EnableHBlank(true);
		else if((g_PrevScrollOffset >= 256 - LINE_NB) && (g_ScrollOffset < 256 - LINE_NB))
			VDP_EnableHBlank(false);
		// Set the screen scrolling offset
		VDP_SetVerticalOffset(g_ScrollOffset);
		
		UpdateHWSprite(); // Need to be called before g_PrevScrollOffset = g_ScrollOffset
				
		// Backup previous scrolling offset
		g_PrevScrollOffset = g_ScrollOffset;

		//---------------------------------------------------------------------
		// RESTORE BACKGROUND
		//---------------------------------------------------------------------
		u8 nearestPly = 0xFF;
		u16 nearestDist = 0xFFFF;
		p = &g_Chara[g_WritePage][0];
		u8 i;		
		for(i = 0; i < CHARA_NB; ++i)
		{
			RestaureBG(p);
			
			if(p->team == 1)
			{
				u16 sqrDist = GetSqrDistance(&(p->loc), &ballPosition);
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
			g_Chara[g_WritePage][nearestPly].nearest = true;

		// RestaureBG(&g_Ball[g_WritePage]);
		
		//---------------------------------------------------------------------
		// UPDATE PLAYERS & BACKUP BACKGROUND
		//---------------------------------------------------------------------
		p = &g_Chara[g_WritePage][0];			
		for(i = 0; i < CHARA_NB; ++i)
		{
			if(i == g_PlayerChara) // controller 1
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
					p->action = ACTION_Run;
					p->loc.x += RunMove[p->dir].x - 1;
					p->loc.y += RunMove[p->dir].y - 1;
				}
				else
					p->action = ACTION_Idle;
			}
			else // AI
			{
				Vector816* t = &ballPosition;
				if(p->nearest == 0)
				{
					Player_ComputeTaget(p, ballArea);
					t = &p->target;
				}
				
				u16 sqrD = GetSqrDistance(&(p->loc), t);
				if (sqrD > 16*16) // Move
				{
					p->action = ACTION_Run;
					// Turn toward movement
					dir = 0;
					if(p->loc.x < t->x)
						dir += JOY_INPUT_DIR_RIGHT;
					else if(p->loc.x > t->x)
						dir += JOY_INPUT_DIR_LEFT;
					if(p->loc.y > t->y)
						dir += JOY_INPUT_DIR_UP;
					else if(p->loc.y < t->y)
						dir += JOY_INPUT_DIR_DOWN;
					p->dir = DirToIdx[dir];
					// Move
					p->loc.x += RunMove[p->dir].x - 1;
					p->loc.y += RunMove[p->dir].y - 1;
				}
				else // Idle
				{
					Vector816* b = &ballPosition;
					p->action = ACTION_Idle;
					// Turn toward ball
					dir = 0;
					u8 dx = 128 + p->loc.x - b->x;
					if(dx < 128 + -MIN_DIFF)
						dir += JOY_INPUT_DIR_RIGHT;
					else if(dx > 128 + MIN_DIFF)
						dir += JOY_INPUT_DIR_LEFT;
					u8 dy = 128 + p->loc.y - b->y;
					if(dy > 128 + MIN_DIFF)
						dir += JOY_INPUT_DIR_UP;
					else if(dy < 128 + -MIN_DIFF)
						dir += JOY_INPUT_DIR_DOWN;
					p->dir = DirToIdx[dir];
				}
			}
			if(p->loc.x < 8)
				p->loc.x = 8;
			else if(p->loc.x > (u8)(255 - 8))
				p->loc.x = (u8)(255 - 8);
			if(p->loc.y < 16)
				p->loc.y = 16;
			else if(p->loc.y > FIELD_SIZE-1)
				p->loc.y = FIELD_SIZE-1;
		
			if(p->action == ACTION_Idle)
				p->sprtIdX = 0;
			else //if(p->action == ACTION_Run)
				p->sprtIdX = Anim_RunFrames[(g_Frame >> 1) & 0x03];
			p->sprtIdY = p->dir;
			
			ComputeCharaSprite(p);
			BackupBG(p);
			
			p++;
		}

		// ComputeBallSprite();
		// BackupBG(&g_Ball[g_WritePage]);

		// Player change
		u8 row6 = Keyboard_Read(KEY_ROW(KEY_GRAPH));
		if(((row6 & KEY_FLAG(KEY_GRAPH)) == 0) && ((prevRow6 & KEY_FLAG(KEY_GRAPH)) != 0))
		{
			g_PlayerChara = (g_PlayerChara + 1) % (CHARA_NB / 2);
			ayFX_PlayBank(0, 0);
		}
		prevRow6 = row6;
			
		//---------------------------------------------------------------------
		// DRAW PLAYERS
		//---------------------------------------------------------------------
		p = &g_Chara[g_WritePage][0];
		loop(i, CHARA_NB)
		{
			DrawCharaSprite(p++);
		}	
		//DrawBallSprite();
	}
}