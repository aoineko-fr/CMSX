//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#pragma once

#include "core.h"

// Draw a text in graphical mode at given position
void DrawText(c8* string, u8 x, u8 y, u8 color, u8 bg, u8 scr);

// Draw a character from the Main-ROM charset in Screen 5
void DrawChar5(u8 character, u8 x, u8 y, u8 bg, u8 color);
// Draw a character from the Main-ROM charset in Screen 8
void DrawChar8(u8 character, u8 x, u8 y, u8 bg, u8 color);
