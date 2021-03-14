//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
//║ BIOS ROUTINES WRAPPER                                                     ║
//║                                                                           ║
//║ References:                                                               ║
//║ - http://map.grauw.nl/resources/                                          ║
//║ - https://www.msx.org                                                     ║
//║ - MSX2 Technical Handbook                                                 ║
//║ - Pratique du MSX 2                                                       ║
//╚═══════════════════════════════════════════════════════════════════════════╝
#pragma once

// R_xxxx	Data/routines in Main-ROM
// S_xxxx	Data/routines in Sub-ROM
// M_xxxx	Data/routines in Memory
// H_xxxx	Hooks
// P_xxxx	Ports
// I_xxxx	Interrupts
#include "bios_var.h"
#include "bios_hook.h"
#include "bios_port.h"
#include "bios_mainrom.h"
#include "bios_subrom.h"
#include "bios_discrom.h"

//=============================================================================
// DEFINES
//=============================================================================

// Slot
// ExxxSSPP
// │   ││└┴─ Primary slot number (00-11)
// │   └┴─── Secondary slot number (00-11)
// └──────── Expanded slot (0 = no, 1 = yes)
#define SLOT_0 (0x00)
#define SLOT_1 (0x01)
#define SLOT_2 (0x02)
#define SLOT_3 (0x03)

#define SLOT_EXP (1 << 7)

#define SLOT_0_0 (SLOT_0 | (0x00 << 2) | SLOT_EXP)
#define SLOT_0_1 (SLOT_0 | (0x01 << 2) | SLOT_EXP)
#define SLOT_0_2 (SLOT_0 | (0x02 << 2) | SLOT_EXP)
#define SLOT_0_3 (SLOT_0 | (0x03 << 2) | SLOT_EXP)

#define SLOT_1_0 (SLOT_1 | (0x00 << 2) | SLOT_EXP)
#define SLOT_1_1 (SLOT_1 | (0x01 << 2) | SLOT_EXP)
#define SLOT_1_2 (SLOT_1 | (0x02 << 2) | SLOT_EXP)
#define SLOT_1_3 (SLOT_1 | (0x03 << 2) | SLOT_EXP)

#define SLOT_2_0 (SLOT_2 | (0x00 << 2) | SLOT_EXP)
#define SLOT_2_1 (SLOT_2 | (0x01 << 2) | SLOT_EXP)
#define SLOT_2_2 (SLOT_2 | (0x02 << 2) | SLOT_EXP)
#define SLOT_2_3 (SLOT_2 | (0x03 << 2) | SLOT_EXP)

#define SLOT_3_0 (SLOT_3 | (0x00 << 2) | SLOT_EXP)
#define SLOT_3_1 (SLOT_3 | (0x01 << 2) | SLOT_EXP)
#define SLOT_3_2 (SLOT_3 | (0x02 << 2) | SLOT_EXP)
#define SLOT_3_3 (SLOT_3 | (0x03 << 2) | SLOT_EXP)

#define SLOT(_p) (0x03 & _p)
#define SLOTEX(_p, _s) ((0x03 & _p) | ((0x03 & _s) << 2) | SLOT_EXP)

// Files
#define FILE(str) "\"" str "\""


//-----------------------------------------------------------------------------
// Input Macros

#define KEYBOARD_HOLD(key)    ((g_NEWKEY[KEY_ROW(key)] & KEY_FLAG(key)) == 0)
#define KEYBOARD_PRESS(key)   (((g_NEWKEY[KEY_ROW(key)] & KEY_FLAG(key)) == 0) && ((g_OLDKEY[KEY_ROW(key)] & KEY_FLAG(key)) != 0))
#define KEYBOARD_RELEASE(key) (((g_NEWKEY[KEY_ROW(key)] & KEY_FLAG(key)) != 0) && ((g_OLDKEY[KEY_ROW(key)] & KEY_FLAG(key)) == 0))


//-----------------------------------------------------------------------------
//  █ █ █▀▀ █   █▀█ █▀▀ █▀█   █▀▀ █ █ █▄ █ █▀▀ ▀█▀ █ █▀█ █▄ █ █▀
//  █▀█ ██▄ █▄▄ █▀▀ ██▄ █▀▄   █▀  █▄█ █ ▀█ █▄▄  █  █ █▄█ █ ▀█ ▄█
//-----------------------------------------------------------------------------

// Call a bios function
inline void Bios_MainCall(u16 addr);

/// Handle soft reboot
void Bios_Reboot();

/// Handle clean transition to Basic or MSX-DOS environment
void Bios_Exit(u8 ret) __FASTCALL;

/// Get the slot ID of a given page
u8 Bios_GetSlot(u8 page) __FASTCALL;

