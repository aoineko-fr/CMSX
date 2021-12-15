// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
//  System module sample

//=============================================================================
// INCLUDES
//=============================================================================
#include "cmsx.h"

//=============================================================================
// DEFINES
//=============================================================================
#define SLOT_Y 3

#define DOT_CHAR ((g_ROMVersion.CharacterSet == 0) ? 0xA5 : 0x07)

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Character animation data
const c8 g_ChrAnim[] = { '|', '\\', '-', '/' };

// Slot box
const u8 g_SlotTop[]	= { 0x18, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x19, 0 }; 
const u8 g_SlotMid[]	= { 0x16, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x16, 0 }; 
const u8 g_SlotBot[]	= { 0x1A, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x1B, 0 }; 
// Expended slot box
const u8 g_SlotExTop[]	= { 0x18, 0x17, 0x19, 0 }; 
const u8 g_SlotExMid[]	= { 0x16, 0x20, 0x16, 0 }; 
const u8 g_SlotExBot[]	= { 0x1A, 0x17, 0x1B, 0 }; 

//=============================================================================
// MEMORY DATA
//=============================================================================
u8 g_VDP;

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
//
void Print_DrawSlot(u8 slot)
{
	Print_DrawInt(Sys_SlotGetPrimary(slot));
	if(Sys_SlotIsExpended(slot))
	{
		Print_DrawChar('-');
		Print_DrawInt(Sys_SlotGetSecondary(slot));
	}
}

/*
//-----------------------------------------------------------------------------
//
void InstallCustomVBlankInterrupt()
{
	__asm
		ORG     38H

		push    AF
		ex      AF, AF'
		push    AF

		in      A, (99H)
		and     A
		jp      P, _D02

		push    HL
		push    DE
		push    BC
		exx
		push    HL
		push    DE
		push    BC
		push    IY
		push    IX

		call    H_TIMI

		pop     IX
		pop     IY
		pop     BC
		pop     DE
		pop     HL
		exx
		pop     BC
		pop     DE
		pop     HL
		
_D02:		
		pop     AF
		ex      AF, AF'
		pop     AF
		
		ei
		ret
	
	__endasm;	
}*/

//-----------------------------------------------------------------------------
//
void InterSlotWritePage3(u8 slot, u16 addr, u8 value) __sdcccall(0)
{
	slot, addr, value;
	__asm
		di
		push	ix
		ld		ix, #0
		add		ix, sp
		ld		d, 4(ix)
		ld		l, 5(ix)
		ld		h, 6(ix)
		ld		a, 7(ix)
		ld		i, a
		// Backup
		ld		a, (0xFFFF)
		cpl
		ld		c, a			// Save current secondary slot in C
		in		a, (0xA8)
		ld		b, a			// Save current primary slot in B

		// Switch slot
		ld		a, d
		rrca
		rrca
		and		#0xC0
		ld		e, a
		ld		a, b
		and		#0x3F
		or		e				// remplace ?? par le slot primaire (00,01,10 ou 11)
		out		(0xA8), a

		ld		a, d
		rlca
		rlca
		rlca
		rlca
		and		#0xC0
		ld		e, a
		ld		a, c
		and		#0x3F
		or		e				// remplace ?? par le slot secondaire (00,01,10 ou 11)
		ld		(0xFFFF), a
		// Write
		ld		a, i
		ld		(hl), a

		// Restore		
		ld		a, b
		out		(0xA8), a
		ld		a, c
		ld		(0xFFFF), a

		pop		ix
		ei
	__endasm;
}

//-----------------------------------------------------------------------------
//
u8 InterSlotReadPage3(u8 slot, u16 addr) __sdcccall(0)
{
	slot, addr;
	__asm
		di
		push	ix
		ld		ix, #0
		add		ix, sp
		ld		d, 4(ix)
		ld		l, 5(ix)
		ld		h, 6(ix)
		// Backup
		ld		a, (0xFFFF)
		cpl
		ld		c, a			// Save current secondary slot in C
		in		a, (0xA8)
		ld		b, a			// Save current primary slot in B

		// Switch slot
		ld		a, d
		rrca
		rrca
		and		#0xC0
		ld		e, a
		ld		a, b
		and		#0x3F
		or		e				// remplace ?? par le slot primaire (00,01,10 ou 11)
		out		(0xA8), a

		ld		a, d
		rlca
		rlca
		rlca
		rlca
		and		#0xC0
		ld		e, a
		ld		a, c
		and		#0x3F
		or		e				// remplace ?? par le slot secondaire (00,01,10 ou 11)
		ld		(0xFFFF), a
		// Read
		ld		a, (hl)
		ld		l, a // return value

		// Restore		
		ld		a, b
		out		(0xA8), a
		ld		a, c
		ld		(0xFFFF), a

		pop		ix
		ei
	__endasm;
}

//-----------------------------------------------------------------------------
//
u8 IsSlotPageRAM(u8 slotId, u8 page)
{
	u16 addr = page * 0x4000;
	if(page < 3)
	{
		u8 data = Bios_InterSlotRead(slotId, addr);

		Bios_InterSlotWrite(slotId, addr, ~data);
		
		if(Bios_InterSlotRead(slotId, addr) == data)
			return 0;

		Bios_InterSlotWrite(slotId, addr, data);
	}
	else
	{
		u8 data = InterSlotReadPage3(slotId, addr);

		InterSlotWritePage3(slotId, addr, ~data);
		
		if(InterSlotReadPage3(slotId, addr) == data)
			return 0;

		InterSlotWritePage3(slotId, addr, data);
	}
	return 1;
}

//-----------------------------------------------------------------------------
//
const c8* GetSlotName(u8 slotId, u8 page)
{
	if((slotId == g_EXPTBL[0]) && (page == 0))
		return "M-R";

	if((slotId == g_EXPTBL[0]) && (page == 1))
		return "BAS";

	if((slotId == g_EXBRSA) && (page == 0))
		return "SUB";

	if((slotId == g_MASTER) && (page == 1))
		return "DSK";
		
	if(IsSlotPageRAM(slotId, page))
		return "RAM";
	
	u8 prim = slotId & 0x03;
	u8 sec = (slotId >> 2) & 0x03;
	u16 addr = M_SLTATR + 16 * prim + 4 * sec + page;
	u8 app = *((u8*)addr);
	if(app & 0x80)
		return " B ";
	if(app & 0x40)
		return " D ";
	if(app & 0x20)
		return " S ";
	
	return " ? ";
}

//-----------------------------------------------------------------------------
//
void DisplayHeader()
{
	Print_Clear();
	Print_SetPosition(0, 0);
	Print_DrawText("MGL - SYSTEM INFORMATION");
	Print_DrawLineH(0, 1, 40);
}

//-----------------------------------------------------------------------------
//
void DisplayFooter()
{
	Print_SetPosition(0, 23);
	Print_DrawText("F1:Info F2:Slot ESC:Exit");
}

//-----------------------------------------------------------------------------
//
void DisplayInfo()
{
	DisplayHeader();

	Print_DrawLineV(19, 2, 20);

	//-------------------------------------------------------------------------
	Print_SetPosition(0, 3);
	Print_DrawText("System");
	// MSX version
	Print_DrawText("\n- Version: ");
	switch(g_MSXVER)
	{
		case 0: Print_DrawText("MSX 1"); break;
		case 1: Print_DrawText("MSX 2"); break;
		case 2: Print_DrawText("MSX 2+"); break;
		case 3: Print_DrawText("MSX TurboR"); break;
		default: Print_DrawText("Unknow"); break;
	}
	Print_DrawText("\n- Font:    ");
	switch(g_ROMVersion.CharacterSet)
	{
		case 0: Print_DrawText("Jap"); break;
		case 1: Print_DrawText("Int"); break;
		case 2: Print_DrawText("Rus"); break;
		default: Print_DrawText("Unknow"); break;
	}
	Print_DrawText("\n- Keyb:    ");
	switch(g_ROMVersion.KeyboardType)
	{
		case 0: Print_DrawText("Jap"); break;
		case 1: Print_DrawText("Int"); break;
		case 2: Print_DrawText("Fra"); break;
		case 3: Print_DrawText("UK"); break;
		case 4: Print_DrawText("Ger"); break;
		case 5: Print_DrawText("USSR"); break;
		case 6: Print_DrawText("Spain"); break;
		default: Print_DrawText("Unknow"); break;
	}
	
	//-------------------------------------------------------------------------
	// VDP version
	Print_DrawText("\n\nVideo");
	Print_DrawText("\n- VDP:     ");
	switch(g_VDP)
	{
		case 0: Print_DrawText("TMS9918A"); break;
		case 1: Print_DrawText("V9938"); break;
		case 2: Print_DrawText("V9958"); break;
		default: Print_DrawText("Unknow"); break;
	}
	// VDP frequency
	Print_DrawText("\n- Freq:    ");
	switch(g_ROMVersion.VSF)
	{
		case 0: Print_DrawText("60 Hz"); break;
		case 1: Print_DrawText("50 Hz"); break;
	}
	// VRAM size
	Print_DrawText("\n- VRAM:    ");
	switch(GET_VRAM_SIZE())
	{
		case 0: Print_DrawText("16 KB"); break;
		case 1: Print_DrawText("64 KB"); break;
		case 2: Print_DrawText("128 KB"); break;
		case 3: Print_DrawText("192 KB"); break;
	}
	// VRAM size
	Print_DrawText("\n- IO R/W:  ");
	Print_DrawHex8(g_VDP_DR);
	Print_DrawChar('/');
	Print_DrawHex8(g_VDP_DW);
	
	//-------------------------------------------------------------------------
	// Memory
	Print_DrawText("\n\nMemory");
	Print_DrawText("\n- Heap:    ");
	Print_DrawHex16(Mem_GetHeapAddress());
	Print_DrawText("\n- Stack:   ");
	Print_DrawHex16(Mem_GetStackAddress());
	Print_DrawText("\n- Free:    ");
	Print_DrawInt(Mem_GetHeapSize());
	Print_DrawText(" B");
	
	//-------------------------------------------------------------------------
	// Slots
	u8 X = 21;
	u8 Y = 3;
	Print_DrawTextAt(X, Y++, "Current Slots");
	Print_DrawTextAt(X, Y++, "- Page #0:  ");
	Print_DrawSlot(Sys_GetPageSlot(0));
	Print_DrawTextAt(X, Y++, "- Page #1:  ");
	Print_DrawSlot(Sys_GetPageSlot(1));
	Print_DrawTextAt(X, Y++, "- Page #2:  ");
	Print_DrawSlot(Sys_GetPageSlot(2));
	Print_DrawTextAt(X, Y++, "- Page #3:  ");
	Print_DrawSlot(Sys_GetPageSlot(3));

	//-------------------------------------------------------------------------
	// ROMs
	Y++;
	Print_DrawTextAt(X, Y++, "ROM");
	Print_DrawTextAt(X, Y++, "- Main:     ");
	Print_DrawSlot(g_MNROM);
	Print_DrawTextAt(X, Y++, "- Sub:      ");
	Print_DrawSlot(g_EXBRSA);
	Print_DrawTextAt(X, Y++, "- Disk:     ");
	Print_DrawSlot(g_MASTER);

	DisplayFooter();
}

//-----------------------------------------------------------------------------
//
void DisplaySlots()
{
	DisplayHeader();

	// Draw frames
	Print_DrawTextAt(0, SLOT_Y + 2, "FFFF\n\n\nC000\nBFFF\n\n\n8000\n7FFF\n\n\n4000\n3FFF\n\n\n0000");
	for(u8 slot = 0; slot < 4; ++slot)
	{
		Print_DrawTextAt((slot * 9) + 6, SLOT_Y, "Slot");
		Print_DrawInt(slot);
		if(Sys_IsSlotExpanded(slot))
		{
			for(u8 sub = 0; sub < 4; ++sub)
			{
				Print_SetPosition((slot * 9) + 5 + (sub * 2), SLOT_Y + 1);
				Print_DrawInt(sub);
			}
			for(u8 page = 0; page < 4; ++page)
			{
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 2);
				Print_DrawCharX(DOT_CHAR, 9);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 3);
				Print_DrawText("| | | | |");
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 4);
				Print_DrawText("| | | | |");
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 5);
				Print_DrawCharX(DOT_CHAR, 9);
			}
		}
		else
		{
			for(u8 page = 0; page < 4; ++page)
			{
				u8 pageSlot = Sys_GetPageSlot(3 - page);
				if(pageSlot == SLOT(slot))
				{
					Print_DrawTextAt((slot * 9) + 4, (page * 4) + SLOT_Y + 2, g_SlotTop);
					Print_DrawTextAt((slot * 9) + 4, (page * 4) + SLOT_Y + 3, g_SlotMid);
					Print_DrawTextAt((slot * 9) + 4, (page * 4) + SLOT_Y + 4, g_SlotMid);
					Print_DrawTextAt((slot * 9) + 4, (page * 4) + SLOT_Y + 5, g_SlotBot);
				}
				else
				{
					Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 2);
					Print_DrawCharX(DOT_CHAR, 9);
					Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 3);
					Print_DrawChar('|');
					g_PrintData.CursorX += 7;
					Print_DrawChar('|');
					Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 4);
					Print_DrawChar('|');
					g_PrintData.CursorX += 7;
					Print_DrawChar('|');
					Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 5);
					Print_DrawCharX(DOT_CHAR, 9);
				}
			}
		}
	}

	// Find slot type
	for(u8 page = 0; page < 4; ++page)
	{
		u8 pageSlot = Sys_GetPageSlot(3 - page);
		for(u8 slot = 0; slot < 4; ++slot)
		{
			if(Sys_IsSlotExpanded(slot))
			{
				for(u8 sub = 0; sub < 4; ++sub)
				{
					if(pageSlot == SLOTEX(slot, sub))
					{
						Print_SetPosition((slot * 9) + 4 + (sub * 2), (page * 4) + SLOT_Y + 2);
						Print_DrawText(g_SlotExTop);
						Print_SetPosition((slot * 9) + 4 + (sub * 2), (page * 4) + SLOT_Y + 3);
						Print_DrawText(g_SlotExMid);
						Print_SetPosition((slot * 9) + 4 + (sub * 2), (page * 4) + SLOT_Y + 4);
						Print_DrawText(g_SlotExMid);
						Print_SetPosition((slot * 9) + 4 + (sub * 2), (page * 4) + SLOT_Y + 5);
						Print_DrawText(g_SlotExBot);
					}
					Print_SetPosition((slot * 9) + 4 + (sub * 2), (page * 4) + SLOT_Y + 3 + (sub & 0x1));
					Print_DrawText(GetSlotName(SLOTEX(slot, sub), 3 - page));
				}
			}
			else
			{
				Print_SetPosition((slot * 9) + 7, (page * 4) + SLOT_Y + 3);
				Print_DrawText(GetSlotName(SLOT(slot), 3 - page));
			}
		}
	}
	
	DisplayFooter();
}



//=============================================================================
// MAIN LOOP
//=============================================================================

//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	g_VDP = VDP_GetVersion();
	
	VDP_SetMode(VDP_MODE_SCREEN0);
	VDP_FillVRAM_16K(0, 0x0000, 0x4000); // Clear VRAM
	
	Print_SetTextFont(PRINT_DEFAULT_FONT, 1);
	Print_SetColor(0xF, 0x0);

	callback cb = DisplayInfo;
	cb();

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		u8 row = Keyboard_Read(KEY_ROW(KEY_F1));
		if(IS_KEY_PRESSED(row, KEY_F1))
		{
			cb = DisplayInfo;
			cb();
		}
		else if(IS_KEY_PRESSED(row, KEY_F2))
		{
			cb = DisplaySlots;
			cb();
		}


		Print_SetPosition(39, 0);
		Print_DrawChar(g_ChrAnim[count++ & 0x03]);

		// Halt();
	}

	// Bios_ChangeMode(SCREEN_0);
	// Bios_ClearScreen();
}