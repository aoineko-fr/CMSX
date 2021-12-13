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

//-----------------------------------------------------------------------------
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

#define SLOT(p) (0x03 & p)
#define SLOTEX(p, s) ((0x03 & p) | ((0x03 & s) << 2) | SLOT_EXP)

//-----------------------------------------------------------------------------
// Optimized assmbler bits shift

// Right shift of A register
#define RShift(a)	RShift_##a
// a >> 1
#define RShift_1	srl	a
// a >> 2
#define RShift_2	rrca			\
					rrca			\
					and a, #0x3F
// a >> 3
#define RShift_3	rrca			\
					rrca			\
					rrca			\
					and a, #0x1F	
// a >> 4
#define RShift_4	rlca			\
					rlca			\
					rlca			\
					rlca			\
					and a, #0x0F
// a >> 5
#define RShift_5	rlca			\
					rlca			\
					rlca			\
					and a, #0x07
// a >> 6
#define RShift_6	rlca			\
					rlca			\
					and a, #0x03
// a >> 7
#define RShift_7	rlca			\
					and a, #0x01

// Left shift of A register
#define LShift(a)	LShift_##a
// a << 1
#define LShift_1	add	a, a
// a << 2
#define LShift_2	add	a, a		\
					add	a, a
// a << 3
#define LShift_3	add	a, a		\
					add	a, a		\
					add	a, a
// a << 4
#define LShift_4	add	a, a		\
					add	a, a		\
					add	a, a		\
					add	a, a
// a << 5
#define LShift_5	rrca			\
					rrca			\
					rrca			\
					and a, #0xE0
// a << 6
#define LShift_6	rrca			\
					rrca			\
					and a, #0xC0
// a << 7
#define LShift_7	rrca			\
					and a, #0x80



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
