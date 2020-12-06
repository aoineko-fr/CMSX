//╔═══════════════════════════════════════════════════════════════════════════╗
//║ █▀▀ █▀▄▀█ █▀ ▀▄▀                                                          ║
//║ █▄▄ █ ▀ █ ▄█ █ █ v0.2                                                     ║
//╟───────────────────────────────────────────────────────────────────────────╢
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
#include "bios_func.h"
#include "ports.h"




//─────────────────────────────────────────────────────────────────────────────
// Defines

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

// Screens
#define SCREEN_0	0
#define SCREEN_1	1
#define SCREEN_2	2
#define SCREEN_3	3
#define SCREEN_4	4
#define SCREEN_5	5
#define SCREEN_6	6
#define SCREEN_7	7
#define SCREEN_8	8
#define SCREEN_9	9
#define SCREEN_10	10
#define SCREEN_11	10
#define SCREEN_12	10

// Files
#define FILE(str) "\"" str "\""

// Logical operation

#define LOG_OP_IMP		0x00 // 0000 | DC=SC
#define LOG_OP_AND		0x01 // 0001 | DC=SC&DC
#define LOG_OP_OR		0x02 // 0010 | DC=SC|DC
#define LOG_OP_XOR		0x03 // 0011 | DC=SC^DC
#define LOG_OP_NOT		0x04 // 0100 | DC=!SC
#define LOG_OP_TIMP		0x08 // 1000 | DC = (SC==0) ? DC : SC
#define LOG_OP_TAND		0x09 // 1001 | DC = (SC==0) ? DC : SC&DC
#define LOG_OP_TOR		0x0A // 1010 | DC = (SC==0) ? DC : SC|DC
#define LOG_OP_TXOR		0x0B // 1011 | DC = (SC==0) ? DC : SC^DC
#define LOG_OP_TNOT		0x0C // 1100 | DC = (SC==0) ? DC : !SC
