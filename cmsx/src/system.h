//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄                                                        
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀                                                        
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █                                                        
//_____________________________________________________________________________
#pragma once

//=============================================================================
// INCLUDES
//=============================================================================

#include "core.h"
#include "bios.h"

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

//=============================================================================
// FONCTION
//=============================================================================

//-----------------------------------------------------------------------------
// Helper inline functions

/// Direct call a routine at a given address (generate ASM code: "call XXXX")
inline void Call(u16 addr) { ((void(*)(void))(addr))(); }

/// Enable interruption
inline void EnableInterrupt() { __asm__("ei"); }

/// Disable interruption
inline void DisableInterrupt() { __asm__("di"); }

/// Disable interruption
inline void Halt() { __asm__("halt"); }


//-----------------------------------------------------------------------------
// Slot

/// Get the slot ID of a given page
u8 Sys_GetSlot(u8 page);

/// Set the slot ID of a given page
void Sys_SetSlot(u8 page, u8 slot);

/// Set the slot ID of a given page
inline bool Sys_IsSlotExpanded(u8 slot) { return g_EXPTBL[slot] & 0x80; }
