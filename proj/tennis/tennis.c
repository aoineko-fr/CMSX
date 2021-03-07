//_____________________________________________________________________________
//  ▄▄▄▄ ▄            ▄▄     ▄▄▄                ▄▄  
//  ██▄  ▄  ██▀▄ ▄▀██ ██    ▀█▄  ▄█▄█ ▄▀██  ██▀ ██▄ 
//  ██   ██ ██ █ ▀▄██ ▀█▄   ▄▄█▀ ██ █ ▀▄██ ▄██  ██ █
//_____________________________________________________________________________

// CMSX
#include "core.h"
#include "vdp.h"
#include "color.h"
#include "bios_hook.h"
#include "bios_port.h"
#include "input.h"
#include "math.h"
#include "game.h"
#include "pt3/pt3_player.h"
#include "ayfx/ayfx_player.h"

//=============================================================================
//
//   D E F I N E S
//
//=============================================================================

//-----------------------------------------------------------------------------
// DEFINES
#define VERSION						"V0.1.0"

// VRAM Tables Address
#define VRAM_NAME_TABLE				0x3C00
#define VRAM_COLOR_TABLE			0x2000
#define VRAM_PATERN_TABLE			0x0000
#define VRAM_SPRITE_ATTRIBUTE_0		0x3F00
#define VRAM_SPRITE_ATTRIBUTE_1		0x3F80
#define VRAM_SPRITE_PATTERN_0		0x1800
#define VRAM_SPRITE_PATTERN_1		0x3800

// Unit conversion (Pixel <> Q10.6)
#define UNIT_TO_PX(a)				(u8)((a) / 64)
#define PX_TO_UNIT(a)				(i16)((a) * 64)

#define MOVE_DIAG					PX_TO_UNIT(1.5f * 0.71f)
#define MOVE_MAIN					PX_TO_UNIT(1.5f * 1.00f)
#define GRAVITY						PX_TO_UNIT(0.2)

#define PLY_MIN_X					PX_TO_UNIT(8)
#define PLY_MAX_X					PX_TO_UNIT(255 - 8)
#define PLY_UP_MIN_X				PX_TO_UNIT(8)
#define PLY_UP_MAX_X				PX_TO_UNIT(88)
#define PLY_DOWN_MIN_X				PX_TO_UNIT(97 + 8)
#define PLY_DOWN_MAX_X				PX_TO_UNIT(191 + 8)

/// Player Actions
enum ACTION
{
	ACTION_IDLE = 0,
	ACTION_WALK_F,
	ACTION_WALK_R,
	ACTION_WALK_L,
	ACTION_SHOOT_R,
	ACTION_SHOOT_L,
	ACTION_SMASH,
	ACTION_MAX,
};

/// Player Actions
enum EVENT
{
	EVENT_NONE = 0,
	EVENT_SHOOT_R,
	EVENT_SHOOT_L,
	EVENT_SMASH,
	EVENT_MAX,
};

/// Sprite list
enum SPRITE
{
	SPRITE_PLY1_BLACK_H = 0,
	SPRITE_PLY1_BLACK_L,
	SPRITE_PLY1_RACKET,

	SPRITE_BALL_OUTLINE,

	SPRITE_PLY2_BLACK_H,
	SPRITE_PLY2_BLACK_L,
	SPRITE_PLY2_RACKET,

	SPRITE_PLY1_CLOTH,
	SPRITE_PLY1_WHITE_H,
	SPRITE_PLY1_WHITE_L,
	SPRITE_PLY1_SKIN_H,
	SPRITE_PLY1_SKIN_L,

	SPRITE_NET_POST_W,
	SPRITE_NET_POST_G,
	SPRITE_NET_LEFT, 
	SPRITE_NET_RIGHT,

	SPRITE_PLY2_CLOTH,
	SPRITE_PLY2_WHITE_H,
	SPRITE_PLY2_WHITE_L,
	SPRITE_PLY2_SKIN_H,
	SPRITE_PLY2_SKIN_L,

	SPRITE_BALL_BODY,
	SPRITE_BALL_SHADOW,

	SPRITE_MAX,
};

#if (SPRITE_MAX >= 32)
	Error! Only 32 sprites can be defined.
#endif

//-----------------------------------------------------------------------------
// TYPES

// Functions
typedef bool (*inputFct)(void);	///< Input check function signature

/// 8-bits vector structure
typedef struct
{
	u8			x;	
	u8			y;	
} Vector8;

/// 16-bits vector structure
typedef struct
{
	i16			x;	
	i16			y;	
} Vector16;

/// Player structure
typedef struct
{
	Vector16	pos;
	Vector8		srcPos;	// Ball screen position (Q8.0 format)
	u8			id;
	u8			action;
	u8			counter;
	u8			step;
	u8			anim;
	u8			prevAnim[2]; // buffered animation frame
	inputFct	inButton1;
	inputFct	inButton2;
	inputFct	inUp;
	inputFct	inDown;
	inputFct	inLeft;
	inputFct	inRight;
} Player;

/// Player structure
typedef struct
{
	Vector16	pos;	// Ball position (Q10.6 format)
	Vector8		srcPos;	// Ball screen position (Q8.0 format)
	i16			height; // Ball height (Q10.6 format)
	u8			dir;	// Ball direction (0-63)
	i16			velXY;  // Ball horizontal velocity (Q10.6 format)
	i16			velZ;   // Ball vertical velocity (Q10.6 format)
	u8          coolDown;
	u8			lastPly;
	u8			bounce;
} Ball;

/// Animation strcture
typedef struct
{
	u8			frame;
	u8			len;
	u8			event;
} Anim;

/// Action strcture
typedef struct
{
	const Anim* animFrames;
	u8			animLen;
	u8			loop;      // looping action?
	u8			interrupt; // can be interrupted?
} Action;

//-----------------------------------------------------------------------------
// FUNCTIONS PROTOTYPE

void StateTitle_Start();
void StateTitle_Update();

void StateMenu_Start();
void StateMenu_Update();

void StateGame_Start();
void StateGame_Update();

void StateTrain_Start();
void StateTrain_Update();

//=============================================================================
//
//   D A T A
//
//=============================================================================

//-----------------------------------------------------------------------------
// ROM DATA - PAGE 0

// Menu title (GM2 tables)
#define ADDR_DATALOGO_NAMES		(0x0200)
#define ADDR_DATALOGO_PATTERNS	(ADDR_DATALOGO_NAMES + sizeof(g_DataLogo_Names))
#define ADDR_DATALOGO_COLORS	(ADDR_DATALOGO_PATTERNS + sizeof(g_DataLogo_Patterns))
#define D_g_DataLogo_Names		__at(ADDR_DATALOGO_NAMES)
#define D_g_DataLogo_Patterns	__at(ADDR_DATALOGO_PATTERNS)
#define D_g_DataLogo_Colors		__at(ADDR_DATALOGO_COLORS)
#include "data_logo.h"

// Menu title ball
#define ADDR_DATALOGOBALL		(ADDR_DATALOGO_COLORS + sizeof(g_DataLogo_Colors))
#define D_g_DataLogoBall		__at(ADDR_DATALOGOBALL)
#include "data_logo_ball.h"

// Court background (GM2 tables)
#define ADDR_DATACOURT_NAMES	(ADDR_DATALOGOBALL + sizeof(g_DataLogoBall))
#define ADDR_DATACOURT_PATTERNS	(ADDR_DATACOURT_NAMES + sizeof(g_DataCourt_Names))
#define ADDR_DATACOURT_COLORS	(ADDR_DATACOURT_PATTERNS + sizeof(g_DataCourt_Patterns))
#define D_g_DataCourt_Names		__at(ADDR_DATACOURT_NAMES)
#define D_g_DataCourt_Patterns  __at(ADDR_DATACOURT_PATTERNS)
#define D_g_DataCourt_Colors    __at(ADDR_DATACOURT_COLORS)
#include "data_court.h"

// Player 1 Sprites
#define ADDR_DATAPLAYER1		(ADDR_DATACOURT_COLORS + sizeof(g_DataCourt_Colors))
#define D_g_DataPlayer1			__at(ADDR_DATAPLAYER1)
#include "data_player1.h"

//-----------------------------------------------------------------------------
// ROM DATA - PAGE 1-2

// Player 2 Sprites
#include "data_player2.h" 
// Racket sprites
#include "data_racket.h"
// Score sprites
#include "data_score.h"
// Ball sprites
#include "data_ball.h"
// Fonts
#include "data_font.h"

// SFX
#include "data_sfx.h"

// Music
#include "data_music.h"
#include "pt3\pt3_notetable2.h"

// Math
#include "mathtable\mt_trigo_Q10.6_64.inc"

// Animation
const Anim g_FramesIdle[] = 
{
	{ 19, 10, EVENT_NONE },
	{ 18, 10, EVENT_NONE },
	{ 19, 10, EVENT_NONE },
	{ 20, 10, EVENT_NONE },
};
const Anim g_FramesWalkF[] = 
{
	{ 3, 8, EVENT_NONE },
	{ 4, 8, EVENT_NONE },
	{ 5, 8, EVENT_NONE },
	{ 4, 8, EVENT_NONE },
};
const Anim g_FramesWalkR[] = 
{
	{ 8, 8, EVENT_NONE },
	{ 7, 8, EVENT_NONE },
	{ 6, 8, EVENT_NONE },
	{ 7, 8, EVENT_NONE },
};
const Anim g_FramesWalkL[] = 
{
	{ 2, 8, EVENT_NONE },
	{ 1, 8, EVENT_NONE },
	{ 0, 8, EVENT_NONE },
	{ 1, 8, EVENT_NONE },
};
const Anim g_FramesShootR[] = 
{
	{ 15,  4, EVENT_NONE },
	{ 16,  8, EVENT_SHOOT_R }, // do shoot
	{ 17, 16, EVENT_NONE },
};
const Anim g_FramesShootL[] = 
{
	{  9,  4, EVENT_NONE },
	{ 10,  8, EVENT_SHOOT_L }, // do shoot
	{ 11, 16, EVENT_NONE },
};
const Anim g_FramesSmash[] = 
{
	{ 12,  4, EVENT_NONE },
	{ 13,  8, EVENT_SMASH }, // do smash
	{ 14, 16, EVENT_NONE },
};

// Actions list
const Action g_Actions[] =
{
	/* ACTION_IDLE    */ { g_FramesIdle,   numberof(g_FramesIdle),   1, 1 },
	/* ACTION_WALK_F  */ { g_FramesWalkF,  numberof(g_FramesWalkF),  1, 1 },
	/* ACTION_WALK_R  */ { g_FramesWalkR,  numberof(g_FramesWalkR),  1, 1 },
	/* ACTION_WALK_L  */ { g_FramesWalkL,  numberof(g_FramesWalkL),  1, 1 },
	/* ACTION_SHOOT_R */ { g_FramesShootR, numberof(g_FramesShootR), 0, 0 },
	/* ACTION_SHOOT_L */ { g_FramesShootL, numberof(g_FramesShootL), 0, 0 },
	/* ACTION_SMASH   */ { g_FramesSmash,  numberof(g_FramesSmash),  0, 0 },
};

// Direction index
// +---+---+---+
// | 0 | 3 | 5 |
// +---+---+---+
// | 1 |   | 6 |
// +---+---+---+
// | 2 | 4 | 7 |
// +---+---+---+

// Movement
const Vector16 g_Move[8] =
{
	{ -MOVE_DIAG, -MOVE_DIAG }, // 🡤
	{ -MOVE_MAIN,          0 }, // 🡠
	{ -MOVE_DIAG,  MOVE_DIAG }, // 🡧
	{          0, -MOVE_MAIN }, // 🡡
	{          0,  MOVE_MAIN }, // 🡣
	{  MOVE_DIAG, -MOVE_DIAG }, // 🡥
	{  MOVE_MAIN,          0 }, // 🡢
	{  MOVE_DIAG,  MOVE_DIAG }, // 🡦
};

const u16 g_SpriteAttrib[2] = { VRAM_SPRITE_ATTRIBUTE_0, VRAM_SPRITE_ATTRIBUTE_1 };
const u16 g_SpritePattern[2] = { VRAM_SPRITE_PATTERN_0, VRAM_SPRITE_PATTERN_1 };

const State g_State_Title = { StateTitle_Update, StateTitle_Start, null };
const State g_State_Menu  = { StateMenu_Update,  StateMenu_Start,  null };
const State g_State_Game  = { StateGame_Update,  StateGame_Start,  null };
const State g_State_Train = { StateTrain_Update, StateTrain_Start, null };

//-----------------------------------------------------------------------------
// RAM DATA

// Global variables
Player g_Player[2];
Ball   g_Ball;
u8     g_WriteBuffer = 0;
u8     g_DisplayBuffer = 1;
u8     g_FlickerShadow = 1;
u8     g_PrevRow6;
u8     g_PrevRow8;
u8     g_KeyRow6 = 0xFF; // SHIFT CTRL GRAPH CAPS CODE F1 F2 F3
u8     g_KeyRow8 = 0xFF; // SPACE HOME INS DEL LEFT UP DOWN RIGHT
u8     g_PrevJoy1;
u8     g_PrevJoy2;
u8     g_Joy1 = 0xFF;
u8     g_Joy2 = 0xFF;

u8     g_IntroFrame = 0;

//=============================================================================
//
//   C O D E
//
//=============================================================================

//-----------------------------------------------------------------------------
// GENERIC

///
void CheckShoot(Player* ply)
{
	if(g_Ball.coolDown > 0)
		return;
		
	u8 dir = 16;
	u8 event = g_Actions[ply->action].animFrames[ply->step].event;

	// Check X coordinate
	u8 minX, maxX;
	switch(event)
	{
	case EVENT_SHOOT_R:
		minX = UNIT_TO_PX(ply->pos.x);
		maxX = minX + 16;
		if(ply->id == 0)
			dir += 2;
		else
			dir -= 2;
		break;
	case EVENT_SHOOT_L:
		maxX = UNIT_TO_PX(ply->pos.x);
		minX = maxX - 16;
		if(ply->id == 0)
			dir -= 2;
		else
			dir += 2;
		break;
	case EVENT_SMASH:
		minX = UNIT_TO_PX(ply->pos.x) - 8;
		maxX = minX + 16;
		break;
	default:
		return;
	};
	
	u8 ballX = g_Ball.srcPos.x;
	if((ballX < minX) || (ballX > maxX))
		return;
	
	// Check Y coordinate
	u8 minY, maxY;
	if(ply->id == 0)
	{
		maxY = ply->srcPos.y;
		minY = maxY - 4;
	}
	else
	{
		minY = ply->srcPos.y;
		maxY = minY + 4;
		dir += 32;
	}
	
	u8 ballY = g_Ball.srcPos.y;
	if((ballY < minY) || (ballY > maxY))
		return;
	
	ayFX_PlayBank(1, 0);

	// Set direction
	if(ply->id == 0)
	{
		if(ply->inLeft())
			dir += 4;
		else if(ply->inRight())
			dir -= 4;
	}
	else
	{
		if(ply->inLeft())
			dir -= 4;
		else if(ply->inRight())
			dir += 4;			
	}
	g_Ball.dir = dir;

	// Set strength
	if(ply->inUp())
		g_Ball.velXY = PX_TO_UNIT(4);
	else if(ply->inDown())
		g_Ball.velXY = PX_TO_UNIT(2);
	else
		g_Ball.velXY = PX_TO_UNIT(3);

	if(event == EVENT_SMASH)
		g_Ball.velZ = 0;
	else
		g_Ball.velZ = PX_TO_UNIT(3);

	// Misc
	g_Ball.lastPly = ply->id;
	g_Ball.coolDown = 10;
	g_Ball.bounce = 0;
}

///
void ShootBallRandom()
{
	// Position
	g_Ball.pos.x = PX_TO_UNIT(128);
	g_Ball.pos.y = PX_TO_UNIT(16);
	g_Ball.height = PX_TO_UNIT(28);

	g_Ball.srcPos.x = UNIT_TO_PX(g_Ball.pos.x);
	g_Ball.srcPos.y = UNIT_TO_PX(g_Ball.pos.y);

	u16 rnd = Math_GetRandom();

	// Direction
	g_Ball.dir = rnd;
	// g_Ball.dir %= 5;
	// g_Ball.dir *= 2;
	// g_Ball.dir += 44;
	g_Ball.dir %= 9;
	g_Ball.dir += 44;

	// Velocity
	if(rnd & BIT_8) // top spine
	{
		g_Ball.velZ = PX_TO_UNIT(2.5);
		g_Ball.velXY = PX_TO_UNIT(3);
	}
	else // lob
	{
		g_Ball.velZ = PX_TO_UNIT(4);
		g_Ball.velXY = PX_TO_UNIT(2);
	}

	// Misc
	g_Ball.lastPly = 0xFF;
	g_Ball.coolDown = 0;
	g_Ball.bounce = 0;
}


/// 
void SetAction(Player* ply, u8 id)
{
	if(ply->action != id)
	{
		ply->action = id;
		ply->counter = 0;
		ply->step = 0;
	}
}

/// 
void UpdateAction(Player* ply) __FASTCALL
{
	const Action* act = &g_Actions[ply->action];
	if(ply->counter >= act->animFrames[ply->step].len) // Finished current animation step
	{
		ply->counter = 0;
		ply->step++;
	}
	if(ply->step >= act->animLen) // Finished last animation step
	{
		if(act->loop) // restart action
		{
			ply->counter = 0;
			ply->step = 0;
		}
		else // stop action and transit to IDLE
		{
			SetAction(ply, ACTION_IDLE);
			UpdateAction(ply);
			return;
		}
	}
	// Handle current action
	switch(act->animFrames[ply->step].event)
	{
	case EVENT_NONE:
	case EVENT_SHOOT_R:
	case EVENT_SHOOT_L:
	case EVENT_SMASH:
	case EVENT_MAX:
		break;
	};
	ply->anim = act->animFrames[ply->step].frame;
	ply->counter++;
}

/// 
void UpdateInput(Player* ply) __FASTCALL
{
	if(g_Actions[ply->action].interrupt)
	{			
		if(ply->inButton1())
		{
			if(g_Ball.height > PX_TO_UNIT(32))
				SetAction(ply, ACTION_SMASH);
			else
			{
				i16 dY = ply->srcPos.y - g_Ball.srcPos.y;
				i16 dX = dY * (i16)g_Cosinus64[g_Ball.dir]; // Q16.0 x Q2.6 => Q10.6
				dX /= 64; // Q10.6  => Q16.0
				
				if((g_Ball.srcPos.x + dX) > ply->srcPos.x)
					SetAction(ply, ACTION_SHOOT_R);
				else
					SetAction(ply, ACTION_SHOOT_L);
			}
		}
		else if(ply->inButton2())
		{
		}
		else
		{
			u8 dir = 4;
			if(ply->inUp())
				dir--;
			else if(ply->inDown())
				dir++;
			if(ply->inLeft())
				dir -= 3;
			else if(ply->inRight())
				dir += 3;	
				
			if(dir == 4) // No move
			{
				ply->action = ACTION_IDLE;
			}
			else
			{
				if(dir > 3)
					dir--;

				// Update player position
				ply->pos.x += g_Move[dir].x;
				ply->pos.y += g_Move[dir].y;
				
				// Validate new X position
				if(ply->pos.x < PLY_MIN_X)
					ply->pos.x = PLY_MIN_X;
				else if(ply->pos.x > PLY_MAX_X)
					ply->pos.x = PLY_MAX_X;
				
				// Validate new Y position
				if(ply->id == 0)
				{
					if(ply->pos.y < PLY_DOWN_MIN_X)
						ply->pos.y = PLY_DOWN_MIN_X;
					else if(ply->pos.y > PLY_DOWN_MAX_X)
						ply->pos.y = PLY_DOWN_MAX_X;
				}
				else
				{
					if(ply->pos.y < PLY_UP_MIN_X)
						ply->pos.y = PLY_UP_MIN_X;
					else if(ply->pos.y > PLY_UP_MAX_X)
						ply->pos.y = PLY_UP_MAX_X;
				}

				// Precompute screen position
				ply->srcPos.x = UNIT_TO_PX(ply->pos.x);
				ply->srcPos.y = UNIT_TO_PX(ply->pos.y);
				
				// Set movement action
				if((dir == 3) || (dir == 4)) // Front move
					SetAction(ply, ACTION_WALK_F);
				else if(dir < 3) // Left move
					SetAction(ply, ACTION_WALK_L);
				else // Right move
					SetAction(ply, ACTION_WALK_R);				
			}
		}
	}
}

void PreparePlayer1()
{
	// Flicker Shadows
	u8 pat = 0;
	if(g_FlickerShadow && (g_Frame & 0x1))
	{
		pat += 4;
	}
	VDP_SetSpritePattern(SPRITE_PLY1_BLACK_H, pat);      // Pattern #0  / #4
	VDP_SetSpritePattern(SPRITE_PLY1_BLACK_L, pat + 20); // Pattern #20 / #24
}

///
void UpdatePlayer1()
{	
	// Update sprite position
	u8 x = g_Player[0].srcPos.x - 8;
	u8 y = g_Player[0].srcPos.y - 24;
	VDP_SetSpritePosition(SPRITE_PLY1_BLACK_H, x, y);
	VDP_SetSpritePosition(SPRITE_PLY1_WHITE_H, x, y);
	VDP_SetSpritePosition(SPRITE_PLY1_SKIN_H, x, y);
	y += 8;
	VDP_SetSpritePosition(SPRITE_PLY1_CLOTH, x, y);
	y += 8;
	VDP_SetSpritePosition(SPRITE_PLY1_BLACK_L, x, y);
	VDP_SetSpritePosition(SPRITE_PLY1_WHITE_L, x, y);
	VDP_SetSpritePosition(SPRITE_PLY1_SKIN_L, x, y);

	// Handle special event
	u8 event = g_Actions[g_Player[0].action].animFrames[g_Player[0].step].event;
	switch(event)
	{
	case EVENT_SHOOT_R:
		CheckShoot(&g_Player[0]);
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, x+16, y-16);
		break;
	case EVENT_SHOOT_L:
		CheckShoot(&g_Player[0]);
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, x-8, y-16);
		break;
	case EVENT_SMASH:
		CheckShoot(&g_Player[0]);
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, x+8, y-32);
		break;
	default:
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, 213, 213);
		break;
	};
}

///
void DrawPlayer1()
{
	u8 frame = g_Player[0].anim;
	if(g_Player[0].prevAnim[g_WriteBuffer] == frame)
		return;

	g_Player[0].prevAnim[g_WriteBuffer] = frame;

	const u8* src;
	u16 dst;
		
	src = g_DataPlayer1 + (frame * 224);
	dst = g_SpritePattern[g_WriteBuffer] + (0 * 8); // Pattern #0 - 20
	VDP_WriteVRAM_16K(src, dst, 21 * 8);
	
	src += 21 * 8;
	dst += (22 * 8); // Pattern #22
	VDP_WriteVRAM_16K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #24
	VDP_WriteVRAM_16K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #26
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #28
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #30
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #32
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #34
	VDP_WriteVRAM_16K(src, dst, 8);

	u8 event = g_Actions[g_Player[0].action].animFrames[g_Player[0].step].event;
	if(event != EVENT_NONE)
	{
		switch(event)
		{
		case EVENT_SHOOT_R:
			src = g_DataRacket + (6 * 8);
			break;
		case EVENT_SHOOT_L:
			src = g_DataRacket + (14 * 8);
			break;
		case EVENT_SMASH:
			src = g_DataRacket + (2 * 8);
			break;
		};
		dst += (3 * 8);  // Pattern #37
		VDP_WriteVRAM_16K(src, dst, 8);	
	}
}


///
void PreparePlayer2()
{
	// Flicker Shadows
	u8 pat = 40;
	if(g_FlickerShadow && (g_Frame & 0x1))
	{
		pat += 4;
	}
	VDP_SetSpritePattern(SPRITE_PLY2_BLACK_H, pat);      // Pattern #40 / #44
	VDP_SetSpritePattern(SPRITE_PLY2_BLACK_L, pat + 16); // Pattern #56 / #60
}

///
void UpdatePlayer2()
{	
	// Update sprite position
	u8 x = g_Player[1].srcPos.x - 8;
	u8 y = g_Player[1].srcPos.y - 32;
	VDP_SetSpritePosition(SPRITE_PLY2_BLACK_H, x, y);
	VDP_SetSpritePosition(SPRITE_PLY2_WHITE_H, x, y);
	VDP_SetSpritePosition(SPRITE_PLY2_SKIN_H, x, y);
	y += 8;
	VDP_SetSpritePosition(SPRITE_PLY2_CLOTH, x, y);
	y += 8;
	VDP_SetSpritePosition(SPRITE_PLY2_BLACK_L, x, y);
	VDP_SetSpritePosition(SPRITE_PLY2_WHITE_L, x, y);
	VDP_SetSpritePosition(SPRITE_PLY2_SKIN_L, x, y);

	// Handle special event
	u8 event = g_Actions[g_Player[1].action].animFrames[g_Player[1].step].event;
	switch(event)
	{
	case EVENT_SHOOT_R:
		CheckShoot(&g_Player[1]);
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, x+16, y-8);
		break;                                    
	case EVENT_SHOOT_L:                           
		CheckShoot(&g_Player[1]);
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, x-8, y-8);
		break;                                    
	case EVENT_SMASH:                             
		CheckShoot(&g_Player[1]);
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, x+8, y-24);
		break;                                    
	default:                                      
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, 213, 213);
		break;
	};
}

///
void DrawPlayer2() __FASTCALL
{
	u8 frame = g_Player[1].anim;
	if(g_Player[1].prevAnim[g_WriteBuffer] == frame)
		return;

	g_Player[1].prevAnim[g_WriteBuffer] = frame;

	const u8* src;
	u16 dst;
		
	src = g_DataPlayer2 + (frame * 224);
	dst = g_SpritePattern[g_WriteBuffer] + (41 * 8); // Pattern #41
	VDP_WriteVRAM_16K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #43
	VDP_WriteVRAM_16K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #45
	VDP_WriteVRAM_16K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #47
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #49
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #51
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #53
	VDP_WriteVRAM_16K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #55 - #75
	VDP_WriteVRAM_16K(src, dst, 21 * 8);

	u8 event = g_Actions[g_Player[1].action].animFrames[g_Player[1].step].event;
	if(event != EVENT_NONE)
	{
		switch(event)
		{
		case EVENT_SHOOT_R:
			src = g_DataRacket + (6 * 8);
			break;
		case EVENT_SHOOT_L:
			src = g_DataRacket + (14 * 8);
			break;
		case EVENT_SMASH:
			src = g_DataRacket + (2 * 8);
			break;
		};
		dst += (22 * 8);  // Pattern #77
		VDP_WriteVRAM_16K(src, dst, 8);	
	}
}

///
void PrepareBall()
{
	// Flicker Shadows
	u8 pat = 80;
	if(g_FlickerShadow && (g_Frame & 0x1))
	{
		pat += 4;
	}
	VDP_SetSpritePattern(SPRITE_BALL_OUTLINE, pat);
}

///
void UpdateBall()
{
	// Horizontal force
	i16 dx = g_Ball.velXY * (i16)g_Cosinus64[g_Ball.dir];
	dx /= 64;
	g_Ball.pos.x += dx;
	
	i16 prevY = g_Ball.pos.y;
	i16 dy = g_Ball.velXY * (i16)g_Sinus64[g_Ball.dir];
	dy /= 64;
	g_Ball.pos.y -= dy;
	g_Ball.srcPos.x = UNIT_TO_PX(g_Ball.pos.x);
	g_Ball.srcPos.y = UNIT_TO_PX(g_Ball.pos.y);
	
	// Vertical force - Gravity
	g_Ball.velZ -= GRAVITY;	
	g_Ball.height += g_Ball.velZ;

	// Bounce
	if(g_Ball.height < 0)
	{
		g_Ball.height = 0;
		g_Ball.bounce++;
		g_Ball.velZ = -g_Ball.velZ * 2 / 3;
		if(g_Ball.bounce == 1)
		{
			// g_Ball.velXY = g_Ball.velXY * 3 / 4;
			ayFX_PlayBank(12, 0);
		}
		else if(g_Ball.bounce == 2)
		{
			ayFX_PlayBank(12, 0);
			// Check point
		}
		else if(g_Ball.bounce == 5)
		{
			ShootBallRandom();
		}
	}
	
	if(g_Ball.coolDown > 0)
		g_Ball.coolDown--;
	
	// Check out of screen
	if((g_Ball.pos.y < 0) || (g_Ball.pos.y > PX_TO_UNIT(192)) || (g_Ball.pos.x < 0) || (g_Ball.pos.x > PX_TO_UNIT(256)))
	{
		ShootBallRandom();
	}
	// Check net collision
	else if(g_Ball.height < PX_TO_UNIT(14))
	{
		if(((prevY < PX_TO_UNIT(96)) && (g_Ball.pos.y >= PX_TO_UNIT(96))) || (prevY > PX_TO_UNIT(96)) && (g_Ball.pos.y <= PX_TO_UNIT(96)))
		{
			g_Ball.velXY = 0;			
			if(g_Ball.velZ > 0)
				g_Ball.velZ = 0;
		}
	}

	u8  x = g_Ball.srcPos.x - 3;
	i16 y = g_Ball.srcPos.y - 3;
	
	VDP_SetSpritePosition(SPRITE_BALL_SHADOW,  x, y);	

	y -= UNIT_TO_PX(g_Ball.height);
	if(y < 0)
	{
		VDP_SetSpritePosition(SPRITE_BALL_OUTLINE, 213, 213);
		VDP_SetSpritePosition(SPRITE_BALL_BODY,    213, 213);
	}
	else
	{
		VDP_SetSpritePosition(SPRITE_BALL_OUTLINE, x, y);
		VDP_SetSpritePosition(SPRITE_BALL_BODY,    x, y);
	}
}

///
void DrawBall()
{	
	const u8* src = g_DataBall + (1 * 3 * 8);
	u16 dst = g_SpritePattern[g_WriteBuffer] + (80 * 8); // Pattern #80
	VDP_WriteVRAM_16K(src, dst, 8);
	
	src += (1 * 8);
	dst += (4 * 8); // Pattern #84
	VDP_WriteVRAM_16K(src, dst, 8);

	src += (1 * 8);
	dst += (4 * 8); // Pattern #88
	VDP_WriteVRAM_16K(src, dst, 8);
}

//-----------------------------------------------------------------------------
// INPUT CALLBACK

bool KB1_Button1()	{ return (IS_KEY_PRESSED(g_KeyRow6, KEY_SHIFT) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_SHIFT))); }	
bool KB1_Button2()	{ return (IS_KEY_PRESSED(g_KeyRow6, KEY_CTRL) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_CTRL))); }
bool KB1_Up()		{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_UP)); }
bool KB1_Down()		{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_DOWN)); }
bool KB1_Left()		{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_LEFT)); }
bool KB1_Right()	{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_RIGHT)); }

bool KB2_Button1()	{ return (IS_KEY_PRESSED(g_KeyRow6, KEY_SHIFT) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_SHIFT))); }	
bool KB2_Button2()	{ return (IS_KEY_PRESSED(g_KeyRow6, KEY_CTRL) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_CTRL))); }
bool KB2_Up()		{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_UP)); }
bool KB2_Down()		{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_DOWN)); }
bool KB2_Left()		{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_LEFT)); }
bool KB2_Right()	{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_RIGHT)); }

bool Joy1_Button1()	{ return (IS_JOY_PRESSED(g_Joy1, JOY_INPUT_TRIGGER_A) && !(IS_JOY_PRESSED(g_PrevJoy1, JOY_INPUT_TRIGGER_A))); }	
bool Joy1_Button2()	{ return (IS_JOY_PRESSED(g_Joy1, JOY_INPUT_TRIGGER_B) && !(IS_JOY_PRESSED(g_PrevJoy1, JOY_INPUT_TRIGGER_B))); }
bool Joy1_Up()		{ return (IS_JOY_PRESSED(g_Joy1, JOY_INPUT_DIR_UP)); }
bool Joy1_Down()	{ return (IS_JOY_PRESSED(g_Joy1, JOY_INPUT_DIR_DOWN)); }
bool Joy1_Left()	{ return (IS_JOY_PRESSED(g_Joy1, JOY_INPUT_DIR_LEFT)); }
bool Joy1_Right()	{ return (IS_JOY_PRESSED(g_Joy1, JOY_INPUT_DIR_RIGHT)); }

bool Joy2_Button1()	{ return (IS_JOY_PRESSED(g_Joy2, JOY_INPUT_TRIGGER_A) && !(IS_JOY_PRESSED(g_PrevJoy2, JOY_INPUT_TRIGGER_A))); }	
bool Joy2_Button2()	{ return (IS_JOY_PRESSED(g_Joy2, JOY_INPUT_TRIGGER_B) && !(IS_JOY_PRESSED(g_PrevJoy2, JOY_INPUT_TRIGGER_B))); }
bool Joy2_Up()		{ return (IS_JOY_PRESSED(g_Joy2, JOY_INPUT_DIR_UP)); }
bool Joy2_Down()	{ return (IS_JOY_PRESSED(g_Joy2, JOY_INPUT_DIR_DOWN)); }
bool Joy2_Left()	{ return (IS_JOY_PRESSED(g_Joy2, JOY_INPUT_DIR_LEFT)); }
bool Joy2_Right()	{ return (IS_JOY_PRESSED(g_Joy2, JOY_INPUT_DIR_RIGHT)); }

//=============================================================================
//
//   M A I N
//
//=============================================================================

extern State* g_NextState;

void VDP_InterruptHandler()
{
	Game_VSyncHook();
}

//-----------------------------------------------------------------------------
/// Main loop
void VSyncCallback()
{
	PT3_Decode();
	ayFX_Update();
	PT3_UpdatePSG();
}

//-----------------------------------------------------------------------------
/// Main loop
void main()
{
	// INIT PT3
	PT3_Init();
	PT3_SetNoteTable(PT3_NT2);
	PT3_SetLoop(true);
	PT3_InitSong(g_intro);

	// INIT ayFX
	ayFX_InitBank(g_ayfx_bank);
	ayFX_SetChannel(PSG_CHANNEL_A);
	ayFX_SetMode(AYFX_MODE_FIXED);

	Game_Initialize();
	Game_SetVSyncCallback(VSyncCallback);
	Game_SetState(&g_State_Title);

	while(1)
		Game_Update();
}

//-----------------------------------------------------------------------------
// GAME STATE

//-----------------------------------------------------------------------------
///
void StateTitle_Start()
{
	// Initialize VDP
	VDP_SetMode(VDP_MODE_GRAPHIC2);
	VDP_SetColor(COLOR_BLACK);
	VDP_EnableVBlank(true);
	VDP_EnableDisplay(false);

	// Set screen tables
	VDP_SetLayoutTable(VRAM_NAME_TABLE);
	VDP_SetColorTable(VRAM_COLOR_TABLE);
	VDP_SetPatternTable(VRAM_PATERN_TABLE);

	// Load screen data
	VDP_FillVRAM_16K(0, VRAM_NAME_TABLE, sizeof(g_DataLogo_Names)); // Don't set the Layout table yet
	VDP_WriteVRAM_16K(g_DataLogo_Patterns, VRAM_PATERN_TABLE + (0 * 0x800), sizeof(g_DataLogo_Patterns));
	VDP_WriteVRAM_16K(g_DataLogo_Patterns, VRAM_PATERN_TABLE + (1 * 0x800), sizeof(g_DataLogo_Patterns));
	VDP_WriteVRAM_16K(g_DataLogo_Patterns, VRAM_PATERN_TABLE + (2 * 0x800), sizeof(g_DataLogo_Patterns));
	VDP_WriteVRAM_16K(g_DataLogo_Colors,   VRAM_COLOR_TABLE + (0 * 0x800),  sizeof(g_DataLogo_Colors));
	VDP_WriteVRAM_16K(g_DataLogo_Colors,   VRAM_COLOR_TABLE + (1 * 0x800),  sizeof(g_DataLogo_Colors));
	VDP_WriteVRAM_16K(g_DataLogo_Colors,   VRAM_COLOR_TABLE + (2 * 0x800),  sizeof(g_DataLogo_Colors));

	// Set sprites tables
	VDP_SetSpritePatternTable(VRAM_SPRITE_PATTERN_0);
	VDP_SetSpriteAttributeTable(VRAM_SPRITE_ATTRIBUTE_0);

	// Load sprites data
	VDP_WriteVRAM_16K(g_DataLogoBall, VRAM_SPRITE_PATTERN_0, 8 * 32);

	// Initialize sprite attributes
	VDP_SetSpriteSM1(0, 0, 0, 0,  VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	VDP_SetSpriteSM1(1, 0, 0, 4,  VDP_SPRITE_EC | COLOR_DARK_RED);
	VDP_SetSpriteSM1(2, 0, 0, 8,  VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	VDP_SetSpriteSM1(3, 0, 0, 12, VDP_SPRITE_EC | COLOR_DARK_RED);
	VDP_SetSpriteSM1(4, 0, 0, 16, VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	VDP_SetSpriteSM1(5, 0, 0, 20, VDP_SPRITE_EC | COLOR_DARK_RED);
	VDP_SetSpriteSM1(6, 0, 0, 24, VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	VDP_SetSpriteSM1(7, 0, 0, 28, VDP_SPRITE_EC | COLOR_DARK_RED);
	VDP_SetSpritePositionY(8, VDP_SPRITE_DISABLE_SM1);

	VDP_EnableDisplay(true);
	
	g_IntroFrame = 0;
}

//-----------------------------------------------------------------------------
///
void StateTitle_Update()
{
	if(g_IntroFrame < 26) // Ball movement
	{
		VDP_SetSpritePosition(0, g_IntroFrame * 8 +  0 - 4, 40 +  0);
		VDP_SetSpritePosition(1, g_IntroFrame * 8 +  0 - 4, 40 +  0);
		VDP_SetSpritePosition(2, g_IntroFrame * 8 + 16 - 4, 40 +  0);
		VDP_SetSpritePosition(3, g_IntroFrame * 8 + 16 - 4, 40 +  0);
		VDP_SetSpritePosition(4, g_IntroFrame * 8 +  0 - 4, 40 + 16);
		VDP_SetSpritePosition(5, g_IntroFrame * 8 +  0 - 4, 40 + 16);
		VDP_SetSpritePosition(6, g_IntroFrame * 8 + 16 - 4, 40 + 16);
		VDP_SetSpritePosition(7, g_IntroFrame * 8 + 16 - 4, 40 + 16);
	}
	else if(g_IntroFrame == 26) // White flash
	{	
		ayFX_PlayBank(13, 0);
		VDP_SetColor(COLOR_WHITE);
		VDP_FillVRAM_16K(0xFF, VRAM_COLOR_TABLE + (0 * 0x800),  8);
		VDP_FillVRAM_16K(0xFF, VRAM_COLOR_TABLE + (1 * 0x800),  8);
		VDP_FillVRAM_16K(0xFF, VRAM_COLOR_TABLE + (2 * 0x800),  8);
	}
	else if(g_IntroFrame == 30) // Title
	{	
		VDP_SetColor(COLOR_BLACK);
		VDP_WriteVRAM_16K(g_DataLogo_Colors, VRAM_COLOR_TABLE + (0 * 0x800),  8);
		VDP_WriteVRAM_16K(g_DataLogo_Colors, VRAM_COLOR_TABLE + (1 * 0x800),  8);
		VDP_WriteVRAM_16K(g_DataLogo_Colors, VRAM_COLOR_TABLE + (2 * 0x800),  8);
		VDP_WriteVRAM_16K(g_DataLogo_Names, VRAM_NAME_TABLE, sizeof(g_DataLogo_Names));
	}
	else if(g_IntroFrame == 64) // Title
	{
		PT3_Resume();
	}
	if(g_IntroFrame < 255)
		g_IntroFrame++;
	
	if(Keyboard_IsKeyPressed(KEY_SPACE))
	{
		Game_SetState(&g_State_Game);
	}
}

//-----------------------------------------------------------------------------
///
void StateMenu_Start() {}

//-----------------------------------------------------------------------------
///
void StateMenu_Update() {}

//-----------------------------------------------------------------------------
///
void StateGame_Start()
{
	// Initialize VDP
	VDP_SetMode(VDP_MODE_GRAPHIC2);
	VDP_SetColor(COLOR_DARK_RED);
	VDP_EnableDisplay(false);
	VDP_SetLayoutTable(VRAM_NAME_TABLE);
	VDP_SetColorTable(VRAM_COLOR_TABLE);
	VDP_SetPatternTable(VRAM_PATERN_TABLE);
	VDP_EnableVBlank(true);

	// Load court data to VRAM
	VDP_WriteVRAM_16K(g_DataCourt_Names,    VRAM_NAME_TABLE,                 sizeof(g_DataCourt_Names));
	VDP_WriteVRAM_16K(g_DataCourt_Patterns, VRAM_PATERN_TABLE + (0 * 0x800), sizeof(g_DataCourt_Patterns));
	VDP_WriteVRAM_16K(g_DataCourt_Patterns, VRAM_PATERN_TABLE + (1 * 0x800), sizeof(g_DataCourt_Patterns));
	VDP_WriteVRAM_16K(g_DataCourt_Patterns, VRAM_PATERN_TABLE + (2 * 0x800), sizeof(g_DataCourt_Patterns));
	VDP_WriteVRAM_16K(g_DataCourt_Colors,   VRAM_COLOR_TABLE + (0 * 0x800),  sizeof(g_DataCourt_Colors));
	VDP_WriteVRAM_16K(g_DataCourt_Colors,   VRAM_COLOR_TABLE + (1 * 0x800),  sizeof(g_DataCourt_Colors));
	VDP_WriteVRAM_16K(g_DataCourt_Colors,   VRAM_COLOR_TABLE + (2 * 0x800),  sizeof(g_DataCourt_Colors));
	
	// Initialize sprites
	VDP_SetSpritePatternTable(VRAM_SPRITE_PATTERN_0);
	VDP_SetSpriteAttributeTable(VRAM_SPRITE_ATTRIBUTE_0);
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);
	VDP_FillVRAM(0x00, VRAM_SPRITE_PATTERN_0, 0, 128*8); // Clear sprite patterns table (only 128 entry used)
	VDP_FillVRAM(0x00, VRAM_SPRITE_PATTERN_1, 0, 128*8);

	// Setup player 1 sprites
	VDP_SetSpriteSM1(SPRITE_PLY1_BLACK_H, 0, 193, 0,  COLOR_BLACK);			// Outline
	VDP_SetSpriteSM1(SPRITE_PLY1_BLACK_L, 0, 193, 20, COLOR_BLACK);			// Outline
	VDP_SetSpriteSM1(SPRITE_PLY1_RACKET,  0, 193, 36, COLOR_BLACK);			// Racket
	VDP_SetSpriteSM1(SPRITE_PLY1_CLOTH,   0, 193, 8,  COLOR_LIGHT_BLUE);		// Cloth
	VDP_SetSpriteSM1(SPRITE_PLY1_WHITE_H, 0, 193, 12, COLOR_WHITE);			// White
	VDP_SetSpriteSM1(SPRITE_PLY1_WHITE_L, 0, 193, 28, COLOR_WHITE);			// White
	VDP_SetSpriteSM1(SPRITE_PLY1_SKIN_H,  0, 193, 16, COLOR_LIGHT_RED);		// Skin
	VDP_SetSpriteSM1(SPRITE_PLY1_SKIN_L,  0, 193, 32, COLOR_LIGHT_RED);		// Skin

	// Setup player 2 sprites
	VDP_SetSpriteSM1(SPRITE_PLY2_BLACK_H, 0, 193, 40, COLOR_BLACK);			// Outline
	VDP_SetSpriteSM1(SPRITE_PLY2_BLACK_L, 0, 193, 56, COLOR_BLACK);			// Outline
	VDP_SetSpriteSM1(SPRITE_PLY2_RACKET,  0, 193, 76, COLOR_BLACK);			// Racket
	VDP_SetSpriteSM1(SPRITE_PLY2_CLOTH,   0, 193, 64, COLOR_MEDIUM_GREEN);	// Cloth
	VDP_SetSpriteSM1(SPRITE_PLY2_WHITE_H, 0, 193, 48, COLOR_WHITE);			// White
	VDP_SetSpriteSM1(SPRITE_PLY2_WHITE_L, 0, 193, 68, COLOR_WHITE);			// White
	VDP_SetSpriteSM1(SPRITE_PLY2_SKIN_H,  0, 193, 52, COLOR_LIGHT_RED);		// Skin
	VDP_SetSpriteSM1(SPRITE_PLY2_SKIN_L,  0, 193, 72, COLOR_LIGHT_RED);		// Skin

	// Setup ball sprites
	VDP_SetSpriteSM1(SPRITE_BALL_OUTLINE, 0, 193, 80, COLOR_BLACK);			// Outline
	VDP_SetSpriteSM1(SPRITE_BALL_BODY,    0, 193, 88, COLOR_LIGHT_YELLOW);	// Body
	VDP_SetSpriteSM1(SPRITE_BALL_SHADOW,  0, 193, 88, COLOR_BLACK);			// Shadow

	// Setup net sprites
	VDP_SetSpriteSM1(SPRITE_NET_POST_W, 0, 193, 112, COLOR_WHITE);			// Net post 1
	VDP_SetSpriteSM1(SPRITE_NET_POST_G, 0, 193, 116, COLOR_GRAY);			// Net post 2
	VDP_SetSpriteSM1(SPRITE_NET_LEFT,   0, 193, 120, COLOR_WHITE);			// Net 1
	VDP_SetSpriteSM1(SPRITE_NET_RIGHT,  0, 193, 124, COLOR_WHITE);			// Net 2

	VDP_SetSpritePositionY(SPRITE_MAX, VDP_SPRITE_DISABLE_SM1);

	// VDP_EnableVBlank(true);
	// Bios_SetHookCallback(H_TIMI, VBlankHook);

	VDP_EnableDisplay(true);

	// Initialize player 1
	g_Player[0].id = 0;
	g_Player[0].pos.x = PX_TO_UNIT(100);
	g_Player[0].pos.y = PX_TO_UNIT(130);
	g_Player[0].srcPos.x = UNIT_TO_PX(g_Player[0].pos.x);
	g_Player[0].srcPos.y = UNIT_TO_PX(g_Player[0].pos.y);
	g_Player[0].action = ACTION_IDLE;
	g_Player[0].counter = 0;
	g_Player[0].anim = 0;
	g_Player[0].prevAnim[0] = 0xFF;
	g_Player[0].prevAnim[1] = 0xFF;
	g_Player[0].step = 0;
	g_Player[0].inButton1 = KB1_Button1;
	g_Player[0].inButton2 = KB1_Button2;
	g_Player[0].inUp      = KB1_Up;
	g_Player[0].inDown    = KB1_Down;
	g_Player[0].inLeft    = KB1_Left;
	g_Player[0].inRight   = KB1_Right;

	// Initialize player 2
	g_Player[1].id = 1;
	g_Player[1].pos.x = PX_TO_UNIT(150);
	g_Player[1].pos.y = PX_TO_UNIT(32);
	g_Player[1].srcPos.x = UNIT_TO_PX(g_Player[1].pos.x);
	g_Player[1].srcPos.y = UNIT_TO_PX(g_Player[1].pos.y);
	g_Player[1].action = ACTION_IDLE;
	g_Player[1].counter = 0;
	g_Player[1].anim = 0;
	g_Player[1].prevAnim[0] = 0xFF;
	g_Player[1].prevAnim[1] = 0xFF;
	g_Player[1].step = 0;
	g_Player[1].inButton1 = Joy2_Button1;
	g_Player[1].inButton2 = Joy2_Button2;
	g_Player[1].inUp      = Joy2_Up;
	g_Player[1].inDown    = Joy2_Down;
	g_Player[1].inLeft    = Joy2_Left;
	g_Player[1].inRight   = Joy2_Right;

	PT3_Pause();

	// Initialize ball
	ShootBallRandom();
	
	// Game_Initialize();
}

//-----------------------------------------------------------------------------
///
void StateGame_Update()
{
// VDP_SetColor(COLOR_BLACK);
	
	//---------------------------------------------------------------------
	// Switch buffer
	g_WriteBuffer = 1 - g_WriteBuffer;
	g_DisplayBuffer = 1 - g_DisplayBuffer;
	VDP_SetSpritePatternTable(g_SpritePattern[g_DisplayBuffer]);
	//VDP_SetSpriteAttributeTable(g_SpriteAttrib[g_DisplayBuffer]);
	PreparePlayer1();
	PreparePlayer2();
	PrepareBall();

// VDP_SetColor(COLOR_LIGHT_BLUE);

	//---------------------------------------------------------------------
	// Update input

	// Keyboard
	g_PrevRow6 = g_KeyRow6;	
	g_PrevRow8 = g_KeyRow8;	
	g_KeyRow6 = Keyboard_Read(6); // SHIFT CTRL GRAPH CAPS CODE F1 F2 F3
	g_KeyRow8 = Keyboard_Read(8); // SPACE HOME INS DEL LEFT UP DOWN RIGHT
	// Joystick
	g_PrevJoy1 = g_Joy1;
	g_PrevJoy2 = g_Joy2;
	g_Joy1 = Joystick_Read(JOY_PORT_1);
	g_Joy2 = Joystick_Read(JOY_PORT_2);


	if(IS_KEY_PRESSED(g_KeyRow6, KEY_F2) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_F2)))
		ShootBallRandom();
	if(IS_KEY_PRESSED(g_KeyRow6, KEY_F3) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_F3)))
		g_FlickerShadow = 1 - g_FlickerShadow;
	// Return to main menu
	if(IS_KEY_PRESSED(g_KeyRow8, KEY_DEL))
	{
		VDP_SetSpritePositionY(8, VDP_SPRITE_DISABLE_SM1);
		Game_SetState(&g_State_Title);
	}
	
	UpdateInput(&g_Player[0]);
	UpdateInput(&g_Player[1]);

// VDP_SetColor(COLOR_LIGHT_GREEN);
	
	//---------------------------------------------------------------------
	// Update sprites position
	
	UpdatePlayer1();
	UpdatePlayer2();
	UpdateBall();

	UpdateAction(&g_Player[0]);
	UpdateAction(&g_Player[1]);

// VDP_SetColor(COLOR_LIGHT_YELLOW);

	//---------------------------------------------------------------------
	// Draw anim
	
	DrawPlayer1();
	DrawPlayer2();
	DrawBall();

// VDP_SetColor(COLOR_DARK_RED);
}

//-----------------------------------------------------------------------------
///
void StateTrain_Start() {}

//-----------------------------------------------------------------------------
///
void StateTrain_Update() {}