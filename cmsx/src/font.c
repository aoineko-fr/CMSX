//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "bios_main.h"
#include "video.h"
#include "font.h"

#if (RENDER_MODE == RENDER_VDP)
extern VDP __at(0xC000) vdp;
#endif

typedef void (*DrawCharaCallback)(u8, u8, u8, u8, u8);
	
//-----------------------------------------------------------------------------
// Draw a character from the Main-ROM charset in Screen 5
void DrawChar5(u8 character, u8 x, u8 y, u8 color, u8 bg)
{
	u8* font = (u8*)(g_CGTABL + 8 * character);
	i8 i, j;
	u8 of = x & 0x01; // offset
		
	u8 line[5];
	for(j = 0; j < 8; j++) // lines
	{
		line[0] = line[1] = line[2] = line[3] = line[4] = (bg << 4) | bg;
		for(i = 0; i < 8; i++) // columns
		{
			if(font[j] & (1 << (7 - i)))
			{
				if((i + of) & 0x01)
				{
					line[(i + of) >> 1] &= 0xF0;
					line[(i + of) >> 1] |= color;
				}
				else
				{
					line[(i + of) >> 1] &= 0x0F;
					line[(i + of) >> 1] |= color << 4;
				}
			}
		}		
#if (RENDER_MODE == RENDER_BIOS)
		Bios_TransfertRAMtoVRAM((u16)line, ((y + j) * 128) + (x >> 1), 4 + of);
#elif (RENDER_MODE == RENDER_VDP)
		HMMC((x >> 1), y + j, 4 + of, 1, (u16)line);
#endif
	}
}

//-----------------------------------------------------------------------------
// Draw a character from the Main-ROM charset in Screen 8
void DrawChar8(u8 character, u8 x, u8 y, u8 color, u8 bg)
{
	u8* font = (u8*)(g_CGTABL + 8 * character);
	i8 i, j;
	u8 line[8];
	for(j = 0; j < 8; j++) // lines
	{
		for(i = 0; i < 8; i++) // columns
		{
			line[i] = (font[j] & (1 << (7 - i))) ? color : bg;
		}		
#if (RENDER_MODE == RENDER_BIOS)
		Bios_TransfertRAMtoVRAM((u16)line, ((y + j) * 256) + x, 8);
#elif (RENDER_MODE == RENDER_VDP)
		HMMC(x, y + j, 8, 1, (u16)line); 
#endif
	}
}

//-----------------------------------------------------------------------------
// Draw a text in graphical mode at given position
void DrawText(c8* string, u8 x, u8 y, u8 color, u8 bg, u8 scr)
{
	DrawCharaCallback cb;
	switch(scr)
	{
		case 5:  cb = DrawChar5; break;
		case 6:  return;
		case 7:  return;
		case 8:  cb = DrawChar8; break;
		case 9:  return;
		case 10: return;
		case 11: return;
		case 12: return;
		default: return;
	}

	u8 px = x;
	u8 py = y;
	for(i8 i = 0; string[i] != '\0'; i++)
	{
		if(string[i] == '\n')
		{
			px = x;
			py += 8;
		}
		else if(string[i] == '\t')
		{
			px += 8 * 3;
		}
		else
		{
			cb(string[i], px, py, color, bg);
			px += 8;
		}
	}
}