//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "draw.h"

//-----------------------------------------------------------------------------
///
void Draw_Line(UX x1, UY y1, UX x2, UY y2, u8 color, u8 op)
{
	u16 dx, dy, maj, min;
	u8 arg;

	arg = 0;
	if(x1 > x2)
	{
		arg |= VDP_ARG_DIX_LEFT;
		dx = x1 - x2;
	}
	else // (x1 <= x2)
	{
		arg |= VDP_ARG_DIX_RIGHT;
		dx = x2 - x1;
	}
	if(y1 > y2)
	{
		arg |= VDP_ARG_DIY_UP;
		dy = y1 - y2;
	}
	else // (y1 <= y2)
	{
		arg |= VDP_ARG_DIY_DOWN;
		dy = y2 - y1;
	}
	if(dx > dy)
	{
		arg |= VDP_ARG_MAJ_H;
		maj = dx;
		min = dy;
	}
	else // (dx <= dy)
	{
		arg |= VDP_ARG_MAJ_V;
		maj = dy;
		min = dx;
	}

	VDP_CommandLINE(x1, y1, maj, min, color, arg, op);	
}

//-----------------------------------------------------------------------------
///
void Draw_HLine(UX x1, UX x2, UY y, u8 color, u8 op)
{
	u16 dx, nx;
	if(x1 > x2)
	{
		dx = x2;
		nx = x2 - x1;
	}
	else
	{
		dx = x1;
		nx = x1 - x2;
	}
	VDP_CommandLMMV(dx, y, nx, 1, color, op);	
}

//-----------------------------------------------------------------------------
///
void Draw_VLine(UX x, UY y1, UY y2, u8 color, u8 op)
{
	u16 dy, ny;
	if(y1 > y2)
	{
		dy = y2;
		ny = y2 - y1;
	}
	else
	{
		dy = y1;
		ny = y1 - y2;
	}
	VDP_CommandLMMV(x, dy, 1, ny, color, op);	
}

//-----------------------------------------------------------------------------
///
void Draw_Box(UX x1, UY y1, UX x2, UY y2, u8 color, u8 op)
{
	Draw_HLine(x1, x2, y1, color, op);
	Draw_HLine(x1, x2, y2, color, op);
	Draw_VLine(x1, y1, y2, color, op);
	Draw_VLine(x2, y1, y2, color, op);
}

//-----------------------------------------------------------------------------
///
void Draw_Circle(UX x, UY y, u8 radius, u8 color, u8 op)
{
	
}
