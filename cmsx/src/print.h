//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "core.h"


inline u8 IsTextScreen();
void PrintInit(u8 color);
void SetPrintPos(u8 x, u8 y);
void PrintReturn();
void PrintTab();
void PrintChar(c8 chr);
void PrintCharX(c8 chr, u8 num);
void PrintText(const c8* str);
void PrintTextX(const c8* str, u8 num);
void PrintHex8(u8 value);
void PrintHex16(u16 value);
void PrintBin8(u8 value);
void PrintInt(i16 value);
void PrintSlot(u8 slot);

void PrintLineX(u8 x, u8 y, u8 len);
void PrintLineY(u8 x, u8 y, u8 len);
void PrintBox(u8 sx, u8 sy, u8 dx, u8 dy);


/*
// Draw a text in graphical mode at given position
void PrintString(const c8* string, u8 x, u8 y, u8 color, u8 scr);

void PrintCharGraph(u8 chr, u8 x, u8 y, u8 color);
void PrintCharText(u8 chr, u8 x, u8 y, u8 color);
*/