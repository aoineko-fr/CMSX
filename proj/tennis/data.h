//_____________________________________________________________________________
//  ▄▄▄▄ ▄            ▄▄     ▄▄▄                ▄▄  
//  ██▄  ▄  ██▀▄ ▄▀██ ██    ▀█▄  ▄█▄█ ▄▀██  ██▀ ██▄ 
//  ██   ██ ██ █ ▀▄██ ▀█▄   ▄▄█▀ ██ █ ▀▄██ ▄██  ██ █
//
//  ▄▄▄   ▄▄  ▄▄▄▄  ▄▄ 
//  ██ █ ██▄█  ██  ██▄█
//  ██▄▀ ██ █  ██  ██ █
//_____________________________________________________________________________

//=============================================================================
//
//  ROM DATA - PAGE 0
//
//=============================================================================

// Player 1 sprites
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAPLAYER1		0x0200
	#define D_g_DataPlayer1			__at(ADDR_DATAPLAYER1)
#endif
#include "data_player1.h"

// Player 2 Sprites
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAPLAYER2		(ADDR_DATAPLAYER1 + sizeof(g_DataPlayer1))
	#define D_g_DataPlayer2			__at(ADDR_DATAPLAYER2)
#endif
#include "data_player2.h" 

// SFX
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATASFX			(ADDR_DATAPLAYER2 + sizeof(g_DataPlayer2))
	#define D_g_DataSFX				__at(ADDR_DATASFX)
#endif
#include "data_sfx.h"

// Music
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAMUSIC			(ADDR_DATASFX + sizeof(g_DataSFX))
	#define D_g_DataMusic			__at(ADDR_DATAMUSIC)
#endif
#include "data_music.h"

// Menu title ball
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATALOGOBALL		(ADDR_DATAMUSIC + sizeof(g_DataMusic))
	#define D_g_DataLogoBall		__at(ADDR_DATALOGOBALL)
#endif
#include "data_logo_ball.h"

//=============================================================================
//
//  ROM DATA - PAGE 1-2
//
//=============================================================================

// Menu title (GM2 tables)
#include "data_logo.h"

// Court background (GM2 tables)
#include "data_court.h"

// Score board
#include "data_board.h"

// Pentaru referee
#include "data_referee.h"

// Ball launcher sprites
#include "data_launcher0.h"
#include "data_launcher1.h"

// Fonts
#include "data_font.h"

// Score point sprites
#include "data_points.h"
#include "data_event.h"

// Score font
#include "data_scrfont.h"

// Net Sprites
#include "data_net.h" 

// Racket sprites
#include "data_racket.h"
// Ball sprites
#include "data_ball.h"

#include "pt3\pt3_notetable2.h"

// Math
#include "mathtable\mt_trigo_Q10.6_64.inc"
