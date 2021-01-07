//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once
#include "core.h"
#include "vdp.h"

//-----------------------------------------------------------------------------
// DFINES

#if (DRAW_UNIT == DRAW_UNIT_U16)
	#define UX			u16
	#define UY			u16
#elif (DRAW_UNIT == DRAW_UNIT_Y16)
	#define UX			u8
	#define UY			u16
#else // if (DRAW_UNIT == DRAW_UNIT_U8)
	#define UX			u8
	#define UY			u8
#endif

//-----------------------------------------------------------------------------
// FUNCTIONS

/// Draw a line (pixel unit)
void Draw_Line(UX x1, UY y1, UX x2, UY y2, u8 color, u8 op);

/// Draw an horizontal line (pixel unit)
void Draw_HLine(UX x1, UX x2, UY y, u8 color, u8 op);

/// Draw a vertical line (pixel unit)
void Draw_VLine(UX x, UY y1, UY y2, u8 color, u8 op);

/// Draw a box (pixel unit)
void Draw_Box(UX x1, UY y1, UX x2, UY y2, u8 color, u8 op);

/// Draw a circle (pixel unit)
void Draw_Circle(UX x, UY y, u8 radius, u8 color, u8 op);