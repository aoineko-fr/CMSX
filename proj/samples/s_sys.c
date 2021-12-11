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

inline u8 GetPageSlot(u8 page)
{
	u8 slot = (g_PortPrimarySlot >> (page * 2)) & 0x03;
	if(g_EXPTBL[slot] & 0x80)
	{
		slot |= SLOT_EXP;
		slot |= (((~g_SLTSL) >> (page * 2)) & 0x03) << 2;
	}
	return slot;
}

void InterSlotWritePage3(u8 slot, u16 addr, u8 value)
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

u8 InterSlotReadPage3(u8 slot, u16 addr)
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

const c8* GetSlotName(u8 slotId, u8 page)
{
	if(slotId == g_EXPTBL[0])
		return "M-R";

	if(slotId == g_EXBRSA)
		return "S-R";

	if(slotId == g_MASTER)
		return "D-R";
		
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

void DisplaySlot()
{	
	// Bios_ClearScreen();
	// VDP_FillVRAM_16K(0, 0x0000, 0x4000);

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
				if(GetPageSlot(3 - page) == slotId)
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

void DisplayMemory()
{	
	// Bios_ClearScreen();
	// VDP_FillVRAM_16K(0, 0x0000, 0x4000);

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
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_SCREEN0_W80);
	VDP_SetColor(0xF0);
	VDP_FillVRAM_16K(0, 0x0000, 0x4000); // Clear VRAM
	
	Print_SetTextFont(PRINT_DEFAULT_FONT, 1);
	Print_SetColor(0xF, 0x0);

	DisplaySlot();

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		if(Keyboard_IsKeyPressed(KEY_1))
			DisplaySlot();
		else if(Keyboard_IsKeyPressed(KEY_2))
			DisplayMemory();



		Print_SetPosition(g_LINL40 - 1, 0);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		u8 chr = count++ & 0x03;
		Print_DrawChar(chrAnim[chr]);

		//VDP_WaitRetrace();
	}

	// Bios_ChangeMode(SCREEN_0);
	// Bios_ClearScreen();
}