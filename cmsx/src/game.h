//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
// Application helper functions
//_____________________________________________________________________________
#pragma once
#include "core.h"

//-----------------------------------------------------------------------------
//   G A M E
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FUNCTIONS

/// Initialize game module
void Game_Initialize();

/// Update game frame
void Game_Update();

/// Release game module
void Game_Release();


//-----------------------------------------------------------------------------
//   G A M E   L O O P
//-----------------------------------------------------------------------------
#if (USE_GAME_LOOP)

//-----------------------------------------------------------------------------
// DATA RAM

extern bool g_Exit;

//-----------------------------------------------------------------------------
// FUNCTIONS

/// Game main loop
void Game_MainLoop();

#endif


//-----------------------------------------------------------------------------
//   G A M E   S T A T E
//-----------------------------------------------------------------------------
#if (USE_GAME_STATE)

//-----------------------------------------------------------------------------
// DFINES

typedef struct
{
	callback Update;	// Function to be called on state update (mandatory: CAN'T be NULL)
#if (USE_GAME_STATE_TRANSITION)
	callback Start;		// Function to be called on state start (optional: can be NULL)
	callback Finish;	// Function to be called on state finish (optional: can be NULL)
#endif
} State;

//-----------------------------------------------------------------------------
// FUNCTIONS

/// Set the next state (change will be effective at the next state update)
void Game_SetState(State* newState) __FASTCALL;
/// Check state transition and update current state
void Game_UpdateState();

#endif // (USE_GAME_STATE)


//-----------------------------------------------------------------------------
//   G A M E   V - S Y N C
//-----------------------------------------------------------------------------
#if (USE_GAME_VSYNC)

//-----------------------------------------------------------------------------
// DATA RAM

extern u8 g_Frame;

//-----------------------------------------------------------------------------
// FUNCTIONS

/// Vertical-synchronization hook handler
void Game_VSyncHook();

/// Set V-Sync callback
void Game_SetVSyncCallback(callback cb) __FASTCALL;

/// Wait for vertical-synchronization 
void Game_WaitVSync();

#endif // (USE_GAME_VSYNC)

