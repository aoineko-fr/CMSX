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
#include "input.h"

//=============================================================================
//
//   D E F I N E S
//
//=============================================================================

//-----------------------------------------------------------------------------
// DEFINES
#define VERSION			"V0.1.0"

// VRAM Tables Address
#define VRAM_NAME_TABLE			0x3C00
#define VRAM_COLOR_TABLE		0x2000
#define VRAM_PATERN_TABLE		0x0000
#define VRAM_SPRITE_ATTRIBUTE	0x3F00
#define VRAM_SPRITE_PATTERN		0x1800

/// Player Actions
enum ACTION
{
	ACTION_IDLE = 0,
	ACTION_WALK,
	ACTION_SHOOT,
	ACTION_SMASH,
};

//-----------------------------------------------------------------------------
// TYPES

/// 8-bits vector structure
typedef struct
{
	u8		x;	
	u8		y;	
} Vector;

/// 8-bits vector structure
typedef struct
{
	Vector	pos;
	u8		action;
	u8		frame;
	u8		anim;
	u8		right;	// 1=right, 0=left
	u8		ctrl;	// 1=true, 0=false (puppet)
} Player;

//=============================================================================
//
//   D A T A
//
//=============================================================================

// Fonts
#include "font\font_carwar.h"

// Player 1 - Sprites
#include "data\ply1_blk1.data.h"
#include "data\ply1_blk2.data.h"
#include "data\ply1_clth.data.h"
#include "data\ply1_skin.data.h"
#include "data\ply1_whit.data.h"

// Player 2 - Sprites
#include "data\ply2_blk1.data.h"
#include "data\ply2_blk2.data.h"
#include "data\ply2_clth.data.h"
#include "data\ply2_skin.data.h"
#include "data\ply2_whit.data.h"

// Court G2 data tables
#include "data\court.data.h"

// Global variables
bool   g_VSynch = false;
u8     g_Frame = 0;
Player g_Player[2];


//=============================================================================
//
//   C O D E
//
//=============================================================================

/// H_TIMI interrupt hook
void VBlankHook()
{
	g_VSynch = true;
}

/// Wait for V-Blank period
void WaitVBlank()
{
	while(g_VSynch == false) {}
	g_Frame++;
}

///
void DrawPlayer1(u8 frame) __FASTCALL
{
	const u8* src;
	u16 dst;

	// Black
	if(g_Frame & 0x1)
		src = g_Ply1_Blk1 + (frame * 16);
	else
		src = g_Ply1_Blk2 + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8* 4; // Pattern #4
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #6
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #5
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #7
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst += 8 * 1; // Pattern #8
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #10
	VDP_WriteVRAM_16K(src, dst, 8);
		
	// Skin
	src = g_Ply1_Skin + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 12; // Pattern #12
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #14
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #13
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #15
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst += 8 * 1; // Pattern #16
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #18
	VDP_WriteVRAM_16K(src, dst, 8);

	// Cloth
	src = g_Ply1_Clth + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 20; // Pattern #20
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #22
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #21
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #23
	VDP_WriteVRAM_16K(src, dst, 8);

	// White
	src = g_Ply1_Whit + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 24; // Pattern #24
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #26
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #25
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #27
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst += 8 * 1; // Pattern #28
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #30
	VDP_WriteVRAM_16K(src, dst, 8);
}

///
void DrawPlayer2(u8 frame) __FASTCALL
{
	const u8* src;
	u16 dst;

	// Black
	if(g_Frame & 0x1)
		src = g_Ply2_Blk1 + (frame * 16);
	else
		src = g_Ply2_Blk2 + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 37; // Pattern #37
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #39
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst += 8 * 1; // Pattern #40
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #42
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #41
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #43
	VDP_WriteVRAM_16K(src, dst, 8);
		
	// Skin
	src = g_Ply2_Skin + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 45; // Pattern #45
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #47
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst += 8 * 1; // Pattern #48
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #50
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #49
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #51
	VDP_WriteVRAM_16K(src, dst, 8);

	// Cloth
	src = g_Ply2_Clth + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 52; // Pattern #52
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #54
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #53
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #55
	VDP_WriteVRAM_16K(src, dst, 8);

	// White
	src = g_Ply2_Whit + (frame * 16);
	dst = VRAM_SPRITE_PATTERN + 8 * 57; // Pattern #57
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #59
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst += 8 * 1; // Pattern #60
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #62
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 25;
	dst -= 8 * 1; // Pattern #61
	VDP_WriteVRAM_16K(src, dst, 8);
	src += 8 * 1;
	dst += 8 * 2; // Pattern #63
	VDP_WriteVRAM_16K(src, dst, 8);
}

///
void UpdatePlayer1Position()
{
	if(g_Player[0].pos.x < 8)
		g_Player[0].pos.x = 8;
	else if(g_Player[0].pos.x > 255 - 8)
		g_Player[0].pos.x = (u8)(255 - 8);
	if(g_Player[0].pos.y < 96 + 8)
		g_Player[0].pos.y = 96 + 8;
	else if(g_Player[0].pos.y > 191 + 8)
		g_Player[0].pos.y = 191 + 8;
	
	u8 x = g_Player[0].pos.x - 8;
	u8 y = g_Player[0].pos.y - 24;
	VDP_SetSpritePosition(1, x, y);
	VDP_SetSpritePosition(3, x, y);
	VDP_SetSpritePosition(7, x, y);
	y += 8;
	VDP_SetSpritePosition(5, x, y);
	y += 8;
	VDP_SetSpritePosition(2, x, y);
	VDP_SetSpritePosition(4, x, y);
	VDP_SetSpritePosition(8, x, y);
}

///
void UpdatePlayer2Position()
{
	u8 x = g_Player[1].pos.x - 8;
	u8 y = g_Player[1].pos.y - 24;
	VDP_SetSpritePosition(10, x, y);
	VDP_SetSpritePosition(12, x, y);
	VDP_SetSpritePosition(15, x, y);
	y += 16;
	VDP_SetSpritePosition(11, x, y);
	VDP_SetSpritePosition(13, x, y);
	VDP_SetSpritePosition(14, x, y);
	VDP_SetSpritePosition(16, x, y);	
}



//=============================================================================
//
//   M A I N
//
//=============================================================================

//-----------------------------------------------------------------------------
/// Main loop
void main()
{
	// Graphics 2 Mode, No External Video
	// 16K, Enable Disp., Disable Int., 16x16 Sprites, Mag. Off
	// Address of Name Table in VRAM = 3800h
	// Address of Color Table in VRAM = 2000h
	// Address of Patern Table in VRAM = 0000h
	// Address of Sprite Attribute Table in VRAM = 3B00h / 3B80h
	// Address of Sprite Pattern Table in VRAM = 1800h / 1C00h
	VDP_SetMode(VDP_MODE_GRAPHIC2);
	VDP_SetColor(COLOR_DARK_RED);
	VDP_EnableDisplay(false);
	VDP_SetLayoutTable(VRAM_NAME_TABLE);
	VDP_SetColorTable(VRAM_COLOR_TABLE);
	VDP_SetPaternTable(VRAM_PATERN_TABLE);


	// Court Data
	VDP_WriteVRAM_16K(g_Court_Names,    VRAM_NAME_TABLE, sizeof(g_Court_Names));
	VDP_WriteVRAM_16K(g_Court_Patterns, VRAM_PATERN_TABLE + (0 * 0x800), sizeof(g_Court_Patterns));
	VDP_WriteVRAM_16K(g_Court_Patterns, VRAM_PATERN_TABLE + (1 * 0x800), sizeof(g_Court_Patterns));
	VDP_WriteVRAM_16K(g_Court_Patterns, VRAM_PATERN_TABLE + (2 * 0x800), sizeof(g_Court_Patterns));
	VDP_WriteVRAM_16K(g_Court_Colors,   VRAM_COLOR_TABLE + (0 * 0x800), sizeof(g_Court_Colors));
	VDP_WriteVRAM_16K(g_Court_Colors,   VRAM_COLOR_TABLE + (1 * 0x800), sizeof(g_Court_Colors));
	VDP_WriteVRAM_16K(g_Court_Colors,   VRAM_COLOR_TABLE + (2 * 0x800), sizeof(g_Court_Colors));
	
	// Players Sprites
	VDP_SetSpritePatternTable(VRAM_SPRITE_PATTERN);
	VDP_SetSpriteAttributeTable(VRAM_SPRITE_ATTRIBUTE);
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16 | VDP_SPRITE_SCALE_1);
	VDP_FillVRAM(0x00, VRAM_SPRITE_PATTERN, 0, 256*8); // pattern

	// 
	VDP_SetSpriteSM1(1, 0, 0,  4, COLOR_BLACK);
	VDP_SetSpriteSM1(2, 0, 0,  8, COLOR_BLACK);
	VDP_SetSpriteSM1(3, 0, 0, 12, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1(4, 0, 0, 16, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1(5, 0, 0, 20, COLOR_LIGHT_BLUE);
	VDP_SetSpriteSM1(7, 0, 0, 24, COLOR_WHITE);
	VDP_SetSpriteSM1(8, 0, 0, 28, COLOR_WHITE);

	// 
	VDP_SetSpriteSM1(10, 0, 0, 36, COLOR_BLACK);
	VDP_SetSpriteSM1(11, 0, 0, 40, COLOR_BLACK);
	VDP_SetSpriteSM1(12, 0, 0, 44, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1(13, 0, 0, 48, COLOR_LIGHT_RED);
	VDP_SetSpriteSM1(14, 0, 0, 52, COLOR_LIGHT_GREEN);
	VDP_SetSpriteSM1(15, 0, 0, 56, COLOR_WHITE);
	VDP_SetSpriteSM1(16, 0, 0, 60, COLOR_WHITE);

	VDP_EnableVBlank(true);
	Bios_SetHookCallback(H_TIMI, VBlankHook);

	VDP_EnableDisplay(true);
	
	
	g_Player[0].pos.x = 100;
	g_Player[0].pos.y = 130;
	g_Player[0].action = ACTION_IDLE;
	g_Player[0].frame = 0;
	g_Player[0].anim = 0;
	g_Player[0].right = true;
	g_Player[0].ctrl = true;

	g_Player[1].pos.x = 150;
	g_Player[1].pos.y = 32;
	


	u8 keyRow8; // SPACE HOME INS DEL LEFT UP DOWN RIGHT
	u8 prevRow8 = 0xFF;

	while(1)
	{
		g_VSynch = false;
		WaitVBlank();

		//---------------------------------------------------------------------
		// Update sprites position
		UpdatePlayer1Position();
		UpdatePlayer2Position();

		//---------------------------------------------------------------------
		// Update anim
		
		switch(g_Player[0].action)
		{
		case ACTION_IDLE:
			g_Player[0].anim = 4;
			break;
		case ACTION_WALK:
			if(g_Player[0].right)
				g_Player[0].anim = 5 + ((g_Frame >> 3) & 0x1);
			else
				g_Player[0].anim = 2 + ((g_Frame >> 3) & 0x1);
			break;
		case ACTION_SHOOT:
			g_Player[0].frame++;
			if(g_Player[0].right)
				g_Player[0].anim = 7 + ((g_Player[0].frame >> 3) & 0x1);
			else
				g_Player[0].anim = 1 - ((g_Player[0].frame >> 3) & 0x1);
			if(g_Player[0].frame >= (2 << 3))
			{
				g_Player[0].action = ACTION_IDLE;
				g_Player[0].ctrl = true;
			}
			break;
		};
		
		//---------------------------------------------------------------------
		// Draw anim
		
		// VDP_SetColor(COLOR_LIGHT_BLUE);
		DrawPlayer1(g_Player[0].anim);
		// VDP_SetColor(COLOR_LIGHT_GREEN);
		DrawPlayer2((g_Frame + 4 >> 3) % 13);
		// VDP_SetColor(COLOR_DARK_RED);
		
		//---------------------------------------------------------------------
		// Update input
		
		if(g_Player[0].ctrl)
		{
			g_Player[0].action = ACTION_IDLE;
			keyRow8 = Keyboard_Read(8); // SPACE HOME INS DEL LEFT UP DOWN RIGHT
			if(IS_KEY_PRESSED(keyRow8, KEY_UP))
			{
				g_Player[0].pos.y--;
				g_Player[0].action = ACTION_WALK;
			}
			else if(IS_KEY_PRESSED(keyRow8, KEY_DOWN))
			{
				g_Player[0].pos.y++;
				g_Player[0].action = ACTION_WALK;
			}
			if(IS_KEY_PRESSED(keyRow8, KEY_RIGHT))
			{
				g_Player[0].pos.x++;
				g_Player[0].action = ACTION_WALK;
				g_Player[0].right = true;
			}
			else if(IS_KEY_PRESSED(keyRow8, KEY_LEFT))
			{
				g_Player[0].pos.x--;
				g_Player[0].action = ACTION_WALK;
				g_Player[0].right = false;
			}
			if(IS_KEY_PRESSED(keyRow8, KEY_SPACE) && !(IS_KEY_PRESSED(prevRow8, KEY_SPACE)))
			{
				g_Player[0].action = ACTION_SHOOT;
				g_Player[0].ctrl = false;
				g_Player[0].frame = 0;
			}
			prevRow8 = keyRow8;	
		}
		
		g_Player[1].pos.x++;
	}
}