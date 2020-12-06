//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
/**
 * @file Features to manage the VDP - Inline functions
 *
 * References:
 * - V9938 Technical Data Book Programmer's Guide
 * - https://www.msx.org/wiki/Category:VDP_Registers
 * - http://map.grauw.nl/articles/
 */

//-----------------------------------------------------------------------------
//
// VDP COMMANDS
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * High speed move CPU to VRAM.
 */
inline void VDP_HMMC()
{
}

//-----------------------------------------------------------------------------
/**
 * High speed move VRAM to VRAM, Y coordinate only.
 * @param	sy		Y coordinate of the source area
 * @param	dx,dy	X/Y coordinate of the destination area
 * @param	ny		Y size of the area to move
 * @param	dir		Direction of the move (from the destination)
 */
inline void VDP_YMMM(u16 sy, u16 dx, u16 dy, u16 ny, u8 dir)
{
	g_VDP_Command.SY = sy;
	g_VDP_Command.DX = dx;
	g_VDP_Command.DY = dy;
	g_VDP_Command.NY = ny;
	g_VDP_Command.ARG = dir; 
	g_VDP_Command.CMD = VDP_CMD_YMMM;
	VPD_SendCommand32();
}

//-----------------------------------------------------------------------------
/**
 * High speed move VRAM to VRAM
 */
inline void VDP_HMMM(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny)
{
	g_VDP_Command.SX = sx;
	g_VDP_Command.SY = sy;
	g_VDP_Command.DX = dx;
	g_VDP_Command.DY = dy;
	g_VDP_Command.NX = nx;
	g_VDP_Command.NY = ny;
	g_VDP_Command.ARG = 0; 
	g_VDP_Command.CMD = VDP_CMD_HMMM;
	VPD_SendCommand32();
}

//-----------------------------------------------------------------------------
/**
 * High speed move VDP to VRAM
 */
inline void VDP_HMMV(u16 dx, u16 dy, u16 nx, u16 ny, u8 col)
{
	g_VDP_Command.DX = dx; 
	g_VDP_Command.DY = dy; 
	g_VDP_Command.NX = nx; 
	g_VDP_Command.NY = ny; 
	g_VDP_Command.CLR = col; 
	g_VDP_Command.ARG = 0; 
	g_VDP_Command.CMD = VDP_CMD_HMMV;
	VPD_SendCommand36();
}

//-----------------------------------------------------------------------------
/**
 * Logical move CPU to VRAM
 */
inline void VDP_LMMC()
{
}

//-----------------------------------------------------------------------------
/**
 * Logical move VRAM to CPU
 */
inline void VDP_LMCM()
{
}

//-----------------------------------------------------------------------------
/**
 * Logical move VRAM to VRAM
 */
inline void VDP_LMMM(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny, u8 op)
{
	g_VDP_Command.SX = sx;
	g_VDP_Command.SY = sy;
	g_VDP_Command.DX = dx;
	g_VDP_Command.DY = dy;
	g_VDP_Command.NX = nx;
	g_VDP_Command.NY = ny;
	g_VDP_Command.ARG = 0; 
	g_VDP_Command.CMD = VDP_CMD_LMMM + op;
	VPD_SendCommand32();
}

//-----------------------------------------------------------------------------
/**
 * Logical move VDP to VRAM
 */
inline void VDP_LMMV(u16 dx, u16 dy, u16 nx, u16 ny, u8 col, u8 op)
{
	g_VDP_Command.DX = dx; 
	g_VDP_Command.DY = dy; 
	g_VDP_Command.NX = nx; 
	g_VDP_Command.NY = ny; 
	g_VDP_Command.CLR = col; 
	g_VDP_Command.ARG = 0; 
	g_VDP_Command.CMD = VDP_CMD_LMMV + op;
	VPD_SendCommand36();
}

//-----------------------------------------------------------------------------
/**
 * Draw straight line in VRAM
 */
inline void VDP_LINE()
{
}

//-----------------------------------------------------------------------------
/**
 * Search for the specific color in VRAM to the right or left of the starting point
 */
inline void VDP_SRCH()
{
}

//-----------------------------------------------------------------------------
/**
 * Draw a dot in VRAM 
 */
inline void VDP_PSET()
{
}

//-----------------------------------------------------------------------------
/**
 * Read the color of the specified dot located in VRAM 
 */
inline void VDP_POINT()
{
}

//-----------------------------------------------------------------------------
/**
 * Abort current command
 */
inline void VDP_STOP()
{
	VDP_RegWrite(46, VDP_CMD_STOP);
}

// #define VDP_CopyRAMtoVRAM			VDP_HMMC	///< High speed move CPU to VRAM
// #define VDP_YMoveVRAM				VDP_YMMM	///< High speed move VRAM to VRAM, Y coordinate only
// #define VDP_MoveVRAM				VDP_HMMM	///< High speed move VRAM to VRAM
// #define VDP_FillVRAM				VDP_HMMV	///< High speed move VDP to VRAM
// #define VDP_LogicalCopyRAMtoVRAM	VDP_LMMC	///< Logical move CPU to VRAM
// #define VDP_LogicalYMoveVRAM		VDP_LMCM	///< Logical move VRAM to CPU
// #define VDP_LogicalMoveVRAM			VDP_LMMM	///< Logical move VRAM to VRAM
// #define VDP_LogicalFillVRAM			VDP_LMMV	///< Logical move VDP to VRAM
// #define VDP_DrawLine				VDP_LINE	///< Draw straight line in VRAM
// #define VDP_SearchColor				VDP_SRCH	///< Search for the specific color in VRAM to the right or left of the starting point
// #define VDP_DrawPoint				VDP_PSET	///< Draw a dot in VRAM 
// #define VDP_ReadPoint				VDP_POINT	///< Read the color of the specified dot located in VRAM 
// #define VDP_AbortCommand			VDP_STOP	///< Abort current command
