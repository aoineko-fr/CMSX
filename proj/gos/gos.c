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
#include "profile.h"
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
#define GAME_VERSION	"V0.4.0"

#define USE_AUDIO		0
#define LINE_NB			212	// 192 or 212
#define FIELD_SIZE		384 // 256 or 384

#define TEAM_PLAYERS 	7
#define BALL_ID			TEAM_PLAYERS*2
#define INVALID_ID		0xF

#define TEAM_BMP_X		128
#define TEAM_BMP_Y		384
#define BALL_BMP_X		240
#define BALL_BMP_Y		(1024-32)
#define BACK_X			0
#define BACK_Y			(1024-32)
#define SPRT_OFS_X		(-7)
#define SPRT_OFS_Y		(-15)
#define MIN_DIFF		8

#define GOAL_LENGTH		48
#define GOAL_HEIGHT		20
#define SPRT_TXT_Y		2

#if (TARGET_TYPE == TYPE_BIN)
	#define HBLANK_LINE		(u8)(255-9)
#elif (TARGET_TYPE == TYPE_ROM)
	#define HBLANK_LINE		(u8)(255-5)
#elif (TARGET_TYPE == TYPE_DOS)
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



/// Actor render backup data structure
typedef struct
{
	u8			drawn   : 1;	///< Actor have been drawn
	u8			dummy_  : 7;	///< 0
	Vector816	destPos;		///< Bitmap destination coordinate
	Vector8		size;			///< Birmap size
	
} ActorRender;

/// Actor base datastructure
typedef struct
{
	u8			id;				///< Actor ID
	u8			visible : 1;	///< Actor have to be drawn
	u8			dir     : 3;	///< Player Direction (0-7)
	u8			dummy_  : 4;	///< 
	Vector816	pos;			///< Position
	Vector816	srcPos;			///< Bitmap source coordinate	
	ActorRender	render;			///< Bitmap render info
	ActorRender	prevRender;		///< Bitmap render info of frame N-1

} Actor;

/// Player actor data structure
typedef struct
{
	Actor		actor;			///< Actor data (must be first strcture member)

	u8			team    : 1;	///< Team ID (0 or 1)
	u8			hasball : 1;	///< Is having ball?
	u8			moving  : 1;	///< Is moving?: 0=false, 1=true
	u8			__10    : 1;
	u8			role    : 4;	///< Character position team ID (0-15)

	u8			sprtId  : 3;	///< Sprite ID (0-7)
	u8			__30    : 5;

	// Behavior
	u8			behavior : 4;	///< Current hebavior ID (@see AIBehavior)
	u8			action   : 4;	///< Current action ID (@see PlayerAction)
	u8			actStep;		///< Current step into the action
	Vector816	target;			///< Target position
	
} PlayerActor;

/// Ball actor data structure
typedef struct
{
	Actor		actor;			///< Actor data (must be first strcture member)
	Actor*		owner;			///< ID of the actor owning the ball (INVALID_ID if no one have it)
	u8			speed;			///< Ball speed (in Q4.4 format)
	u8			height;			///< Ball height (in Q4.4 format)
	
} BallActor;

// Team data structure
typedef struct
{
	u8			score;			///< Actor data (must be first strcture member)
	u8			attack   : 1;	///< Team has ball and is attacking
	u8			field    : 1;	///< Field part. 0: top, 1: bottom
	u8			__10     : 6;
	u8          nearestPly;
	u16         nearestDist;

} Team;

// Inpur contoller data structure
typedef struct
{
	u8			id       : 2;	///< Controller ID (0-3)
	u8			input    : 2;	///< Controller input (@see ControllerInput)
	u8			actor    : 4;	///< Controlled actor ID (0-15)

} Controller;




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
u8 g_VSynch = false;	///< Flag to inform game thread that a V-Blank occured
u8 g_FrameCount = 0;	///< Render frames counter

u8 g_LastFrame = 0;		///< Last render frames counter
u8 g_ElapsFrame = 0;	///< Last render frames counter
u8 g_LoopCount = 0;		///< Gameplay loop counter

u8 g_ScrollOffset = 0;
u8 g_PrevScrollOffset = 0;
u8 g_BallArea = 0;

u8 g_KeyRow6;
u8 g_KeyPrevRow6;
u8 g_KeyRow8;
u8 g_KeyPrevRow8;

/// Actors
BallActor	g_Ball;
PlayerActor	g_Players[14];
Team		g_Teams[2];
Controller	g_Controllers[4];

u8			g_SortedId[TEAM_PLAYERS*2];

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
	g_VSynch = true;
	g_FrameCount++;
	
	#if (USE_AUDIO)
		PT3_Decode();
		ayFX_Update();
		PT3_UpdatePSG();
	#endif
}

/// Wait for V-Blank period
void WaitVBlank()
{
PROFILE_SECTION_START(10, 0);
	
	while(g_VSynch == false) {}
	g_WritePage = 1 - g_WritePage;
	g_DisplayPage = 1 - g_DisplayPage;
	VDP_SetPage(V8(g_DisplayPage) * 2);
	g_LoopCount++;
	g_ElapsFrame = g_FrameCount - g_LastFrame;
	g_LastFrame = g_FrameCount;

PROFILE_SECTION_END(10, 0);
}

/// Compute target position.
/// @param		p		The player to compute
/// @param		area	The ball position (from 0 to 5)
void Player_ComputeTaget(PlayerActor* p, u8 area)
{
PROFILE_SECTION_START(20, 0);

	Actor* a = (Actor*)p;
	if(p->team == 0)
	{
		switch(area)
		{
		case 5:
		case 4:
		case 3:
			p->target = FormationDef[p->role];
			break;
		case 2:
			p->target.x = (FormationDef[p->role].x + FormationAtt[p->role].x) / 2;
			p->target.y = (FormationDef[p->role].y + FormationAtt[p->role].y) / 2;
			break;
		case 1:
		case 0:
			p->target = FormationAtt[p->role];
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
			p->target.x = 255 - FormationDef[p->role].x;
			p->target.y = 383 - FormationDef[p->role].y;
			break;
		case 3:
			p->target.x = 255 - (FormationDef[p->role].x + FormationAtt[p->role].x) / 2;
			p->target.y = 383 - (FormationDef[p->role].y + FormationAtt[p->role].y) / 2;
			break;
		case 4:
		case 5:
			p->target.x = 255 - FormationAtt[p->role].x;
			p->target.y = 383 - FormationAtt[p->role].y;
			break;
		};
	}
PROFILE_SECTION_END(20, 0);
}

/// Draw game field
void DrawField()
{
	// Field
	for(u8 j = 0; j < 24; ++j)
	{
		u8 col = (j & 1) ? 0xDD : 0xEE;
		VDP_CommandHMMV(0, (j * 16), 256, 16, col);
	}
	// Lines
	VDP_CommandHMMV(8,   8, 240, 2, 0xFF); // Top
	VDP_CommandHMMV(8, (FIELD_SIZE/2 - 1), 240, 2, 0xFF); // Mid
	VDP_CommandHMMV(8, FIELD_SIZE-8, 240, 2, 0xFF); // Bot
	VDP_CommandHMMV(8,   8, 2, FIELD_SIZE-16, 0xFF); // Left
	VDP_CommandHMMV(246, 8, 2, FIELD_SIZE-16, 0xFF); // Right
	// Goal area
	VDP_CommandHMMV(64,  8, 2,  48, 0xFF);
	VDP_CommandHMMV(190, 8, 2,  48, 0xFF);
	VDP_CommandHMMV(64, 56, 128, 2, 0xFF);
	// Goal area
	VDP_CommandHMMV(64,  384-56, 2,  48, 0xFF);
	VDP_CommandHMMV(190, 384-56, 2,  48, 0xFF);
	VDP_CommandHMMV(64,  384-56, 128, 2, 0xFF);
	// Circle
	Draw_Circle(128, 192, 26, 0xF, 0);
	Draw_Circle(128, 192, 27, 0xF, 0);
	Draw_Circle(128, 192, 1,  0xF, 0);
	Draw_Circle(128, 192, 2,  0xF, 0);

	// Copy field in page 2&3
	VDP_CommandYMMM(0, 0, 512, FIELD_SIZE, 0);
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
	Print_SetPosition(2, SPRT_TXT_Y);
	Print_DrawText("3-0");
	// Timer sprites
	Print_SetSpriteID(SPRITE_Timer);
	Print_SetPosition(256-2-5*8, SPRT_TXT_Y);
	Print_DrawText("43:25");
	
	// Player selection sprite
	VDP_LoadSpritePattern(g_UISprite, 0, 48);
	VDP_SetSpriteExUniColor(SPRITE_Player1, 0,   0, 32, 3);
	// VDP_SetSpriteExUniColor(SPRITE_Player2, 0, 216, 32, 3);
	// VDP_SetSpriteExUniColor(SPRITE_Player3, 0, 216, 32, 3);
	// VDP_SetSpriteExUniColor(SPRITE_Player4, 0, 216, 32, 3);

	// // Goal (horizontal bar)
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+0,  96, 216/*352*/, 40, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+1, 104, 352, 41, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+2, 112, 352, 41, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+3, 120, 352, 41, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+4, 128, 352, 41, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+5, 136, 352, 41, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+6, 144, 352, 41, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalH+7, 152, 352, 42, 0xF);

	// // Goal (left post)
	// VDP_SetSpriteExUniColor(SPRITE_GoalL+0,  96, 360, 43, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalL+1,  96, 368, 43, 0xF);

	// // Goal (right post)
	// VDP_SetSpriteExUniColor(SPRITE_GoalR+0, 152, 360, 44, 0xF);
	// VDP_SetSpriteExUniColor(SPRITE_GoalR+1, 152, 368, 44, 0xF);
}

///
void UpdateHWSprite()
{
PROFILE_SECTION_START(30, 0);

	if(g_ScrollOffset != g_PrevScrollOffset)
	{
		loop(i, 8)
			VDP_SetSpritePositionY(SPRITE_Score + i, SPRT_TXT_Y + g_ScrollOffset);				
	}

	// if((g_PrevScrollOffset < 140) && (g_ScrollOffset >= 140))
	// {
		// VDP_SetSpritePositionY(SPRITE_GoalH+0, 352);
	// }
	// else if((g_PrevScrollOffset >= 140) && (g_ScrollOffset < 140))
	// {
		// VDP_SetSpritePositionY(SPRITE_GoalH+0, 216);
	// }
	
	VDP_SetSpriteExUniColor(31, 128, g_ScrollOffset+32, 64 + g_ElapsFrame, 0xF);

PROFILE_SECTION_END(30, 0);
}

//-----------------------------------------------------------------------------
//
// BITMAP SPRITES
//
//-----------------------------------------------------------------------------

/// Restaure N-2 frame background if needed
void Actor_RestaureBG(Actor* a) __FASTCALL
{
PROFILE_SECTION_START(40, 0);

	if(a->prevRender.drawn)
	{
		u8 dx = a->prevRender.destPos.x;
		u8 nx = dx + a->prevRender.size.x;
		dx &= 0xFE;
		nx -= dx;
		nx++;
		nx &= 0xFE;		
		
		VDP_CommandHMMM(
			BACK_X + (a->id * 16),
			BACK_Y + (16 * g_WritePage), 
			dx,
			a->prevRender.destPos.y,
			nx, 
			a->prevRender.size.y);
	}
	a->prevRender = a->render;

PROFILE_SECTION_END(40, 0);
}

/// Backup actor background
void Actor_Backup(Actor* a) __FASTCALL
{
PROFILE_SECTION_START(50, 0);

	if(a->visible)
	{
		u8 dx = a->render.destPos.x;
		u8 nx = dx + a->render.size.x;
		dx &= 0xFE;
		nx -= dx;
		nx++;
		nx &= 0xFE;

		VDP_CommandHMMM(
			dx,
			a->render.destPos.y,
			BACK_X + (a->id * 16), 
			BACK_Y + (16 * g_WritePage), 
			nx,
			a->render.size.y);
	}

PROFILE_SECTION_END(50, 0);
}

/// Backup actor background
void Actor_Draw(Actor* a) __FASTCALL
{
PROFILE_SECTION_START(60, 0);

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

PROFILE_SECTION_END(60, 0);
}

//-----------------------------------------------------------------------------
//
// ACTOR UPDATE
//
//-----------------------------------------------------------------------------

///
void Ball_Reset()
{
	PROFILE_SECTION_START(70, 0);

	g_Ball.actor.pos.x = 128;
	g_Ball.actor.pos.y = FIELD_SIZE/2;
	g_Ball.speed = 0;
	g_Ball.owner = null;

	PROFILE_SECTION_END(70, 0);
}

/// Update actor
void Ball_Update()
{
	PROFILE_SECTION_START(80, 0);

	if(g_Ball.owner == null)
	{
		if(g_Ball.speed > 0)
		{
			g_Ball.actor.pos.x += ((RunMove[g_Ball.actor.dir].x - 1) * g_Ball.speed) >> 4;
			g_Ball.actor.pos.y += ((RunMove[g_Ball.actor.dir].y - 1) * g_Ball.speed) >> 4;
			g_Ball.speed--;

			if(g_Ball.actor.pos.x < 8)
				Ball_Reset();
			else if(g_Ball.actor.pos.x > (u8)(255 - 8))
				Ball_Reset();
			else if(g_Ball.actor.pos.y < 8)
				Ball_Reset();
			else if(g_Ball.actor.pos.y > FIELD_SIZE-8)
				Ball_Reset();
		}

		if(g_Ball.speed < (1 << 4))
		{
			PlayerActor* p = &g_Players[0];
			loop(i, TEAM_PLAYERS*2)
			{
				u16 sqrDist = GetSqrDistance(&(p->actor.pos), &g_Ball.actor.pos);
				if(sqrDist < 3*3)
				{
					g_Ball.owner = (Actor*)p;
					p->hasball = true;
					break;
				}
				p++;
			}
		}
	}
	else
	{
		g_Ball.actor.pos.x = g_Ball.owner->pos.x + 4 * (RunMove[g_Ball.owner->dir].x - 1);
		g_Ball.actor.pos.y = g_Ball.owner->pos.y + 4 * (RunMove[g_Ball.owner->dir].y - 1);
	}
	
	g_Ball.actor.visible = 1;

	PROFILE_SECTION_END(80, 0);
}

/// Update actor
void Ball_Prepare(Actor* a) __FASTCALL
{
	PROFILE_SECTION_START(90, 0);

	if(a->visible)
	{
		a->srcPos.x = BALL_BMP_X + 4 * ((g_FrameCount >> 2) & 0x3);
		a->srcPos.y = BALL_BMP_Y;

		a->render.destPos.x = a->pos.x - 2;
		a->render.destPos.y = a->pos.y - 3 + (512 * g_WritePage);
		a->render.size.x = 4; 
		a->render.size.y = 4;
	}

	PROFILE_SECTION_END(90, 0);
}

/// Update sight information
void Player_UpdateSight(PlayerActor* p) __FASTCALL
{
	PROFILE_SECTION_START(100, 0);

	Team* t = &g_Teams[p->team];

	u16 sqrDist = GetSqrDistance(&(p->actor.pos), &g_Ball.actor.pos);
	if((sqrDist < t->nearestDist) || (t->nearestDist == 0xFFFF))
	{
		t->nearestDist = sqrDist;
		t->nearestPly = p->actor.id;				
	}

	PROFILE_SECTION_END(100, 0);
}

/// Update actor
void Player_UpdateAction(PlayerActor* p) __FASTCALL
{
	PROFILE_SECTION_START(110, 0);

	Actor* a = (Actor*)p;
	u8 dir = 0;
	if(a->id == g_Controllers[0].actor) // Input controller
	{
PROFILE_SECTION_START(112, 0);
		if(IS_KEY_PRESSED(g_KeyRow8, KEY_UP))
			dir |= JOY_INPUT_DIR_UP;
		else if(IS_KEY_PRESSED(g_KeyRow8, KEY_DOWN))
			dir |= JOY_INPUT_DIR_DOWN;
		if(IS_KEY_PRESSED(g_KeyRow8, KEY_RIGHT))
			dir |= JOY_INPUT_DIR_RIGHT;
		else if(IS_KEY_PRESSED(g_KeyRow8, KEY_LEFT))
			dir |= JOY_INPUT_DIR_LEFT;


		// Apply movement
		if(dir != 0)
		{
			p->action = ACTION_Run;
			a->dir = DirToIdx[dir];
			a->pos.x += RunMove[a->dir].x - 1;
			a->pos.y += RunMove[a->dir].y - 1;
		}
		else
			p->action = ACTION_Idle;
PROFILE_SECTION_END(112, 0);
	}
	else // AI controller
	{
PROFILE_SECTION_START(114, 0);
		Vector816* t;
		if(a->id == g_Teams[p->team].nearestPly)
			t = &g_Ball.actor.pos;
		else
		{
			Player_ComputeTaget(p, g_BallArea);
			t = &p->target;
		}
		
		//u16 sqrDist = GetSqrDistance(&(a->pos), t);
		// if (sqrDist > 8*8) // Move
		{
			p->action = ACTION_Run;
			// Turn toward movement
			if(a->pos.x < t->x)
				dir += JOY_INPUT_DIR_RIGHT;
			else if(a->pos.x > t->x)
				dir += JOY_INPUT_DIR_LEFT;
			if(a->pos.y > t->y)
				dir += JOY_INPUT_DIR_UP;
			else if(a->pos.y < t->y)
				dir += JOY_INPUT_DIR_DOWN;
			a->dir = DirToIdx[dir];
			// Move
			a->pos.x += RunMove[a->dir].x - 1;
			a->pos.y += RunMove[a->dir].y - 1;
		}
		// else // Idle
		// {
			// Vector816* b = &g_Ball.actor.pos;
			// p->action = ACTION_Idle;
			// // Turn toward ball
			// u8 dx = 128 + a->pos.x - b->x;
			// if(dx < 128 + -MIN_DIFF)
				// dir += JOY_INPUT_DIR_RIGHT;
			// else if(dx > 128 + MIN_DIFF)
				// dir += JOY_INPUT_DIR_LEFT;
			// u8 dy = 128 + a->pos.y - b->y;
			// if(dy > 128 + MIN_DIFF)
				// dir += JOY_INPUT_DIR_UP;
			// else if(dy < 128 + -MIN_DIFF)
				// dir += JOY_INPUT_DIR_DOWN;
			// a->dir = DirToIdx[dir];	
		// }
PROFILE_SECTION_END(114, 0);
	}
PROFILE_SECTION_START(116, 0);

	// Validate new position
	if(a->pos.x < 8)
		a->pos.x = 8;
	else if(a->pos.x > (u8)(255 - 8))
		a->pos.x = (u8)(255 - 8);
	if(a->pos.y < 16)
		a->pos.y = 16;
	else if(a->pos.y > FIELD_SIZE-1)
		a->pos.y = FIELD_SIZE-1;

	if((a->pos.y < g_ScrollOffset) || (a->pos.y - 15 > (g_ScrollOffset + LINE_NB)))
		a->visible = 0;
	else
		a->visible = 1;

	// Compute sprite ID
	if(p->action == ACTION_Idle)
		p->sprtId = 0;
	else //if(p->action == ACTION_Run)
		p->sprtId = Anim_RunFrames[(g_FrameCount >> 1) & 0x03];
PROFILE_SECTION_END(116, 0);

	PROFILE_SECTION_END(110, 0);
}

/// Perpare actor
void Player_Prepare(PlayerActor* p) __FASTCALL
{
	PROFILE_SECTION_START(120, 0);

	Actor* a = (Actor*)p;

	if(a->visible)
	{
		const u8* ptr = g_PlayerSprite + g_PlayerSprite_index[p->sprtId + (a->dir << 3)];
		u8 minX = *ptr >> 4;
		u8 maxX = *ptr & 0x0F;
		++ptr;
		u8 minY = *ptr >> 4;
		u8 maxY = *ptr & 0x0F;

		a->srcPos.x = minX + (TEAM_BMP_X * p->team) + (16 * p->sprtId);
		a->srcPos.y = minY + TEAM_BMP_Y + (16 * a->dir);

		a->render.destPos.x = a->pos.x + SPRT_OFS_X + minX;
		a->render.destPos.y = a->pos.y + SPRT_OFS_Y + minY + (512 * g_WritePage);
		a->render.size.x = (maxX - minX + 1);
		a->render.size.y = (maxY - minY + 1);
	}

	PROFILE_SECTION_END(120, 0);
}

///
void UpdateController()
{
	PROFILE_SECTION_START(130, 0);

	if(g_Controllers[0].actor != INVALID_ID)
	{
		PlayerActor* p = &g_Players[g_Controllers[0].actor];
		Actor* a = (Actor*)p;

		if(IS_KEY_PRESSED(g_KeyRow8, KEY_SPACE) && !IS_KEY_PRESSED(g_KeyPrevRow8, KEY_SPACE))
		{
			if(p->hasball) // shoot
			{
				p->hasball = false;
				g_Ball.actor.dir = a->dir;
				g_Ball.owner = null;
				g_Ball.speed = 5 << 4;
				#if (USE_AUDIO)
					ayFX_PlayBank(1, 0);
				#endif
			}
			else // tacle
			{
				
			}
		}
		else if(IS_KEY_PRESSED(g_KeyRow6, KEY_GRAPH) && !IS_KEY_PRESSED(g_KeyPrevRow6, KEY_GRAPH))
		{
			if(p->hasball) // pass
			{
				p->hasball = false;
				g_Ball.actor.dir = a->dir;
				g_Ball.owner = null;
				g_Ball.speed = 3 << 4;
				#if (USE_AUDIO)
					ayFX_PlayBank(6, 0);
				#endif
			}
			else // player change
			{
				g_Controllers[0].actor++;
				g_Controllers[0].actor %= TEAM_PLAYERS;
				#if (USE_AUDIO)
					ayFX_PlayBank(0, 0);
				#endif
			}
		}
		
		VDP_SetSpritePosition(SPRITE_Player1, a->pos.x - 2, a->pos.y - 19);
	}

	PROFILE_SECTION_END(130, 0);
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
	DrawField();
	
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
	PlayerActor* p = g_Players;
	for(u8 i = 0; i < TEAM_PLAYERS*2; ++i)
	{		
		p->role = i % 7;
		if(i < TEAM_PLAYERS) // Team A (0)
		{
			// a->dir = 0;
			p->team = 0;
		}
		else                 // Team B (1)
		{
			// a->dir = 4;
			p->team = 1;
		}
		
		p->action = ACTION_Idle;
		p->hasball = false;
		p->moving = false;

		Player_ComputeTaget(p, 5);

		Actor* a = (Actor*)p;
		a->id = i;
		a->pos = p->target;
		a->render.drawn = 0;
		a->prevRender.drawn = 0;

		p++;
	}

	// Initialize ball
	Actor* b = (Actor*)&g_Ball;
	b->id = BALL_ID;
	b->render.drawn = 0;
	b->prevRender.drawn = 0;
	Ball_Reset();
	
	// Initialize controller
	Controller* ctrP1 = &g_Controllers[0];
	ctrP1->id = 0;
	ctrP1->input = INPUT_Keyboard1;
	ctrP1->actor = 6; // team #0, player #6
	
	VDP_SetHBlankLine(HBLANK_LINE);
	g_BallArea = 0;
	Vector816 ballPosition;

	// PlayerActor* p;			
	// Actor* a;			
	u8 dir;
		
	// Initialize hardware sprites
	InitializeHWSprite();

	#if (USE_AUDIO)
		// INIT PT3
		PT3_Init();
		PT3_SetNoteTable(PT3_NT2);
		PT3_SetLoop(true);
		PT3_InitSong(g_Music00);
		PT3_Play();
		
		// INIT ayFX
		ayFX_InitBank(g_ayfx_bank);
		ayFX_SetChannel(PSG_CHANNEL_C);
		ayFX_SetMode(AYFX_MODE_FIXED);
		ayFX_PlayBank(0, 0);
	#endif
	
	Bios_SetHookCallback(H_KEYI, InterruptHook);
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	u8 prevRow6 = 0xFF;


	VDP_EnableDisplay(true);
	while(1)
	{
PROFILE_FRAME_START();

		//---------------------------------------------------------------------
		g_VSynch = false;
		WaitVBlank();

		ballPosition = b->pos;
		g_BallArea = ballPosition.y >> 6;
		
		//---------------------------------------------------------------------
		// SCROLLING
		//---------------------------------------------------------------------
PROFILE_SECTION_START(140, 0);
		
		// Follow the ball Y position
		i16 newOffset = ((i16)ballPosition.y - (LINE_NB / 2));
		if(newOffset < 0)
			newOffset = 0;
		else if(newOffset > FIELD_SIZE - LINE_NB)
			newOffset = FIELD_SIZE - LINE_NB;
			
		u8 scrollSpeed = Abs8((i8)(g_ScrollOffset - newOffset));
		scrollSpeed >>= 3;
		scrollSpeed++;

		if (g_ScrollOffset > newOffset)
			g_ScrollOffset -= scrollSpeed;
		else if (g_ScrollOffset < newOffset)
			g_ScrollOffset += scrollSpeed;

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

PROFILE_SECTION_END(140, 0);

		//---------------------------------------------------------------------
		// RESTORE BACKGROUND
		//---------------------------------------------------------------------
PROFILE_SECTION_START(150, 0);

		loop(i, TEAM_PLAYERS*2)
		{
			PlayerActor* p = &g_Players[i];
			
			Actor_RestaureBG((Actor*)p);
			Player_UpdateAction(p);
		}	
		
		Actor_RestaureBG((Actor*)&g_Ball);
		Ball_Update();

PROFILE_SECTION_END(150, 0);

		//---------------------------------------------------------------------
		// UPDATE ACTORS & BACKUP BACKGROUND
		//---------------------------------------------------------------------
PROFILE_SECTION_START(160, 0);

		loop(i, TEAM_PLAYERS*2)
		{
			PlayerActor* p = &g_Players[i];
			g_SortedId[i] = i;
			if(i > 0)
			{
				for(u8 j = i-1; j != 255; --j)
				{
					PlayerActor* p2 = &g_Players[g_SortedId[j]];
					if(p->actor.pos.y < p2->actor.pos.y)
					{
						g_SortedId[j+1] = g_SortedId[j];
						g_SortedId[j] = i;
					}
					else
						break;
				}
			}
			
			Player_Prepare(p);
			Actor_Backup((Actor*)p);
		}	
		
		Ball_Prepare((Actor*)&g_Ball);
		Actor_Backup((Actor*)&g_Ball);

		g_KeyPrevRow6 = g_KeyRow6;
		g_KeyRow6 = Keyboard_Read(6); // SHIFT CTRL GRAPH CAPS CODE F1 F2 F3

		g_KeyPrevRow8 = g_KeyRow8;
		g_KeyRow8 = Keyboard_Read(8); // SPACE HOME INS DEL LEFT UP DOWN RIGHT

PROFILE_SECTION_END(160, 0);

		//---------------------------------------------------------------------
		// DRAW PLAYERS
		//---------------------------------------------------------------------
PROFILE_SECTION_START(170, 0);

PROFILE_SECTION_START(200, 0);
		Actor_Draw((Actor*)&g_Ball);
PROFILE_SECTION_END(200, 0);
	
		loop(t, 2)
		{
			g_Teams[t].nearestPly = 0xFF;
			g_Teams[t].nearestDist = 0xFFFF;
		}

PROFILE_SECTION_START(210, 0);
		loop(i, TEAM_PLAYERS*2)
		{
			PlayerActor* p = &g_Players[g_SortedId[i]];			
			// PlayerActor* p = &g_Players[i];
			
			Player_UpdateSight(p);
			Actor_Draw((Actor*)p);
		}
PROFILE_SECTION_END(210, 0);

PROFILE_SECTION_START(220, 0);
		if(g_ScrollOffset < 10)
		{
			VDP_CommandHMMV(128 - GOAL_LENGTH/2+1, 1 + (512 * g_WritePage), 2, 8, 0xFF);
			VDP_CommandHMMV(128 + GOAL_LENGTH/2-1, 1 + (512 * g_WritePage), 2, 8, 0xFF);
		}
		else if(g_ScrollOffset > 144)
		{
			VDP_CommandHMMV(128 - GOAL_LENGTH/2+1, 384 - 8 - GOAL_HEIGHT + (512 * g_WritePage), GOAL_LENGTH,  2, 0xFF);
			VDP_CommandHMMV(128 - GOAL_LENGTH/2+1, 384 - 6 - GOAL_HEIGHT + (512 * g_WritePage), 2, GOAL_HEIGHT-1, 0xFF);
			VDP_CommandHMMV(128 + GOAL_LENGTH/2-1, 384 - 6 - GOAL_HEIGHT + (512 * g_WritePage), 2, GOAL_HEIGHT-1, 0xFF);
		}
PROFILE_SECTION_END(220, 0);

PROFILE_SECTION_END(170, 0);
		
		UpdateController();

PROFILE_FRAME_END();
	}
}