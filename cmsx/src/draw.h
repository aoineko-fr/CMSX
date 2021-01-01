//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once
#include "core.h"
#include "vdp.h"



#if (DRAW_UNIT == DRAW_UNIT_Y16)
	#define UX			u8
	#define UY			u16
#else // if (DRAW_UNIT == DRAW_UNIT_DEFAULT)
	#define UX			u8
	#define UY			u8
#endif

///
void Draw_Line(UX x1, UY y1, UX x2, UY y2, u8 color, u8 op);

///
void Draw_HLine(UX x1, UX x2, UY y, u8 color, u8 op);

///
void Draw_VLine(UX x, UY y1, UY y2, u8 color, u8 op);

///
void Draw_Box(UX x1, UY y1, UX x2, UY y2, u8 color, u8 op);

///
void Draw_Circle(UX x, UY y, u8 radius, u8 color, u8 op);