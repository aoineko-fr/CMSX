//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
// Application helper functions
//_____________________________________________________________________________
#include "game.h"
#include "vdp.h"
#include "bios_hook.h"

//=============================================================================
//
//   G A M E
//
//=============================================================================

//-----------------------------------------------------------------------------
/// Initialize game module
void Game_Initialize()
{
	#if (USE_GAME_VSYNC)
		VDP_EnableVBlank(true);
		Bios_SetHookCallback(H_TIMI, Game_VSyncHook);
	#endif
}

//-----------------------------------------------------------------------------
/// Update game frame
void Game_Update()
{
	#if (USE_GAME_VSYNC)
		Game_WaitVSync();
	#endif
	#if (USE_GAME_STATE)
		Game_UpdateState();
	#endif	
}

//-----------------------------------------------------------------------------
/// Release game module
void Game_Release()
{
	#if (USE_GAME_VSYNC)
		Bios_ClearHook(H_TIMI);
	#endif
}


//=============================================================================
//
//   G A M E   L O O P
//
//=============================================================================

//-----------------------------------------------------------------------------
// RAM DATA

bool g_Exit = false;

//-----------------------------------------------------------------------------
// FUNCTIONS

//-----------------------------------------------------------------------------
/// Game main loop
void Game_MainLoop()
{
	Game_Initialize();
	while(!g_Exit)
		Game_Update();
	Game_Release();
}


//=============================================================================
//
//   G A M E   S T A T E
//
//=============================================================================
#if (USE_GAME_STATE)

//-----------------------------------------------------------------------------
// RAM DATA

State g_State = null;
State g_PrevState = null;

//-----------------------------------------------------------------------------
// FUNCTIONS

//-----------------------------------------------------------------------------
/// Set the next state (change will be effective at the next state update)
/// @param		newState	The new state to start (can be NULL to desactivate state-machine)
void Game_SetState(State newState) __FASTCALL
{
	g_PrevState = g_State;
	g_State = newState;
}

//-----------------------------------------------------------------------------
/// Restore the previous state
void Game_RestoreState()
{
	State prev = g_PrevState;
	g_PrevState = g_State;
	g_State = prev;
}

//-----------------------------------------------------------------------------
/// Check state transition and update current state
void Game_UpdateState()
{
	bool bFinish = false;
	while((g_State != null) && !bFinish)
	{
		bFinish = g_State();
	}	
}

#endif // (USE_GAME_STATE)


//=============================================================================
//
//   G A M E   V - S Y N C H
//
//=============================================================================
#if (USE_GAME_VSYNC)

void Game_DefaultVSyncCB();

//-----------------------------------------------------------------------------
// RAM DATA

bool     g_VSync = false;
u8       g_Frame = 0;
callback g_VSyncCB = Game_DefaultVSyncCB;

//-----------------------------------------------------------------------------
// FUNCTIONS

//-----------------------------------------------------------------------------
/// Default V-Sync callback
void Game_DefaultVSyncCB() {}

//-----------------------------------------------------------------------------
/// Set V-Sync callback
void Game_SetVSyncCallback(callback cb) __FASTCALL
{
	g_VSyncCB = cb;
}

//-----------------------------------------------------------------------------
/// Vertical-synchronization hook handler
void Game_VSyncHook()
{
	g_VSync = true;
	g_VSyncCB();
}

//-----------------------------------------------------------------------------
/// Wait for vertical-synchronization 
void Game_WaitVSync()
{
	while(g_VSync == false) {}
	g_VSync = false;
	g_Frame++;
}

#endif // (USE_GAME_VSYNC)