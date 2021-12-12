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
const u8 g_SlotExTop[]	= { 0x18, 0x17, 0x12, 0x17, 0x12, 0x17, 0x12, 0x17, 0x19, 0 }; 
const u8 g_SlotExMid[]	= { 0x16, 0x20, 0x16, 0x20, 0x16, 0x20, 0x16, 0x20, 0x16, 0 }; 
const u8 g_SlotExBot[]	= { 0x1A, 0x17, 0x11, 0x17, 0x11, 0x17, 0x11, 0x17, 0x1B, 0 }; 

//=============================================================================
// MEMORY DATA
//=============================================================================
u8 g_VDP;

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

/*void InstallCustomVBlankInterrupt()
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
/*inline u8 GetPageSlot(u8 page)
{
	u8 slot = (g_PortPrimarySlot >> (page * 2)) & 0x03;
	if(g_EXPTBL[slot] & 0x80)
	{
		slot |= SLOT_EXP;
		slot |= (((~g_SLTSL) >> (page * 2)) & 0x03) << 2;
	}
	return slot;
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
void DisplaySlot_W80()
{	
	Print_Clear();
	Print_SetPosition(0, 0);
	Print_DrawText("| 1. SLOT | 2. MEM | 3. INPUT |");
	Print_DrawLineH(0, 1, 80);

	u8 x = 9;
	u8 y;
	for(i8 slot = 0; slot < 4; slot++)
	{
		i8 numCol = (g_EXPTBL[slot] & 0x80) ? 4 : 1;
		for(i8 sub = 0; sub < numCol; sub++)
		{
			y = 4;
			u8 slotId = slot;
			if(g_EXPTBL[slot] & 0x80)
			{
				slotId += 0x80 + (sub << 2);
				Print_SetPosition(x, y++);
			}
			else
				Print_SetPosition(x + 1, y++);			
			// PrintSlot(slotId);
			Print_DrawHex8(slotId);
			for(i8 page = 0; page < 4; page++)
			{
				if(Sys_GetSlot(3 - page) == slotId)
				{
					static const c8 raw[] = { 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0 };
					static const c8 col[] = { 0xF9, ' ',  ' ',  ' ',  0xF9, 0 };

					Print_SetPosition(x, y++);
					Print_DrawText(raw);

					Print_SetPosition(x, y);
					Print_DrawText(col);
					Print_SetPosition(x+1, y++);
					Print_DrawText(GetSlotName(slotId, 3 - page));

					Print_SetPosition(x, y);
					Print_DrawText(raw);			
				}
				else
				{
					static const c8 raw[] = "+---+";
					static const c8 col[] = "|   |";

					i8 of = (sub == 0) ? 0 : 1;
					Print_SetPosition(x + of, y++);
					if(page == 0)
						Print_DrawText(raw + of);

					Print_SetPosition(x + of, y);
					Print_DrawText(col + of);
					Print_SetPosition(x + 1, y++);
					Print_DrawText(GetSlotName(slotId, 3 - page));

					Print_SetPosition(x + of, y);
					Print_DrawText(raw + of);
				}
			}
			x += 4;
		}
		x += 3;
	}

	for(i8 page = 0; page < 4; page++)
	{
		Print_SetPosition(0, 5 + (page * 2));
		Print_DrawText("Page ");			
		Print_DrawInt(3 - page);			

		Print_SetPosition(x, 6 + (page * 2));
		Print_DrawHex16(0x4000 * (3 - page));			
	}
	
	Print_SetPosition(0, 14);
	Print_DrawText("M-R: Main-ROM");
	Print_SetPosition(0, 15);
	Print_DrawText("S-R: Sub-ROM");
	Print_SetPosition(0, 16);
	Print_DrawText("D-R: Disk-ROM");
	
	Print_SetPosition(19, 14);
	Print_DrawText("EXPTBL: ");
	Print_DrawHex8(g_EXPTBL[0]);
	Print_SetPosition(19, 15);
	Print_DrawText("EXBRSA: ");
	Print_DrawHex8(g_EXBRSA);
	Print_SetPosition(19, 16);
	Print_DrawText("MASTER: ");
	Print_DrawHex8(g_MASTER);
	
	for(i8 slot = 0; slot < 4; slot++)
	{
		Print_SetPosition(0, 18 + slot);
		Print_DrawText("Slot[");
		Print_DrawInt(slot);
		Print_DrawText("] EXPTBL: ");
		Print_DrawHex8(g_EXPTBL[slot]);
	}

	Print_SetPosition(23, 18);
	Print_DrawText("RAMAD0: ");
	Print_DrawHex8(g_RAMAD0);
	Print_SetPosition(23, 19);
	Print_DrawText("RAMAD1: ");
	Print_DrawHex8(g_RAMAD1);
	Print_SetPosition(23, 20);
	Print_DrawText("RAMAD2: ");
	Print_DrawHex8(g_RAMAD2);
	Print_SetPosition(23, 21);
	Print_DrawText("RAMAD3: ");
	Print_DrawHex8(g_RAMAD3);

	Print_SetPosition(0, 23);
	Print_DrawText("[ESC] to quit");	
}

//-----------------------------------------------------------------------------
//
void DisplayMemory_W80()
{	
	Print_Clear();

	Print_SetPosition(0, 0);
	Print_DrawText("| 1. SLOT | 2. MEM | 3. INPUT |");
	Print_DrawLineH(0, 1, 80);

	Print_SetPosition(0, 3);
	Print_DrawText("Heap:  ");
	Print_DrawHex16(Mem_GetHeapAddress());
	Print_SetPosition(0, 4);
	Print_DrawText("Stack: ");
	Print_DrawHex16(Mem_GetStackAddress());
	Print_SetPosition(0, 5);
	Print_DrawText("Free:  ");
	Print_DrawInt(Mem_GetHeapSize());
	Print_DrawText(" bytes");

	Print_SetPosition(0, 23);
	Print_DrawText("[ESC] to quit");
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

	DisplayFooter();
}

//-----------------------------------------------------------------------------
//
void DisplaySlots()
{
	DisplayHeader();

	// Draw frames
	Print_SetPosition(0, SLOT_Y + 2);
	Print_DrawText("FFFF\n\n\nC000\nBFFF\n\n\n8000\n7FFF\n\n\n4000\n3FFF\n\n\n0000");
	for(u8 slot = 0; slot < 4; ++slot)
	{
		Print_SetPosition((slot * 9) + 6, SLOT_Y);
		Print_DrawText("Slot");
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
				Print_DrawText(g_SlotExTop);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 3);
				Print_DrawText(g_SlotExMid);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 4);
				Print_DrawText(g_SlotExMid);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 5);
				Print_DrawText(g_SlotExBot);
			}
		}
		else
		{
			for(u8 page = 0; page < 4; ++page)
			{
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 2);
				Print_DrawText(g_SlotTop);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 3);
				Print_DrawText(g_SlotMid);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 4);
				Print_DrawText(g_SlotMid);
				Print_SetPosition((slot * 9) + 4, (page * 4) + SLOT_Y + 5);
				Print_DrawText(g_SlotBot);
			}
		}
	}

	// Find slot type
	for(u8 page = 0; page < 4; ++page)
	{
		u8 pageSlot = Sys_GetSlot(3 - page);
		for(u8 slot = 0; slot < 4; ++slot)
		{
			if(Sys_IsSlotExpanded(slot))
			{
				for(u8 sub = 0; sub < 4; ++sub)
				{
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