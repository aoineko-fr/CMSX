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
	#define ADDR_DATALAUNCHER0		(ADDR_DATAPLAYER1 + sizeof(g_DataPlayer1))
	#define D_g_DataLauncher0		__at(ADDR_DATALAUNCHER0)
#endif
#include "data_launcher0.h"
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATALAUNCHER1		(ADDR_DATALAUNCHER0 + sizeof(g_DataLauncher0))
	#define D_g_DataLauncher1		__at(ADDR_DATALAUNCHER1)
#endif
#include "data_launcher1.h"

// Score board
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATASCORE_NAMES	(ADDR_DATALAUNCHER1 + sizeof(g_DataLauncher1))
	#define ADDR_DATASCOREL1_NAMES	(ADDR_DATASCORE_NAMES + sizeof(g_DataScore_Names))
	#define ADDR_DATASCOREL2_NAMES	(ADDR_DATASCOREL1_NAMES + sizeof(g_DataScoreL1_Names))
	#define ADDR_DATASCOREL3_NAMES	(ADDR_DATASCOREL2_NAMES + sizeof(g_DataScoreL2_Names))
	#define ADDR_DATASCOREL4_NAMES	(ADDR_DATASCOREL3_NAMES + sizeof(g_DataScoreL3_Names))
	#define ADDR_DATASCOREL5_NAMES	(ADDR_DATASCOREL4_NAMES + sizeof(g_DataScoreL4_Names))
	#define ADDR_DATASCORE_PATTERNS	(ADDR_DATASCOREL5_NAMES + sizeof(g_DataScoreL5_Names))
	#define ADDR_DATASCORE_COLORS	(ADDR_DATASCORE_PATTERNS + sizeof(g_DataScore_Patterns))
	#define D_g_DataScore_Names		__at(ADDR_DATASCORE_NAMES)
	#define D_g_DataScoreL1_Names   __at(ADDR_DATASCOREL1_NAMES)
	#define D_g_DataScoreL2_Names   __at(ADDR_DATASCOREL2_NAMES)
	#define D_g_DataScoreL3_Names   __at(ADDR_DATASCOREL3_NAMES)
	#define D_g_DataScoreL4_Names   __at(ADDR_DATASCOREL4_NAMES)
	#define D_g_DataScoreL5_Names   __at(ADDR_DATASCOREL5_NAMES)
	#define D_g_DataScore_Patterns	__at(ADDR_DATASCORE_PATTERNS)
	#define D_g_DataScore_Colors	__at(ADDR_DATASCORE_COLORS)
#endif
#include "data_board.h"

// Pentaru referee
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAREFEREE_NAMES		(ADDR_DATASCORE_COLORS + sizeof(g_DataScore_Colors))
	#define ADDR_DATAREFEREE_PATTERNS	(ADDR_DATAREFEREE_NAMES + sizeof(g_DataReferee_Names))
	#define ADDR_DATAREFEREE_COLORS		(ADDR_DATAREFEREE_PATTERNS + sizeof(g_DataReferee_Patterns))
	#define D_g_DataReferee_Names		__at(ADDR_DATAREFEREE_NAMES)
	#define D_g_DataReferee_Patterns	__at(ADDR_DATAREFEREE_PATTERNS)
	#define D_g_DataReferee_Colors		__at(ADDR_DATAREFEREE_COLORS)
#endif
#include "data_referee.h"

// SFX
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATASFX			(ADDR_DATAREFEREE_COLORS + sizeof(g_DataReferee_Colors))
	#define D_g_DataSFX				__at(ADDR_DATASFX)
#endif
#include "data_sfx.h"

// Music
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAMUSIC			(ADDR_DATASFX + sizeof(g_DataSFX))
	#define D_g_DataMusic			__at(ADDR_DATAMUSIC)
#endif
#include "data_music.h"

// Fonts
#if (TARGET_TYPE == TARGET_TYPE_ROM)
	#define ADDR_DATAFONT			(ADDR_DATAMUSIC + sizeof(g_DataMusic))
	#define D_g_DataFont			__at(ADDR_DATAFONT)
#endif
#include "data_font.h"

//=============================================================================
//
//  ROM DATA - PAGE 1-2
//
//=============================================================================

#include "data_scrfont.h"

// Net Sprites
#include "data_net.h" 


// Player 2 Sprites
#include "data_player2.h" 
// Racket sprites
#include "data_racket.h"
// Score sprites
#include "data_score.h"
// Ball sprites
#include "data_ball.h"

#include "pt3\pt3_notetable2.h"

// Math
#include "mathtable\mt_trigo_Q10.6_64.inc"
