//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "bios_main.h"
#include "vdp.h"
#include "print.h"

static struct PrintConfig g_PrintConfig;

//-----------------------------------------------------------------------------
//
// INITIALIZATION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initialize print module
bool Print_Initialize(u8 screen, const u8* font)
{
	Print_SetFont(font);

	g_PrintConfig.Mode = PRINT_MODE_GRAPH;
	g_PrintConfig.Page = 0;
	g_PrintConfig.Bpp = 4;
	g_PrintConfig.UnitX = 8;
	g_PrintConfig.UnitY = 8;
	g_PrintConfig.TextColor = 0xFF;
	g_PrintConfig.BackgroundColor = 0;
	
	switch(screen)
	{
	case 0:
	case 1:
		g_PrintConfig.Mode = PRINT_MODE_TEXT;
		g_PrintConfig.UnitX = 1;
		g_PrintConfig.UnitY = 1;
		break;
	
	case 6:
		g_PrintConfig.Bpp = 1;
		break;
	
	case 8:
		g_PrintConfig.Bpp = 8;
		break;
	case 9:
	case 10:
	case 11:
	case 12:
		/// @todo Implement
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
/// Set the current font
void Print_SetFont(const u8* font) __FASTCALL
{
	if(font == null) // Use Bios font (if any)
	{
		g_PrintConfig.FontSize  = 0x88;
		g_PrintConfig.FontFirst = 0;
		g_PrintConfig.FontLast  = 255;
		g_PrintConfig.FontForms = (const u8*)g_CGTABL;
	}
	else
	{
		g_PrintConfig.FontSize  = *font++;
		g_PrintConfig.FontFirst = *font++;
		g_PrintConfig.FontLast  = *font++;
		g_PrintConfig.FontForms = font;
	}
}

//-----------------------------------------------------------------------------
/// Set the draw color
void Print_SetColor(u8 text, u8 background)
{
	g_PrintConfig.TextColor = text;
	g_PrintConfig.BackgroundColor = background;
}


//-----------------------------------------------------------------------------
//
// SYSTEM FUNCTION
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Draw a character in a 4-bits color mode
void DrawChar4B(u8 character, u8 x, u8 y, u8 color, u8 bg)
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
// #if (RENDER_MODE == RENDER_BIOS)
		// Bios_TransfertRAMtoVRAM((u16)line, ((y + j) * 128) + (x >> 1), 4 + of);
// #elif (RENDER_MODE == RENDER_VDP)
		// VDP_HMMC((x >> 1), y + j, 4 + of, 1, (u16)line);
// #endif
	}
}


//-----------------------------------------------------------------------------
//
// DRAW FUNCTION
//
//-----------------------------------------------------------------------------













//-----------------------------------------------------------------------------
//
void Print_DrawText(u8 x, u8 y, const c8* string)
{
}


static const c8 hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

inline u8 IsTextScreen() { return (g_SCRMOD == 0) || (g_SCRMOD == 1) || (g_SCRMOD == 2); }

void PrintInit(u8 color)
{
	g_LOGOPR = LOG_OP_TIMP;
	g_FORCLR = color;
}

void SetPrintPos(u8 x, u8 y)
{
	if(IsTextScreen())
	{
		g_CSRX = x;
		g_CSRY = y;
	}
	else
	{
		g_GRPACX = x;
		g_GRPACY = y;
	}
}

void PrintReturn()
{
	if(IsTextScreen())
	{
		g_CSRX = 1;
		g_CSRY++;
	}
	else
	{
		g_GRPACX = 0;
		g_GRPACY += 8;
	}
}

void PrintTab()
{
	if(IsTextScreen())
	{
		g_CSRX += 2;
	}
	else
	{
		g_GRPACX += 2 * 8;
	}
}

void PrintChar(c8 chr)
{
	if(IsTextScreen())
		Bios_TextPrintChar(chr);	
	else
		Bios_GraphPrintChar(chr);	
}

void PrintCharX(c8 chr, u8 num)
{
	for(i8 i = 0; i < num; i++)
		PrintChar(chr);
}

void PrintTextX(const c8* str, u8 num)
{
	for(i8 i = 0; i < num; i++)
		PrintText(str);	
}

void PrintText(const c8* str)
{
	while(*str != 0)
	{
		if(*str == '\n')
		{
			PrintReturn();
		}
		else if(*str == '\t')
		{
			PrintTab();
		}
		else
			PrintChar(*str);
		str++;
	}
}

void PrintBin8(u8 value)
{
	for(i8 i = 0; i < 8; i++)
	{
		if(value & (1 << (7 - i)))
			PrintChar('1');
		else
			PrintChar('0');
	}
	PrintChar('b');
}

void PrintHex8(u8 value)
{
	PrintChar(hexChar[(value >> 4) & 0x000F]);
	PrintChar(hexChar[value & 0x000F]);
	PrintChar('h');
}

void PrintHex16(u16 value)
{
	PrintChar(hexChar[(value >> 12) & 0x000F]);
	PrintChar(hexChar[(value >> 8) & 0x000F]);
	PrintHex8((u8)value);
}

void PrintInt(i16 value)
{
	if(value < 0)
	{	
		PrintChar('-');
		value = -value;
	}
	
	c8 str[8];
	c8* ptr = str;
	*ptr = 0;
	while(value >= 10)
	{
		*++ptr = '0' + (value % 10);
		value /= 10;
	}
	*++ptr = '0' + value;
	while(*ptr != 0)
		PrintChar(*ptr--);	
}

void PrintSlot(u8 slot)
{
	PrintInt(slot & 0x03);
	if(slot & 0x80)
	{
		PrintChar('-');	
		PrintInt((slot >> 2) & 0x03);
	}
}

void PrintLineX(u8 x, u8 y, u8 len)
{
	SetPrintPos(x, y);
	for(i8 i = 0; i < len; i++)
	{
		PrintChar(0x01); // graph
		PrintChar(0x57); // -
	}
}

void PrintLineY(u8 x, u8 y, u8 len)
{
	for(i8 i = 0; i < len; i++)
	{
		SetPrintPos(x, y + i);
		PrintChar(0x01); // graph
		PrintChar(0x56); // |
	}
}

void PrintBox(u8 sx, u8 sy, u8 dx, u8 dy)
{
	// Corner
	SetPrintPos(sx, sy);
	PrintChar(0x01); // graph
	PrintChar(0x58);
	SetPrintPos(dx, sy);
	PrintChar(0x01); // graph
	PrintChar(0x59);
	SetPrintPos(sx, dy);
	PrintChar(0x01); // graph
	PrintChar(0x5A);
	SetPrintPos(dx, dy);
	PrintChar(0x01); // graph
	PrintChar(0x5B);

	PrintLineX(sx + 1, sy, dx - sx - 1);
	PrintLineX(sx + 1, dy, dx - sx - 1);

	PrintLineY(sx, sy + 1, dy - sy - 1);
	PrintLineY(dx, sy + 1, dy - sy - 1);
}


/*
typedef void (*DrawCharaCallback)(u8, u8, u8, u8);

void PrintCharGraph(u8 chr, u8 x, u8 y, u8 color)
{
	g_GRPACX = x * 8;
	g_GRPACY = y * 8;
	Bios_TextPrintChar(chr);	
}

void PrintCharText(u8 chr, u8 x, u8 y, u8 color)
{
	g_CSRX = x;
	g_CSRY = y;
	Bios_TextPrintChar(chr);	
}

	
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
void PrintString(const c8* string, u8 x, u8 y, u8 color, u8 scr)
{
	DrawCharaCallback cb;
	if((src == 0) || (src == 2))
		cb = PrintCharText;
	else
		cb = PrintCharGraph;
		
	g_LOGOPR = LOG_OP_TIMP;
	g_FORCLR = color;

	u8 px = x;
	u8 py = y;
	for(i8 i = 0; string[i] != '\0'; i++)
	{
		if(string[i] == '\n')
		{
			px = x;
			py++;
		}
		else if(string[i] == '\t')
		{
			px += 3;
		}
		else
		{
			cb(string[i], px, py, color);
			px++;
		}
	}
}*/