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
#include "string.h"
#include "game.h"
#include "game_menu.h"
#include "print.h"
#include "pt3/pt3_player.h"
#include "ayfx/ayfx_player.h"

//=============================================================================
//
//   D E F I N E S
//
//=============================================================================

//-----------------------------------------------------------------------------
// DEFINES
#define VERSION						"V0.16.0"
#define DEBUG						1

// VRAM Tables Address - MSX1
#define MSX1_LAYOUT_TABLE			0x3C00
#define MSX1_COLOR_TABLE			0x2000
#define MSX1_PATTERN_TABLE			0x0000
#define MSX1_SPRITE_ATTRIBUTE		0x3F00
#define MSX1_SPRITE_PATTERN_0		0x1800
#define MSX1_SPRITE_PATTERN_1		0x3800

// VRAM Tables Address - MSX2/2+/tR
#define MSX2_LAYOUT_TABLE			0x1800
#define MSX2_COLOR_TABLE			0x2000
#define MSX2_PATTERN_TABLE			0x0000
#define MSX2_SPRITE_ATTRIBUTE		0x1E00
#define MSX2_SPRITE_PATTERN_0		0x3800
#define MSX2_SPRITE_PATTERN_1		0x3800

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

// Menu layout
#define MENU_ITEMS_X				12
#define MENU_ITEMS_Y				14
#define MENU_ITEMS_H				(24-MENU_ITEMS_Y)

#if (DEBUG)
	#define DEBUG_CODE(code) code
#else
	#define DEBUG_CODE(code)
#endif

enum PLAYER_ID
{
	PLY1 = 0, 		// Bottom
	PLY2,     		// Top
	PLAYER_NUM,
};

enum SPIN
{
	SPIN_TOP = 0,
	SPIN_FLAT,
	SPIN_BACK,
	SPIN_MAX,
};

enum SHOT_TYPE
{
	SHOT_FLAT = 0,	// Flat shot
	SHOT_SLICE,		// Drop shot / lob
	SHOT_ATTACK,	// Top spin shot
	SHOT_MAX,
};

/// 
enum TRAIN
{
	TRAIN_BOTH = 0,
	TRAIN_FLAT,
	TRAIN_LOB,
	TRAIN_MAX,
};

/// 
enum DIR
{
	DIR_CENTER = 0,
	DIR_RIGHT,
	DIR_LEFT,
	DIR_MAX,
};

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
	EVENT_PREPARE,
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

/// Menu pages enumeration
enum
{
	MENU_MAIN = 0,
	MENU_MATCH1P,
	MENU_MATCH2P,
	MENU_TRAINING,
	MENU_OPTIONS,
	MENU_CREDIT,
	MENU_MAX,
};

enum POINT_VALIDATION
{
	POINT_PENDING = 0,
	POINT_VALIDATED,
	POINT_FINISHED,
	POINT_MAX,
};

// enum RULES_EVENT
// {
	// EVENT_IN = 0,
	// EVENT_OUT,
// }	EVENT_NET,
// };


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
	u8			shot;
	u8			shotDir;
	u8			shotCnt;
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
	u8			point;
	u8			spin;
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

///
typedef struct
{
	Vector8		min;
	Vector8		max;
} Field;

///
typedef struct
{
	i16			velXY;  // Ball horizontal velocity (Q10.6 format)
	i16			velZ;   // Ball vertical velocity (Q10.6 format)
	u8			spin;
} Shot;

//-----------------------------------------------------------------------------
// FUNCTIONS PROTOTYPE

void StateTitle_Start();
void StateTitle_Update();

void StateMenu_Start();
void StateMenu_Update();

void StateMatch_Start();
void StateMatch_Update();

void StateTraining_Start();
void StateTraining_Update();

const char* Menu_StartMatch(u8 op, i8 value);
const char* Menu_StartTrain(u8 op, i8 value);
const char* Menu_SetAI(u8 op, i8 value);
const char* Menu_SetSets(u8 op, i8 value);
const char* Menu_SetShot(u8 op, i8 value);
const char* Menu_CreditScroll(u8 op, i8 value);

//_____________________________________________________________________________
//  ▄▄▄   ▄▄  ▄▄▄▄  ▄▄ 
//  ██ █ ██▄█  ██  ██▄█
//  ██▄▀ ██ █  ██  ██ █
//_____________________________________________________________________________

//=============================================================================
//
//  ROM DATA - PAGE 0
//
//=============================================================================

// Menu title (GM2 tables)
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATALOGO_NAMES		(0x0200)
	#define ADDR_DATALOGO_PATTERNS	(ADDR_DATALOGO_NAMES + sizeof(g_DataLogo_Names))
	#define ADDR_DATALOGO_COLORS	(ADDR_DATALOGO_PATTERNS + sizeof(g_DataLogo_Patterns))
	#define D_g_DataLogo_Names		__at(ADDR_DATALOGO_NAMES)
	#define D_g_DataLogo_Patterns	__at(ADDR_DATALOGO_PATTERNS)
	#define D_g_DataLogo_Colors		__at(ADDR_DATALOGO_COLORS)
#endif
#include "data_logo.h"

// Menu title ball
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATALOGOBALL		(ADDR_DATALOGO_COLORS + sizeof(g_DataLogo_Colors))
	#define D_g_DataLogoBall		__at(ADDR_DATALOGOBALL)
#endif
#include "data_logo_ball.h"

// Court background (GM2 tables)
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATACOURT_NAMES	(ADDR_DATALOGOBALL + sizeof(g_DataLogoBall))
	#define ADDR_DATACOURT_PATTERNS	(ADDR_DATACOURT_NAMES + sizeof(g_DataCourt_Names))
	#define ADDR_DATACOURT_COLORS	(ADDR_DATACOURT_PATTERNS + sizeof(g_DataCourt_Patterns))
	#define D_g_DataCourt_Names		__at(ADDR_DATACOURT_NAMES)
	#define D_g_DataCourt_Patterns  __at(ADDR_DATACOURT_PATTERNS)
	#define D_g_DataCourt_Colors    __at(ADDR_DATACOURT_COLORS)
#endif
#include "data_court.h"

// Player 1 sprites
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAPLAYER1		(ADDR_DATACOURT_COLORS + sizeof(g_DataCourt_Colors))
	#define D_g_DataPlayer1			__at(ADDR_DATAPLAYER1)
#endif
#include "data_player1.h"

// Ball launcher sprites
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATALAUNCHER		(ADDR_DATAPLAYER1 + sizeof(g_DataPlayer1))
	#define D_g_DataLauncher		__at(ADDR_DATALAUNCHER)
#endif
#include "data_launcher.h"


//=============================================================================
//
//  ROM DATA - PAGE 1-2
//
//=============================================================================

#include "data_board.h"
#include "data_referee.h"


// Player 2 Sprites
#include "data_player2.h" 
// Racket sprites
#include "data_racket.h"
// Score sprites
#include "data_score.h"
// Ball sprites
#include "data_ball.h"
// Fonts
#include "font/font_carwar.h"
// #include "font/font_cmsx_sys1.h"
// #include "font/font_gfx_future1.h"
// #include "font/font_gfx_tennis1.h"
// #include "font/font_gfx_tennis2.h"
#define FONT g_Font_Carwar	// g_Font_Carwar g_Font_CMSX_Sys1 g_Font_GFX_Future1 g_Font_GFX_Tennis1 g_Font_GFX_Tennis2


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
	{ 15,  4, EVENT_PREPARE },
	{ 16,  8, EVENT_SHOOT_R }, // do shoot
	{ 17, 16, EVENT_NONE },
};
const Anim g_FramesShootL[] = 
{
	{  9,  4, EVENT_PREPARE },
	{ 10,  8, EVENT_SHOOT_L }, // do shoot
	{ 11, 16, EVENT_NONE },
};
const Anim g_FramesSmash[] = 
{
	{ 12,  4, EVENT_PREPARE },
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

///
const Field g_Field[PLAYER_NUM] =
{
	{ { 55, 31 }, { 200, 95 } },	// Top field    (Player 1 target)
	{ { 55, 96 }, { 200, 160 } },	// Bottom field (Player 2 target)
};

/// Movement
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

const State g_State_Title = { StateTitle_Update, StateTitle_Start, null };
const State g_State_Menu  = { StateMenu_Update,  StateMenu_Start,  null };
const State g_State_Match = { StateMatch_Update, StateMatch_Start,  null };
const State g_State_Training = { StateTraining_Update, StateTraining_Start, null };

///
const u8 g_ColorShade[8] =
{ 
	COLOR_MERGE(9, 0), 
	COLOR_MERGE(9, 0), 
	COLOR_MERGE(8, 0), 
	COLOR_MERGE(8, 0), 
	COLOR_MERGE(8, 0), 
	COLOR_MERGE(6, 0), 
	COLOR_MERGE(6, 0), 
	COLOR_MERGE(6, 0)
};

// Menu 0 - Main
const MenuEntry g_MenuMain[] =
{
	{ "1P MATCH",	MENU_ITEM_GOTO|MENU_MATCH1P, 0, 0 },
	{ "2P MATCH",	MENU_ITEM_GOTO|MENU_MATCH2P, 0, 0 },
	{ "TRAINING",	MENU_ITEM_GOTO|MENU_TRAINING, 0, 0 },
	{ "OPTIONS", 	MENU_ITEM_GOTO|MENU_OPTIONS, 0, 0 },
	{ "CREDITS", 	MENU_ITEM_GOTO|MENU_CREDIT, 0, 0 },
	{ "",        	MENU_ITEM_DISABLE, 0, 0 },
	{ "",        	MENU_ITEM_DISABLE, 0, 0 },
	{ "",        	MENU_ITEM_DISABLE, 0, 0 },
	{ "",        	MENU_ITEM_DISABLE, 0, 0 },
	{ VERSION,   	MENU_ITEM_DISABLE, 0, 0 },
};

// Menu 1 - Match P1
const MenuEntry g_MenuMatchP1[] =
{
	{ "START>",		MENU_ITEM_ACTION, Menu_StartMatch, 0 },
	{ "SETS",		MENU_ITEM_ACTION, Menu_SetSets, 0 },
	{ "AI",			MENU_ITEM_ACTION, Menu_SetAI, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "<BACK",		MENU_ITEM_GOTO|MENU_MAIN, 0, 0 },
};

// Menu 2 - Match P2
const MenuEntry g_MenuMatchP2[] =
{
	{ "START>",		MENU_ITEM_ACTION, Menu_StartMatch, 0 },
	{ "SETS",		MENU_ITEM_ACTION, Menu_SetSets, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "<BACK",		MENU_ITEM_GOTO|MENU_MAIN, 0, 0 },
};

// Menu 3 - Training
const MenuEntry g_MenuTraining[] =
{
	{ "START>",		MENU_ITEM_ACTION, Menu_StartTrain, 0 },
	{ "SHOT",		MENU_ITEM_ACTION, Menu_SetShot, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "<BACK",		MENU_ITEM_GOTO|MENU_MAIN, 0, 0 },
};

// Menu 4 - Option
const MenuEntry g_MenuOption[] =
{
	{ "MUSIC",		MENU_ITEM_BOOL, &g_PlayMusic, 0 },
	{ "SFX",		MENU_ITEM_BOOL, &g_PlaySFX, 0 },
	{ "SHADE",		MENU_ITEM_BOOL, &g_FlickerShadow, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "",			MENU_ITEM_DISABLE, 0, 0 },
	{ "<BACK",		MENU_ITEM_GOTO|MENU_MAIN, 0, 0 },
};

// Menu 5 - Credits
const MenuEntry g_MenuCredits[] =
{
	{ "PIXEL PHENIX (C) 2021",     MENU_ITEM_TEXT, 0, (i8)-6 },
	{ "",                          MENU_ITEM_DISABLE, 0, 0 },
	{ "CODE:  AOINEKO",            MENU_ITEM_TEXT, 0, (i8)-6 },
	{ "GRAPH: AOINEKO&GFX",        MENU_ITEM_TEXT, 0, (i8)-6 },
	{ "MUSIC: ???",                MENU_ITEM_TEXT, 0, (i8)-6 },
	{ "SFX:   ???",                MENU_ITEM_TEXT, 0, (i8)-6 },
	{ "",                          MENU_ITEM_DISABLE, 0, 0 },
	{ "",                          MENU_ITEM_UPDATE, Menu_CreditScroll, (i8)-6 },
	{ "",                          MENU_ITEM_DISABLE, 0, 0 },
	{ "<BACK",                     MENU_ITEM_GOTO|MENU_MAIN, 0, 0 },
};

const Menu g_Menus[MENU_MAX] =
{
	{ "MAIN MENU",		g_MenuMain,			numberof(g_MenuMain) },
	{ "MATCH P1",		g_MenuMatchP1,		numberof(g_MenuMatchP1) },
	{ "MATCH P2",		g_MenuMatchP2,		numberof(g_MenuMatchP2) },
	{ "TRAINING",  		g_MenuTraining,		numberof(g_MenuTraining) },
	{ "OPTIONS",		g_MenuOption,		numberof(g_MenuOption) },
	{ "CREDITS",		g_MenuCredits,		numberof(g_MenuCredits) },
};

const Shot g_Shots[] =
{
	//	ZY				Z				Spin
	// Normal shot - Long
	{ PX_TO_UNIT(3.5), PX_TO_UNIT(3.0), SPIN_FLAT },
	// Normal shot - Med
	{ PX_TO_UNIT(3.0), PX_TO_UNIT(2.7), SPIN_FLAT },
	// Normal shot - Short
	{ PX_TO_UNIT(2.0), PX_TO_UNIT(3.0), SPIN_FLAT },
	// Normal shot - Smash
	{ PX_TO_UNIT(5.0), PX_TO_UNIT(-0.5), SPIN_FLAT },
	
	// High shot - Long
	{ PX_TO_UNIT(2.2), PX_TO_UNIT(5.0), SPIN_FLAT },	// Lob
	// High shot - Med
	{ PX_TO_UNIT(2.2), PX_TO_UNIT(4.0), SPIN_FLAT },
	// High shot - Short
	{ PX_TO_UNIT(1.7), PX_TO_UNIT(3.0), SPIN_BACK },	// Down shot
	// High shot - Smash
	{ PX_TO_UNIT(4.0), PX_TO_UNIT(0.0), SPIN_FLAT },
	
	// Low shot - Long
	{ PX_TO_UNIT(4.0), PX_TO_UNIT(1.9), SPIN_TOP },
	// Low shot - Med
	{ PX_TO_UNIT(3.0), PX_TO_UNIT(2.3), SPIN_TOP },
	// Low shot - Short
	{ PX_TO_UNIT(2.0), PX_TO_UNIT(2.7), SPIN_FLAT },
	// Low shot - Smash
	{ PX_TO_UNIT(6.0), PX_TO_UNIT(-0.5), SPIN_FLAT },
	
};

//=============================================================================
//
//  RAM DATA - PAGE 3
//
//=============================================================================

// Gameplay
Player 		g_Player[PLAYER_NUM];
Ball		g_Ball;
u8			g_Level = 1;			///< AI level (0=Easy, 1=Medium, 2=Hard)
u8			g_Sets = 1;			///< Sets count (0=1 set, 1=3 sets, 2=5 sets)
callback	g_ScoreFct = null;

// Configuration
u8			g_FlickerShadow = true;
u8			g_PlayMusic = true;
u8			g_PlaySFX = true;

// System
u8			g_VersionVDP;
u16			g_SpritePattern[2];
u8			g_WriteBuffer = 0;
u8			g_DisplayBuffer = 1;
u8			g_FontDefaultOffset;
u8			g_FontSelectOffset;

// Input
u8			g_PrevRow4;
u8			g_PrevRow6;
u8			g_PrevRow8;
u8			g_KeyRow4 = 0xFF; // K L M N O P Q R
u8			g_KeyRow6 = 0xFF; // SHIFT CTRL GRAPH CAPS CODE F1 F2 F3
u8			g_KeyRow8 = 0xFF; // SPACE HOME INS DEL LEFT UP DOWN RIGHT
u8			g_PrevJoy1;
u8			g_PrevJoy2;
u8			g_Joy1 = 0xFF;
u8			g_Joy2 = 0xFF;

// Menu
u8			g_IntroFrame = 0;
const Menu*	g_CurrentMenu; 
u8			g_MenuID = 0; 
u8			g_MenuItem = 0; 

// Training
u8			g_TrainScore;
u8			g_TrainBest = 0;
u8			g_TrainShot = TRAIN_BOTH;

#if (DEBUG)
	u8		g_Debug = 0;
#endif

//_____________________________________________________________________________
//   ▄▄   ▄▄  ▄▄▄  ▄▄▄▄
//  ██ ▀ ██ █ ██ █ ██▄ 
//  ▀█▄▀ ▀█▄▀ ██▄▀ ██▄▄
//_____________________________________________________________________________

//=============================================================================
//
//   M E N U
//
//=============================================================================

//-----------------------------------------------------------------------------
///
const c8* Menu_StartMatch(u8 op, i8 value)
{
	if(op == MENU_ACTION_SET)
		Game_SetState(&g_State_Match);
	return ""; 
}

//-----------------------------------------------------------------------------
///
const c8* Menu_StartTrain(u8 op, i8 value)
{
	if(op == MENU_ACTION_SET)
		Game_SetState(&g_State_Training);
	return "";
}

//-----------------------------------------------------------------------------
///
const char* Menu_SetAI(u8 op, i8 value)
{
	switch(op)
	{
	case MENU_ACTION_INC: g_Level = (g_Level + 1) % 3; break;
	case MENU_ACTION_DEC: g_Level = (g_Level + 2) % 3; break;	
	}

	if(g_Level == 0)
		return "EASY";
	else if(g_Level == 1)
		return "MED";

	return "HARD";
}

//-----------------------------------------------------------------------------
///
const char* Menu_SetSets(u8 op, i8 value)
{
	switch(op)
	{
	case MENU_ACTION_INC: g_Sets = (g_Sets + 1) % 3; break;
	case MENU_ACTION_DEC: g_Sets = (g_Sets + 2) % 3; break;	
	}

	if(g_Sets == 0)
		return "1";
	else if(g_Sets == 1)
		return "3";

	return "5";
}

//-----------------------------------------------------------------------------
///
const char* Menu_SetShot(u8 op, i8 value)
{
	switch(op)
	{
	case MENU_ACTION_INC: g_TrainShot = (g_TrainShot + 1) % TRAIN_MAX; break;
	case MENU_ACTION_DEC: g_TrainShot = (g_TrainShot + (TRAIN_MAX-1)) % TRAIN_MAX; break;	
	}
	
	switch(g_TrainShot)
	{
	case TRAIN_BOTH: return "BOTH";
	case TRAIN_FLAT: return "FLAT";
	case TRAIN_LOB:  return "LOB";
	};
}

#define SCROLL_BUF_SIZE 22
const c8* g_CrediScroll = "______________________DEDICATED_TO_MY_WONDERFUL_WIFE_AND_SON_<3_THANKS_TO_ALL_MSX-VILLAGE_AND_MRC_MEMBERS_FOR_SUPPORT!_MSX_NEVER_DIE._PRAISE_THE_HOLY-BRIOCHE!_\\O/";
c8 g_CrediScrollBug[SCROLL_BUF_SIZE];
u8 g_CrediScrollCnt = 0;

//-----------------------------------------------------------------------------
///
const char* Menu_CreditScroll(u8 op, i8 value)
{
	if(op == MENU_ACTION_GET)
	{	
		for(u8 i=0; i < SCROLL_BUF_SIZE-1; i++)
		{
			u8 j = (i + g_CrediScrollCnt) % String_Length(g_CrediScroll);
			g_CrediScrollBug[i] = g_CrediScroll[j];
		}
		g_CrediScrollBug[SCROLL_BUF_SIZE-1] = 0;
		g_CrediScrollCnt++;
	}
	return g_CrediScrollBug;
}


#pragma save
#pragma disable_warning	244		///< remove "pointer types incompatible" warning

//-----------------------------------------------------------------------------
///
void Menu_DisplayItem(u8 item) __FASTCALL
{
	// Clean buttom-third
	u16 dst = g_ScreenLayoutLow + ((MENU_ITEMS_Y + item) * 32);
	VDP_FillVRAM_64K(0, dst, 32);
	
	// Draw item
	if(item < g_CurrentMenu->itemNum)
	{
		MenuEntry* pCurEntry = &g_CurrentMenu->items[item];
		if(g_MenuItem == item)
			Print_SelectTextFont(g_FontSelectOffset);
		else
			Print_SelectTextFont(g_FontDefaultOffset);
			
		u8 x = MENU_ITEMS_X;
		u8 y = MENU_ITEMS_Y;
		
		if(pCurEntry->type == MENU_ITEM_TEXT)
			x += pCurEntry->value;
			
		Print_SetPosition(x, y + item);
		Print_DrawText(pCurEntry->text);

		Print_SetPosition(x + 9, y + item);

		if(pCurEntry->type == MENU_ITEM_ACTION)
		{
			menuCallback cb = (menuCallback)pCurEntry->action;
			const c8* str = cb(MENU_ACTION_GET, pCurEntry->value);
			if(*str)
			{
				if(g_MenuItem == item)
					Print_DrawChar('<');
				else
					Print_Space();
				Print_DrawText(str);
				if(g_MenuItem == item)
					Print_DrawChar('>');
			}
		}
		else if(pCurEntry->type == MENU_ITEM_INT)
		{
			i8* data = (u8*)pCurEntry->action;
			if(g_MenuItem == item)
				Print_DrawChar('<');
			else
				Print_Space();
			Print_DrawInt(*data);
			if(g_MenuItem == item)
				Print_DrawChar('>');
		}
		else if(pCurEntry->type == MENU_ITEM_BOOL)
		{
			u8* data = (u8*)pCurEntry->action;
			if(g_MenuItem == item)
				Print_DrawChar('<');
			else
				Print_Space();
			Print_DrawText(*data ? "TRUE" : "FALSE");
			if(g_MenuItem == item)
				Print_DrawChar('>');
		}
	}
}

//-----------------------------------------------------------------------------
///
void Menu_Initialize(const Menu* menu) __FASTCALL
{
	// Initialize menu
	g_CurrentMenu = menu;
	g_MenuItem = 0;
	while(g_MenuItem < g_CurrentMenu->itemNum)
	{
		if(g_CurrentMenu->items[g_MenuItem].type >= MENU_ITEM_ACTION)
			break;
		g_MenuItem++;
	}
	
	// Display menu items
	for(u8 item = 0; item < MENU_ITEMS_H; item++)
	{
		Menu_DisplayItem(item);
	}
}

//-----------------------------------------------------------------------------
///
void Menu_Update()
{
	// Handle activation
	MenuEntry* pCurEntry = &g_CurrentMenu->items[g_MenuItem];
	if((IS_KEY_PRESSED(g_KeyRow8, KEY_SPACE) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_SPACE))))
	{
		if(pCurEntry->type == MENU_ITEM_ACTION)
		{
			menuCallback cb = (menuCallback)pCurEntry->action;
			cb(MENU_ACTION_SET, pCurEntry->value);
			Menu_DisplayItem(g_MenuItem);
		}
		if((pCurEntry->type & MENU_ITEM_GOTO) != 0)
		{
			Menu_Initialize(&g_Menus[pCurEntry->type & 0x7F]);
		}
		return;
	}

	// Handle change value
	if((IS_KEY_PRESSED(g_KeyRow8, KEY_RIGHT) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_RIGHT))))
	{
		if(pCurEntry->type == MENU_ITEM_ACTION)
		{
			menuCallback cb = (menuCallback)pCurEntry->action;
			cb(MENU_ACTION_INC, pCurEntry->value);
		}
		else if(pCurEntry->type == MENU_ITEM_INT)
		{
			i8* data = (u8*)pCurEntry->action;
			*data++;
		}
		else if(pCurEntry->type == MENU_ITEM_BOOL)
		{
			u8* data = (u8*)pCurEntry->action;
			*data = 1 - *data;
		}
		Menu_DisplayItem(g_MenuItem);
	}
	else if((IS_KEY_PRESSED(g_KeyRow8, KEY_LEFT) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_LEFT))))
	{
		if(pCurEntry->type == MENU_ITEM_ACTION)
		{
			menuCallback cb = (menuCallback)pCurEntry->action;
			cb(MENU_ACTION_DEC, pCurEntry->value);
		}
		else if(pCurEntry->type == MENU_ITEM_INT)
		{
			i8* data = (u8*)pCurEntry->action;
			*data--;
		}
		else if(pCurEntry->type == MENU_ITEM_BOOL)
		{
			u8* data = (u8*)pCurEntry->action;
			*data = 1 - *data;
		}
		Menu_DisplayItem(g_MenuItem);
	}
	
	// Handle navigation
	if((IS_KEY_PRESSED(g_KeyRow8, KEY_UP) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_UP))))
	{
		u8 i = g_MenuItem;
		while(i > 0)
		{
			i--;
			if(g_CurrentMenu->items[i].type >= MENU_ITEM_ACTION)
			{
				u8 prev = g_MenuItem;
				g_MenuItem = i;
				Menu_DisplayItem(prev);
				Menu_DisplayItem(g_MenuItem);
				break;
			}
		}			
	}
	else if((IS_KEY_PRESSED(g_KeyRow8, KEY_DOWN) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_DOWN))))
	{
		u8 i = g_MenuItem;
		while(i < g_CurrentMenu->itemNum-1)
		{
			i++;
			if(g_CurrentMenu->items[i].type >= MENU_ITEM_ACTION)
			{
				u8 prev = g_MenuItem;
				g_MenuItem = i;
				Menu_DisplayItem(prev);
				Menu_DisplayItem(g_MenuItem);
				break;
			}
		}
	}
	
	// Update menu items
	if((g_Frame & 0x07) == 0) // 8th frame
	{
		Print_SelectTextFont(g_FontDefaultOffset);
		for(u8 item = 0; item < g_CurrentMenu->itemNum; item++)
		{
			MenuEntry* pCurEntry = &g_CurrentMenu->items[item];
			if(pCurEntry->type == MENU_ITEM_UPDATE)
			{
				menuCallback cb = (menuCallback)pCurEntry->action;
				const c8* str = cb(MENU_ACTION_GET, pCurEntry->value);
				u8 x = MENU_ITEMS_X;
				x += pCurEntry->value;			
				Print_SetPosition(x, MENU_ITEMS_Y + item);
				Print_DrawText(str);
			}
		}
	}
}

#pragma restore		///< restore "pointer types incompatible" warning


//=============================================================================
//
//   B A L L
//
//=============================================================================

//-----------------------------------------------------------------------------
///
void Ball_ShootRandom()
{
	// Position
	g_Ball.pos.x = PX_TO_UNIT(128);
	g_Ball.pos.y = PX_TO_UNIT(24);
	g_Ball.height = PX_TO_UNIT(10);

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
	if(((g_TrainShot == TRAIN_FLAT)) || ((g_TrainShot == TRAIN_BOTH) && (rnd & BIT_8))) // top spine*/
	{
		g_Ball.velXY = PX_TO_UNIT(3);
		g_Ball.velZ = PX_TO_UNIT(3);
		g_Ball.spin = SPIN_FLAT;
	}
	else // lob
	{
		g_Ball.velXY = PX_TO_UNIT(2);
		g_Ball.velZ = PX_TO_UNIT(5);
		g_Ball.spin = SPIN_FLAT;
	}

	// Misc
	g_Ball.lastPly = PLY2;
	g_Ball.coolDown = 0;
	g_Ball.bounce = 0;
	g_Ball.point = POINT_PENDING;
		
	ayFX_PlayBank(1, 0);
}

//-----------------------------------------------------------------------------
///
void Ball_Prepare()
{
	// Flicker Shadows
	u8 pat = 80;
	if(g_FlickerShadow && (g_Frame & 0x1))
	{
		pat += 4;
	}
	VDP_SetSpritePattern(SPRITE_BALL_OUTLINE, pat);
}

//-----------------------------------------------------------------------------
///
bool Ball_CheckField()
{
	const Field* field;
	if(g_Ball.lastPly == PLY1)
		field = &g_Field[PLY1];
	else
		field = &g_Field[PLY2];

	if(g_Ball.srcPos.x < field->min.x)
		return false;
	if(g_Ball.srcPos.x > field->max.x)
		return false;
	if(g_Ball.srcPos.y < field->min.y)
		return false;
	if(g_Ball.srcPos.y > field->max.y)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
///
void Ball_Score()
{
	if((g_Ball.point != POINT_FINISHED) && (g_ScoreFct != null))
	{
		g_ScoreFct();
		g_Ball.point = POINT_FINISHED;
	}
}

//-----------------------------------------------------------------------------
///
void Ball_Update()
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

		/*if(g_Ball.spin == SPIN_TOP)
		{
		}
		else*/ if(g_Ball.spin == SPIN_BACK)
		{
			g_Ball.velXY = g_Ball.velXY * 2 / 3;
		}
		else if(g_Ball.spin == SPIN_FLAT)
		{
			g_Ball.velXY = g_Ball.velXY * 3 / 4;
		}

		if(g_Ball.bounce == 1)
		{
			// g_Ball.velXY = g_Ball.velXY * 3 / 4;
			ayFX_PlayBank(12, 0);

			if(Ball_CheckField() && (g_Ball.point == POINT_PENDING))
				g_Ball.point = POINT_VALIDATED;
			else
				Ball_Score();
		}
		else if(g_Ball.bounce == 2)
		{
			Ball_Score();
			ayFX_PlayBank(12, 0);
			// Check point
		}
		else if(g_Ball.bounce == 5)
		{
			Ball_ShootRandom();
		}
	}

	if(g_Ball.coolDown > 0)
		g_Ball.coolDown--;

	// Check out of screen
	if((g_Ball.pos.y < 0) || (g_Ball.pos.y > PX_TO_UNIT(192)) || (g_Ball.pos.x < 0) || (g_Ball.pos.x > PX_TO_UNIT(256)))
	{
		Ball_Score();
		Ball_ShootRandom();
	}
	// Check net collision
	else if(g_Ball.height < PX_TO_UNIT(14))
	{
		if(((prevY < PX_TO_UNIT(96)) && (g_Ball.pos.y >= PX_TO_UNIT(96))) || (prevY > PX_TO_UNIT(96)) && (g_Ball.pos.y <= PX_TO_UNIT(96)))
		{
			Ball_Score();
			g_Ball.velXY = 0;
			g_Ball.velZ = 0;
		}
	}

	// Update sprites position
	u8  x = g_Ball.srcPos.x - 3;
	i16 y = g_Ball.srcPos.y - 3;

	// Ball shadow
	if((y >= 82) && (y < 96))
		VDP_SetSpritePosition(SPRITE_BALL_SHADOW,  213, 213);
	else
		VDP_SetSpritePosition(SPRITE_BALL_SHADOW,  x, y);

	// Ball body
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

//-----------------------------------------------------------------------------
///
void Ball_Draw()
{	
	const u8* src = g_DataBall + (1 * 3 * 8);
	u16 dst = g_SpritePattern[g_WriteBuffer] + (80 * 8); // Pattern #80
	VDP_WriteVRAM_64K(src, dst, 8);
	
	src += (1 * 8);
	dst += (4 * 8); // Pattern #84
	VDP_WriteVRAM_64K(src, dst, 8);

	src += (1 * 8);
	dst += (4 * 8); // Pattern #88
	VDP_WriteVRAM_64K(src, dst, 8);
}


//=============================================================================
//
//   P L A Y E R S
//
//=============================================================================

//-----------------------------------------------------------------------------
///
void Player_CheckShoot(Player* ply)
{
	if(g_Ball.coolDown > 0)
		return;
		
	u8 event = g_Actions[ply->action].animFrames[ply->step].event;

	// Check Z coordinate
	if(event == EVENT_SMASH)
	{
		if(g_Ball.height > PX_TO_UNIT(40))
			return;
	}	
	
	u8 dir = 16;
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
		minX = UNIT_TO_PX(ply->pos.x) - 10;
		maxX = minX + 20;
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
		maxY = ply->srcPos.y + 2;
		minY = maxY - 4;
	}
	else
	{
		minY = ply->srcPos.y - 2;
		maxY = minY + 4;
		dir += 32;
	}
	
	u8 ballY = g_Ball.srcPos.y;
	if((ballY < minY) || (ballY > maxY))
		return;
	
	ayFX_PlayBank(1, 0);

	ply->shotCnt = ply->counter;
	//ply->shotCnt -= 4;
	// if(ply->shotCnt > 6)
		// ply->shotCnt = 6;

	// Set shot direction
	if(ply->id == 0)
	{
		if(ply->inLeft())
			dir += ply->shotCnt;
		else if(ply->inRight())
			dir -= ply->shotCnt;
	}
	else
	{
		if(ply->inLeft())
			dir -= ply->shotCnt;
		else if(ply->inRight())
			dir += ply->shotCnt;			
	}
	g_Ball.dir = dir;
	
	// Set shot strength
	u8 shotId = ply->shot * 4; // Long shot
	if(event == EVENT_SMASH)
	{
		shotId += 3;
	}
	else if(ply->inDown()) // Short shot
	{
		shotId += 2;
	}
	else if(!ply->inUp()) // Med shot
	{
		shotId++;
	}
	
	const Shot* shot = &g_Shots[shotId];

	g_Ball.velXY = shot->velXY;
	g_Ball.velZ  = shot->velZ;
	g_Ball.spin  = shot->spin;

	// Misc
	g_Ball.lastPly = ply->id;
	g_Ball.coolDown = 10;
	g_Ball.bounce = 0;
	g_Ball.point = POINT_PENDING;
}







//-----------------------------------------------------------------------------
// GENERIC

/// Set sprite attribute for Sprite Mode 2 and fill color table with unique color
void SetSprite(u8 index, u8 x, u8 y, u8 shape, u8 color)
{
	// Initialize sprite attributes
	if(g_VersionVDP == VDP_VERSION_TMS9918A)
		VDP_SetSpriteSM1(index, x, y, shape, color);
	else
		VDP_SetSpriteExUniColor(index, x, y, shape, color);
}


//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
///
void UpdateAction(Player* ply) __FASTCALL
{
	const Action* act = &g_Actions[ply->action];

	// Finished current animation step
	if(ply->counter >= act->animFrames[ply->step].len)
	{
		ply->counter = 0;
		ply->step++;
	}

	// Finished last animation step
	if(ply->step >= act->animLen)
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
	
	// Handle current action's event
	switch(act->animFrames[ply->step].event)
	{
	case EVENT_NONE:
		break;
	case EVENT_PREPARE:
	case EVENT_SHOOT_R:
	case EVENT_SHOOT_L:
	case EVENT_SMASH:
		/*if(ply->inRight())
		{
			if(ply->shotDir != DIR_RIGHT)
			{
				ply->shotDir = DIR_RIGHT;
				ply->shotCnt = 0;
			}
			ply->shotCnt++;
		}
		if(ply->inLeft())
		{
			if(ply->shotDir != DIR_LEFT)
			{
				ply->shotDir = DIR_LEFT;
				ply->shotCnt = 0;
			}
			ply->shotCnt++;
		}*/
		if(ply->inButton1() && (ply->shot == SHOT_SLICE))
			ply->shot = SHOT_ATTACK;
		if(ply->inButton2() && (ply->shot == SHOT_FLAT))
			ply->shot = SHOT_ATTACK;
		break;
	};

	// Update animation
	ply->anim = act->animFrames[ply->step].frame;
	ply->counter++;
}

//-----------------------------------------------------------------------------
///
void HandleInput(Player* ply) __FASTCALL
{
	if(g_Actions[ply->action].interrupt)
	{			
		if(ply->inButton1() || ply->inButton2())
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
			if(ply->inButton1())
				ply->shot = SHOT_FLAT;
			else // if(ply->inButton2())
				ply->shot = SHOT_SLICE;
			//ply->shotCnt = 0;
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

//-----------------------------------------------------------------------------
///
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

//-----------------------------------------------------------------------------
///
void UpdatePlayer1()
{	
	// Update sprite position
	u8 x = g_Player[PLY1].srcPos.x - 8;
	u8 y = g_Player[PLY1].srcPos.y - 24;
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
	u8 event = g_Actions[g_Player[PLY1].action].animFrames[g_Player[PLY1].step].event;
	switch(event)
	{
	case EVENT_SHOOT_R:
		Player_CheckShoot(&g_Player[PLY1]);
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, x+16, y-16);
		break;
	case EVENT_SHOOT_L:
		Player_CheckShoot(&g_Player[PLY1]);
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, x-8, y-16);
		break;
	case EVENT_SMASH:
		Player_CheckShoot(&g_Player[PLY1]);
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, x+8, y-32);
		break;
	default:
		VDP_SetSpritePosition(SPRITE_PLY1_RACKET, 213, 213);
		break;
	};
}

//-----------------------------------------------------------------------------
///
void DrawPlayer1()
{
	u8 frame = g_Player[PLY1].anim;
	if(g_Player[PLY1].prevAnim[g_WriteBuffer] == frame)
		return;

	g_Player[PLY1].prevAnim[g_WriteBuffer] = frame;

	const u8* src;
	u16 dst;

	src = g_DataPlayer1 + (frame * 224);
	dst = g_SpritePattern[g_WriteBuffer] + (0 * 8); // Pattern #0 - 20
	VDP_WriteVRAM_64K(src, dst, 21 * 8);
	
	src += 21 * 8;
	dst += (22 * 8); // Pattern #22
	VDP_WriteVRAM_64K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #24
	VDP_WriteVRAM_64K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #26
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #28
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #30
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #32
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #34
	VDP_WriteVRAM_64K(src, dst, 8);

	u8 event = g_Actions[g_Player[PLY1].action].animFrames[g_Player[PLY1].step].event;
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
		VDP_WriteVRAM_64K(src, dst, 8);	
	}
}

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
///
void UpdatePlayer2()
{	
	// Update sprite position
	u8 x = g_Player[PLY2].srcPos.x - 8;
	u8 y = g_Player[PLY2].srcPos.y - 32;
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
	u8 event = g_Actions[g_Player[PLY2].action].animFrames[g_Player[PLY2].step].event;
	switch(event)
	{
	case EVENT_SHOOT_R:
		Player_CheckShoot(&g_Player[PLY2]);
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, x+16, y-8);
		break;                                    
	case EVENT_SHOOT_L:                           
		Player_CheckShoot(&g_Player[PLY2]);
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, x-8, y-8);
		break;                                    
	case EVENT_SMASH:                             
		Player_CheckShoot(&g_Player[PLY2]);
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, x+8, y-24);
		break;                                    
	default:                                      
		VDP_SetSpritePosition(SPRITE_PLY2_RACKET, 213, 213);
		break;
	};
}

//-----------------------------------------------------------------------------
///
void DrawPlayer2() __FASTCALL
{
	u8 frame = g_Player[PLY2].anim;
	if(g_Player[PLY2].prevAnim[g_WriteBuffer] == frame)
		return;

	g_Player[PLY2].prevAnim[g_WriteBuffer] = frame;

	const u8* src;
	u16 dst;
		
	src = g_DataPlayer2 + (frame * 224);
	dst = g_SpritePattern[g_WriteBuffer] + (41 * 8); // Pattern #41
	VDP_WriteVRAM_64K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #43
	VDP_WriteVRAM_64K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #45
	VDP_WriteVRAM_64K(src, dst, 8);
	
	src += 8;
	dst += (2 * 8); // Pattern #47
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #49
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #51
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #53
	VDP_WriteVRAM_64K(src, dst, 8);

	src += 8;
	dst += (2 * 8);  // Pattern #55 - #75
	VDP_WriteVRAM_64K(src, dst, 21 * 8);

	u8 event = g_Actions[g_Player[PLY2].action].animFrames[g_Player[PLY2].step].event;
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
		VDP_WriteVRAM_64K(src, dst, 8);	
	}
}

//-----------------------------------------------------------------------------
///
void PrepareLauncher()
{
	// Flicker Shadows
	u8 pat = 40;
	if(g_FlickerShadow && (g_Frame & 0x1))
	{
		pat += 8;
	}
	VDP_SetSpritePattern(SPRITE_PLY2_BLACK_H, pat);     // Pattern #40 / #48
	VDP_SetSpritePattern(SPRITE_PLY2_BLACK_L, pat + 4); // Pattern #44 / #52
}


//-----------------------------------------------------------------------------
// INPUT CALLBACK

bool KB1_Button1()	{ return (IS_KEY_PRESSED(g_KeyRow8, KEY_SPACE) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_SPACE))); }	
bool KB1_Button2()	{ return (IS_KEY_PRESSED(g_KeyRow4, KEY_N) && !(IS_KEY_PRESSED(g_PrevRow4, KEY_N))); }
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
//   G A M E   S T A T E S
//
//=============================================================================

//-----------------------------------------------------------------------------
///
void UpdateInput()
{
	// Keyboard
	g_PrevRow4 = g_KeyRow4;	
	g_PrevRow6 = g_KeyRow6;	
	g_PrevRow8 = g_KeyRow8;	
	g_KeyRow4 = Keyboard_Read(4); // K L M N O P Q R
	g_KeyRow6 = Keyboard_Read(6); // SHIFT CTRL GRAPH CAPS CODE F1 F2 F3
	g_KeyRow8 = Keyboard_Read(8); // SPACE HOME INS DEL LEFT UP DOWN RIGHT
	// Joystick
	g_PrevJoy1 = g_Joy1;
	g_PrevJoy2 = g_Joy2;
	g_Joy1 = Joystick_Read(JOY_PORT_1);
	g_Joy2 = Joystick_Read(JOY_PORT_2);
}

void VDP_FillScreen_GM2(u8 value) __FASTCALL
{
	VDP_FillVRAM_64K(value, g_ScreenLayoutLow, 32*24);
}

void VDP_LoadPattern_GM2(const u8* src, u8 count, u8 offset)
{
	u16 dst = g_ScreenPatternLow + (offset * 8);
	VDP_WriteVRAM_64K(src, dst, count * 8);
	dst += 0x800;
	VDP_WriteVRAM_64K(src, dst, count * 8);
	dst += 0x800;
	VDP_WriteVRAM_64K(src, dst, count * 8);
}

void VDP_LoadColor_GM2(const u8* src, u8 count, u8 offset)
{
	u16 dst = g_ScreenColorLow + (offset * 8);
	VDP_WriteVRAM_64K(src, dst, count * 8);
	dst += 0x800;
	VDP_WriteVRAM_64K(src, dst, count * 8);
	dst += 0x800;
	VDP_WriteVRAM_64K(src, dst, count * 8);
}

void VDP_DrawLayout_GM2(const u8* src, u8 dx, u8 dy, u8 nx, u8 ny)
{
	u16 dst = g_ScreenLayoutLow + (dy * 32) + dx;
	for(u8 y = 0; y < ny; ++y)
	{
		VDP_WriteVRAM_64K(src, dst, nx);
		src += nx;
		dst += 32;
	}
}

//-----------------------------------------------------------------------------
///
void StateTitle_Start()
{
	VDP_EnableDisplay(false);

	VDP_SetSpritePatternTable(g_SpritePattern[0]);
	VDP_SetColor(COLOR_BLACK);

	// Load screen data
	VDP_FillScreen_GM2(0); // Don't set the Layout table yet
	VDP_LoadPattern_GM2(g_DataLogo_Patterns, sizeof(g_DataLogo_Patterns)/8, 0);
	VDP_LoadColor_GM2(g_DataLogo_Colors, sizeof(g_DataLogo_Colors)/8, 0);

	// Load sprites data
	VDP_WriteVRAM_64K(g_DataLogoBall, g_SpritePatternLow, 8 * 32);

	// Initialize sprite attributes
	SetSprite(0, 0, 0, 0,  VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	SetSprite(1, 0, 0, 4,  VDP_SPRITE_EC | COLOR_DARK_RED);
	SetSprite(2, 0, 0, 8,  VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	SetSprite(3, 0, 0, 12, VDP_SPRITE_EC | COLOR_DARK_RED);
	SetSprite(4, 0, 0, 16, VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	SetSprite(5, 0, 0, 20, VDP_SPRITE_EC | COLOR_DARK_RED);
	SetSprite(6, 0, 0, 24, VDP_SPRITE_EC | COLOR_LIGHT_YELLOW);
	SetSprite(7, 0, 0, 28, VDP_SPRITE_EC | COLOR_DARK_RED);
	VDP_HideSpriteFrom(8);

	// Initialize font
	g_FontDefaultOffset = sizeof(g_DataLogo_Patterns) / 8;
	Print_SetTextFont(FONT, g_FontDefaultOffset);
	Print_SetColor(0xF, 0x0);
	g_FontSelectOffset = g_FontDefaultOffset + g_PrintData.CharCount;
	Print_SetTextFont(FONT, g_FontSelectOffset);
	Print_SetColorShade(g_ColorShade);
	
	g_IntroFrame = 0;

	VDP_EnableDisplay(true);
}

//-----------------------------------------------------------------------------
///
void StateTitle_Update()
{
	UpdateInput();

	if(g_IntroFrame < 26) // Ball movement
	{
		VDP_SetSpritePosition(0, g_IntroFrame * 8 +  0 - 4, 32 +  0);
		VDP_SetSpritePosition(1, g_IntroFrame * 8 +  0 - 4, 32 +  0);
		VDP_SetSpritePosition(2, g_IntroFrame * 8 + 16 - 4, 32 +  0);
		VDP_SetSpritePosition(3, g_IntroFrame * 8 + 16 - 4, 32 +  0);
		VDP_SetSpritePosition(4, g_IntroFrame * 8 +  0 - 4, 32 + 16);
		VDP_SetSpritePosition(5, g_IntroFrame * 8 +  0 - 4, 32 + 16);
		VDP_SetSpritePosition(6, g_IntroFrame * 8 + 16 - 4, 32 + 16);
		VDP_SetSpritePosition(7, g_IntroFrame * 8 + 16 - 4, 32 + 16);
	}
	else if(g_IntroFrame == 26) // White flash
	{	
		ayFX_PlayBank(13, 0);
		VDP_SetColor(COLOR_WHITE);
		VDP_FillVRAM_64K(0xFF, g_ScreenColorLow + (0 * 0x800),  8);
		VDP_FillVRAM_64K(0xFF, g_ScreenColorLow + (1 * 0x800),  8);
		VDP_FillVRAM_64K(0xFF, g_ScreenColorLow + (2 * 0x800),  8);
	}
	else if(g_IntroFrame == 30) // Title
	{	
		VDP_SetColor(COLOR_BLACK);
		VDP_WriteVRAM_64K(g_DataLogo_Colors, g_ScreenColorLow + (0 * 0x800),  8);
		VDP_WriteVRAM_64K(g_DataLogo_Colors, g_ScreenColorLow + (1 * 0x800),  8);
		VDP_WriteVRAM_64K(g_DataLogo_Colors, g_ScreenColorLow + (2 * 0x800),  8);
		VDP_DrawLayout_GM2(g_DataLogo_Names, 4, 2, 19, 10);
	}
	else if(g_IntroFrame == 64) // Title
	{
		PT3_Resume();

		Print_SelectTextFont(g_FontDefaultOffset);
		Print_SetPosition(11, 18);
		Print_DrawText("PRESS SPACE");
	}
	if(g_IntroFrame < 255)
		g_IntroFrame++;
	
	if(Keyboard_IsKeyPressed(KEY_SPACE)) // Skip
	{
		PT3_Resume();

		VDP_SetColor(COLOR_BLACK);
		VDP_WriteVRAM_64K(g_DataLogo_Colors, g_ScreenColorLow + (0 * 0x800),  8);
		VDP_WriteVRAM_64K(g_DataLogo_Colors, g_ScreenColorLow + (1 * 0x800),  8);
		VDP_WriteVRAM_64K(g_DataLogo_Colors, g_ScreenColorLow + (2 * 0x800),  8);
		VDP_DrawLayout_GM2(g_DataLogo_Names, 4, 2, 19, 10);

		g_IntroFrame = 25;
		VDP_SetSpritePosition(0, g_IntroFrame * 8 +  0 - 4, 32 +  0);
		VDP_SetSpritePosition(1, g_IntroFrame * 8 +  0 - 4, 32 +  0);
		VDP_SetSpritePosition(2, g_IntroFrame * 8 + 16 - 4, 32 +  0);
		VDP_SetSpritePosition(3, g_IntroFrame * 8 + 16 - 4, 32 +  0);
		VDP_SetSpritePosition(4, g_IntroFrame * 8 +  0 - 4, 32 + 16);
		VDP_SetSpritePosition(5, g_IntroFrame * 8 +  0 - 4, 32 + 16);
		VDP_SetSpritePosition(6, g_IntroFrame * 8 + 16 - 4, 32 + 16);
		VDP_SetSpritePosition(7, g_IntroFrame * 8 + 16 - 4, 32 + 16);

		Game_SetState(&g_State_Menu);
	}
}

//-----------------------------------------------------------------------------
///
void StateMenu_Start()
{
	Menu_Initialize(&g_Menus[g_MenuID]);
}

//-----------------------------------------------------------------------------
///
void StateMenu_Update()
{
	UpdateInput();
	Menu_Update();
}

//-----------------------------------------------------------------------------
///
void InitializeCourt()
{
	VDP_SetSpritePatternTable(g_SpritePattern[0]);	
	VDP_SetColor(COLOR_DARK_RED);

	// Load court data to VRAM
	VDP_FillScreen_GM2(0);
	VDP_DrawLayout_GM2(g_DataCourt_Names, 3, 3, 27, 18);
	VDP_LoadPattern_GM2(g_DataCourt_Patterns, sizeof(g_DataCourt_Patterns)/8, 0);
	VDP_LoadColor_GM2(g_DataCourt_Colors, sizeof(g_DataCourt_Colors)/8, 0);

	// Initialize sprites
	VDP_FillVRAM(0x00, g_SpritePattern[0], 0, 128*8); // Clear sprite patterns table (only 128 entry used)
	VDP_FillVRAM(0x00, g_SpritePattern[1], 0, 128*8);
	
	// Setup ball sprites
	SetSprite(SPRITE_BALL_OUTLINE, 0, 193, 80, COLOR_BLACK);			// Outline
	SetSprite(SPRITE_BALL_BODY,    0, 193, 88, COLOR_LIGHT_YELLOW);		// Body
	SetSprite(SPRITE_BALL_SHADOW,  0, 193, 88, COLOR_BLACK);			// Shadow

	// Setup net sprites
	SetSprite(SPRITE_NET_POST_W, 0, 193, 112, COLOR_WHITE);			// Net post 1
	SetSprite(SPRITE_NET_POST_G, 0, 193, 116, COLOR_GRAY);			// Net post 2
	SetSprite(SPRITE_NET_LEFT,   0, 193, 120, COLOR_WHITE);			// Net 1
	SetSprite(SPRITE_NET_RIGHT,  0, 193, 124, COLOR_WHITE);			// Net 2

	VDP_HideSpriteFrom(SPRITE_MAX);

	PT3_Pause();
}

//-----------------------------------------------------------------------------
///
void InitializeSpriteP1()
{
	// Setup player 1 sprites
	SetSprite(SPRITE_PLY1_BLACK_H, 0, 193, 0,  COLOR_BLACK);			// Outline
	SetSprite(SPRITE_PLY1_BLACK_L, 0, 193, 20, COLOR_BLACK);			// Outline
	SetSprite(SPRITE_PLY1_RACKET,  0, 193, 36, COLOR_BLACK);			// Racket
	SetSprite(SPRITE_PLY1_CLOTH,   0, 193, 8,  COLOR_LIGHT_BLUE);		// Cloth
	SetSprite(SPRITE_PLY1_WHITE_H, 0, 193, 12, COLOR_WHITE);			// White
	SetSprite(SPRITE_PLY1_WHITE_L, 0, 193, 28, COLOR_WHITE);			// White
	SetSprite(SPRITE_PLY1_SKIN_H,  0, 193, 16, COLOR_LIGHT_RED);		// Skin
	SetSprite(SPRITE_PLY1_SKIN_L,  0, 193, 32, COLOR_LIGHT_RED);		// Skin

	// Initialize player 1
	g_Player[PLY1].id = 0;
	g_Player[PLY1].pos.x = PX_TO_UNIT(100);
	g_Player[PLY1].pos.y = PX_TO_UNIT(130);
	g_Player[PLY1].srcPos.x = UNIT_TO_PX(g_Player[PLY1].pos.x);
	g_Player[PLY1].srcPos.y = UNIT_TO_PX(g_Player[PLY1].pos.y);
	g_Player[PLY1].action = ACTION_IDLE;
	g_Player[PLY1].counter = 0;
	g_Player[PLY1].anim = 0;
	g_Player[PLY1].prevAnim[0] = 0xFF;
	g_Player[PLY1].prevAnim[1] = 0xFF;
	g_Player[PLY1].step = 0;
	g_Player[PLY1].inButton1 = KB2_Button1;
	g_Player[PLY1].inButton2 = KB2_Button2;
	g_Player[PLY1].inUp      = KB2_Up;
	g_Player[PLY1].inDown    = KB2_Down;
	g_Player[PLY1].inLeft    = KB2_Left;
	g_Player[PLY1].inRight   = KB2_Right;
}

//-----------------------------------------------------------------------------
///
void InitializeSpriteP2()
{
	// Setup player 2 sprites
	SetSprite(SPRITE_PLY2_BLACK_H, 0, 193, 40, COLOR_BLACK);			// Outline
	SetSprite(SPRITE_PLY2_BLACK_L, 0, 193, 56, COLOR_BLACK);			// Outline
	SetSprite(SPRITE_PLY2_RACKET,  0, 193, 76, COLOR_BLACK);			// Racket
	SetSprite(SPRITE_PLY2_CLOTH,   0, 193, 64, COLOR_MEDIUM_GREEN);	// Cloth
	SetSprite(SPRITE_PLY2_WHITE_H, 0, 193, 48, COLOR_WHITE);			// White
	SetSprite(SPRITE_PLY2_WHITE_L, 0, 193, 68, COLOR_WHITE);			// White
	SetSprite(SPRITE_PLY2_SKIN_H,  0, 193, 52, COLOR_LIGHT_RED);		// Skin
	SetSprite(SPRITE_PLY2_SKIN_L,  0, 193, 72, COLOR_LIGHT_RED);		// Skin

	// Initialize player 2
	g_Player[PLY2].id = 1;
	g_Player[PLY2].pos.x = PX_TO_UNIT(150);
	g_Player[PLY2].pos.y = PX_TO_UNIT(32);
	g_Player[PLY2].srcPos.x = UNIT_TO_PX(g_Player[PLY2].pos.x);
	g_Player[PLY2].srcPos.y = UNIT_TO_PX(g_Player[PLY2].pos.y);
	g_Player[PLY2].action = ACTION_IDLE;
	g_Player[PLY2].counter = 0;
	g_Player[PLY2].anim = 0;
	g_Player[PLY2].prevAnim[0] = 0xFF;
	g_Player[PLY2].prevAnim[1] = 0xFF;
	g_Player[PLY2].step = 0;
	g_Player[PLY2].inButton1 = Joy2_Button1;
	g_Player[PLY2].inButton2 = Joy2_Button2;
	g_Player[PLY2].inUp      = Joy2_Up;
	g_Player[PLY2].inDown    = Joy2_Down;
	g_Player[PLY2].inLeft    = Joy2_Left;
	g_Player[PLY2].inRight   = Joy2_Right;
}

//-----------------------------------------------------------------------------
///
void InitializeLauncher()
{
	// Load launcher pattern
	u16 dst;
	dst	= g_SpritePattern[g_WriteBuffer] + (40 * 8); // Pattern #40 - #71
	VDP_WriteVRAM_64K(g_DataLauncher, dst, 8 * 8 *4);
	dst	= g_SpritePattern[g_DisplayBuffer] + (40 * 8); // Pattern #40 - #71
	VDP_WriteVRAM_64K(g_DataLauncher, dst, 8 * 8 *4);
	
	// Setup launcher sprites
	SetSprite(SPRITE_PLY2_BLACK_H, 128-8, -4 +  0, 40, COLOR_BLACK);			// Black 1&2
	SetSprite(SPRITE_PLY2_BLACK_L, 128-8, -4 + 16, 44, COLOR_BLACK);			// Black 1&2
	SetSprite(SPRITE_PLY2_RACKET,  128-8, -4 +  0, 56, COLOR_LIGHT_YELLOW);	// Yellow
	SetSprite(SPRITE_PLY2_CLOTH,   128-8, -4 +  8, 60, COLOR_MEDIUM_GREEN);	// Green
	SetSprite(SPRITE_PLY2_WHITE_H, 128-8, -4 + 16, 64, COLOR_WHITE);			// White
	SetSprite(SPRITE_PLY2_WHITE_L,     0,     193,  0, 0);					// Unused
	SetSprite(SPRITE_PLY2_SKIN_H,      0,     193,  0, 0);					// Unused
	SetSprite(SPRITE_PLY2_SKIN_L,      0,     193,  0, 0);					// Unused
}
//-----------------------------------------------------------------------------
///
void StateMatch_Start()
{
	VDP_EnableDisplay(false);

	InitializeCourt();
	InitializeSpriteP1();
	InitializeSpriteP2();
	
	g_ScoreFct = null;

	// Initialize ball
	Ball_ShootRandom();

	VDP_EnableDisplay(true);
}

//-----------------------------------------------------------------------------
///
void StateMatch_Update()
{
// VDP_SetColor(COLOR_BLACK);

	//---------------------------------------------------------------------
	// Switch buffer
	g_WriteBuffer = 1 - g_WriteBuffer;
	g_DisplayBuffer = 1 - g_DisplayBuffer;
	VDP_SetSpritePatternTable(g_SpritePattern[g_DisplayBuffer]);
	PreparePlayer1();
	PreparePlayer2();
	Ball_Prepare();

// VDP_SetColor(COLOR_LIGHT_BLUE);

	//---------------------------------------------------------------------
	// Update input

	UpdateInput();


	if(IS_KEY_PRESSED(g_KeyRow6, KEY_F2) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_F2)))
		Ball_ShootRandom();
	if(IS_KEY_PRESSED(g_KeyRow6, KEY_F3) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_F3)))
		g_FlickerShadow = 1 - g_FlickerShadow;
	// Return to main menu
	if(IS_KEY_PRESSED(g_KeyRow8, KEY_DEL))
	{
		VDP_HideSpriteFrom(0);
		Game_SetState(&g_State_Title);
	}
	
	HandleInput(&g_Player[PLY1]);
	HandleInput(&g_Player[PLY2]);

// VDP_SetColor(COLOR_LIGHT_GREEN);
	
	//---------------------------------------------------------------------
	// Update sprites position
	
	UpdatePlayer1();
	UpdatePlayer2();
	Ball_Update();

	UpdateAction(&g_Player[PLY1]);
	UpdateAction(&g_Player[PLY2]);

// VDP_SetColor(COLOR_LIGHT_YELLOW);

	//---------------------------------------------------------------------
	// Draw anim
	
	DrawPlayer1();
	DrawPlayer2();
	Ball_Draw();

// VDP_SetColor(COLOR_DARK_RED);
}

//-----------------------------------------------------------------------------
///
void TrainingScore()
{
	if((g_Ball.point == POINT_VALIDATED) && (g_Ball.lastPly == PLY1))
	{
		g_TrainScore++;
		if(g_TrainScore > g_TrainBest)
			g_TrainBest = g_TrainScore;
	}
	else if(((g_Ball.point == POINT_PENDING) && (g_Ball.lastPly == PLY1))
		|| ((g_Ball.point == POINT_VALIDATED) && (g_Ball.lastPly == PLY2)))
	{
		u16 dst = g_ScreenLayoutLow + (1 * 32) + 7;
		VDP_FillVRAM_64K(0, dst, 4);
		g_TrainScore = 0;
	}
	Print_SetPosition(7, 1);
	Print_DrawInt(g_TrainScore);
	Print_SetPosition(28, 1);
	Print_DrawInt(g_TrainBest);
}

//-----------------------------------------------------------------------------
///
void StateTraining_Start()
{
	VDP_EnableDisplay(false);

	InitializeCourt();
	InitializeSpriteP1();
	InitializeLauncher();
	
	// Initialize font
	g_FontDefaultOffset = sizeof(g_DataCourt_Patterns) / 8;
	Print_SetTextFont(FONT, g_FontDefaultOffset);
	Print_SetColor(0xF, 0x9);

	// Initialize scoring
	Print_SetPosition(1, 1);
	Print_DrawText("SCORE");
	Print_SetPosition(23, 1);
	Print_DrawText("BEST");

	g_TrainScore = 0;
	g_ScoreFct = TrainingScore;
	TrainingScore();

	// Initialize ball
	Ball_ShootRandom();

	VDP_EnableDisplay(true);
}

//-----------------------------------------------------------------------------
///
void StateTraining_Update()
{
	//---------------------------------------------------------------------
	// Switch buffer

	g_WriteBuffer = 1 - g_WriteBuffer;
	g_DisplayBuffer = 1 - g_DisplayBuffer;
	VDP_SetSpritePatternTable(g_SpritePattern[g_DisplayBuffer]);

	PreparePlayer1();
	PrepareLauncher();
	Ball_Prepare();

	//---------------------------------------------------------------------
	// Update input

	UpdateInput();

	if(IS_KEY_PRESSED(g_KeyRow6, KEY_F2) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_F2)))
		Ball_ShootRandom();
	if(IS_KEY_PRESSED(g_KeyRow6, KEY_F3) && !(IS_KEY_PRESSED(g_PrevRow6, KEY_F3)))
		g_FlickerShadow = 1 - g_FlickerShadow;

	#if (DEBUG)

		if(IS_KEY_PRESSED(g_KeyRow8, KEY_HOME) && !(IS_KEY_PRESSED(g_PrevRow8, KEY_HOME)))
		{
			g_Debug = 1 - g_Debug;

			u8 y = 4;
			Print_SetPosition(1, y++);
			Print_DrawText("ACT:");
			Print_SetPosition(1, y++);
			Print_DrawText("STP:");
			Print_SetPosition(1, y++);
			Print_DrawText("ANI:");
			y++;
			Print_SetPosition(1, y++);
			Print_DrawText("SHT:");
			Print_SetPosition(1, y++);
			Print_DrawText("DIR:");
			Print_SetPosition(1, y++);
			Print_DrawText("CNT:");

			Print_SetPosition(1, 22);
			Print_DrawText((g_VersionVDP == VDP_VERSION_TMS9918A) ? "MSX1" : "MSX2");
		}

		if(g_Debug)
		{
			u8 y = 4;
			Print_SetPosition(5, y++);
			Print_DrawHex8(g_Player[0].action);
			Print_SetPosition(5, y++);
			Print_DrawHex8(g_Player[0].step);
			Print_SetPosition(5, y++);
			Print_DrawHex8(g_Player[0].anim);
			y++;
			Print_SetPosition(5, y++);
			Print_DrawHex8(g_Player[0].shot);
			Print_SetPosition(5, y++);
			Print_DrawHex8(g_Player[0].shotDir);
			Print_SetPosition(5, y++);
			Print_DrawHex8(g_Player[0].shotCnt);
		}
	#endif

	// Return to main menu
	if(IS_KEY_PRESSED(g_KeyRow8, KEY_DEL))
	{
		VDP_HideSpriteFrom(0);
		Game_SetState(&g_State_Title);
	}
	
	HandleInput(&g_Player[PLY1]);
	
	//---------------------------------------------------------------------
	// Update sprites position
	
	UpdatePlayer1();
	Ball_Update();

	UpdateAction(&g_Player[PLY1]);

	//---------------------------------------------------------------------
	// Draw anim
	
	DrawPlayer1();
	Ball_Draw();
}

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
	g_VersionVDP = VDP_GetVersion();

	// Initialize VDP
	if(g_VersionVDP == VDP_VERSION_TMS9918A)
	{
		VDP_SetMode(VDP_MODE_GRAPHIC2);
		VDP_SetLayoutTable(MSX1_LAYOUT_TABLE);
		VDP_SetColorTable(MSX1_COLOR_TABLE);
		VDP_SetPatternTable(MSX1_PATTERN_TABLE);
		VDP_SetSpritePatternTable(MSX1_SPRITE_PATTERN_0);
		VDP_SetSpriteAttributeTable(MSX1_SPRITE_ATTRIBUTE);
		g_SpritePattern[0] = MSX1_SPRITE_PATTERN_0;
		g_SpritePattern[1] = MSX1_SPRITE_PATTERN_1;
	}
	else // VDP_VERSION_V99xx
	{
		VDP_SetMode(VDP_MODE_GRAPHIC3);
		VDP_SetLayoutTable(MSX2_LAYOUT_TABLE);
		VDP_SetColorTable(MSX2_COLOR_TABLE);
		VDP_SetPatternTable(MSX2_PATTERN_TABLE);
		VDP_SetSpritePatternTable(MSX2_SPRITE_PATTERN_0);
		VDP_SetSpriteAttributeTable(MSX2_SPRITE_ATTRIBUTE);
		g_SpritePattern[0] = MSX2_SPRITE_PATTERN_0;
		g_SpritePattern[1] = MSX2_SPRITE_PATTERN_1;
	}
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);
	VDP_EnableVBlank(true);
	
	// Initialize PT3
	PT3_Init();
	PT3_SetNoteTable(PT3_NT2);
	PT3_SetLoop(true);
	PT3_InitSong(g_intro);

	// Initialize ayFX
	ayFX_InitBank(g_ayfx_bank);
	ayFX_SetChannel(PSG_CHANNEL_A);
	ayFX_SetMode(AYFX_MODE_FIXED);

	Game_Initialize();
	Game_SetVSyncCallback(VSyncCallback);
	Game_SetState(&g_State_Title);

	while(1)
		Game_Update();
}
