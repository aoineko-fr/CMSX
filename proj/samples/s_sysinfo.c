// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
#pragma sdcc_hash +

#include "core.h"
#include "color.h"
#include "bios_main.h"
#include "input.h"
#include "print.h"
#include "memory.h"

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
	Bios_ClearScreen();

	SetPrintPos(1, 1);
	PrintText("| 1. SLOT | 2. MEM | 3. INPUT |");
	PrintLineX(1, 2, 80);
	
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
				SetPrintPos(x + 1, y++);
			}
			else
				SetPrintPos(x + 2, y++);			
			PrintSlot(slotId);
			for(i8 page = 0; page < 4; page++)
			{
				if(GetPageSlot(3 - page) == slotId)
				{
					static const c8 raw[] = { 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0 };
					static const c8 col[] = { 0xF9, ' ',  ' ',  ' ',  0xF9, 0 };

					SetPrintPos(x, y++);
					PrintText(raw);

					SetPrintPos(x, y);
					PrintText(col);
					SetPrintPos(x+1, y++);
					PrintText(GetSlotName(slotId, 3 - page));

					SetPrintPos(x, y);
					PrintText(raw);			
				}
				else
				{
					static const c8 raw[] = "+---+";
					static const c8 col[] = "|   |";

					i8 of = (sub == 0) ? 0 : 1;
					SetPrintPos(x + of, y++);
					if(page == 0)
						PrintText(raw + of);

					SetPrintPos(x + of, y);
					PrintText(col + of);
					SetPrintPos(x + 1, y++);
					PrintText(GetSlotName(slotId, 3 - page));

					SetPrintPos(x + of, y);
					PrintText(raw + of);
				}
			}
			x += 4;
		}
		x += 3;
	}

	for(i8 page = 0; page < 4; page++)
	{
		SetPrintPos(1, 6 + (page * 2));
		PrintText("Page ");			
		PrintInt(3 - page);			

		SetPrintPos(x + 1, 7 + (page * 2));
		PrintHex16(0x4000 * (3 - page));			
	}
	
	SetPrintPos(1, 15);
	PrintText("M-R: Main-ROM");
	SetPrintPos(1, 16);
	PrintText("S-R: Sub-ROM");
	SetPrintPos(1, 17);
	PrintText("D-R: Disk-ROM");
	
	SetPrintPos(20, 15);
	PrintText("EXPTBL: ");
	PrintHex8(g_EXPTBL[0]);
	SetPrintPos(20, 16);
	PrintText("EXBRSA: ");
	PrintHex8(g_EXBRSA);
	SetPrintPos(20, 17);
	PrintText("MASTER: ");
	PrintHex8(g_MASTER);
	
	for(i8 slot = 0; slot < 4; slot++)
	{
		SetPrintPos(1, 19 + slot);
		PrintText("Slot[");
		PrintInt(slot);
		PrintText("] EXPTBL: ");
		PrintHex8(g_EXPTBL[slot]);
	}

	SetPrintPos(24, 19);
	PrintText("RAMAD0: ");
	PrintHex8(g_RAMAD0);
	SetPrintPos(24, 20);
	PrintText("RAMAD1: ");
	PrintHex8(g_RAMAD1);
	SetPrintPos(24, 21);
	PrintText("RAMAD2: ");
	PrintHex8(g_RAMAD2);
	SetPrintPos(24, 22);
	PrintText("RAMAD3: ");
	PrintHex8(g_RAMAD3);

	SetPrintPos(1, 24);
	PrintText("[ESC] to quit");	
}

void DisplayMemory()
{	
	Bios_ClearScreen();

	SetPrintPos(1, 1);
	PrintText("| 1. SLOT | 2. MEM | 3. INPUT |");
	PrintLineX(1, 2, 80);

	SetPrintPos(1, 4);
	PrintText("Heap:  ");
	PrintHex16(Mem_GetHeapAddress());
	SetPrintPos(1, 5);
	PrintText("Stack: ");
	PrintHex16(Mem_GetStackAddress());
	SetPrintPos(1, 6);
	PrintText("Free:  ");
	PrintInt(Mem_GetHeapSize());
	PrintText(" bytes");

	SetPrintPos(1, 24);
	PrintText("[ESC] to quit");
}


//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	Bios_Beep();
	
	g_LINL40 = 80;
	Bios_ChangeMode(SCREEN_0);
	Bios_ChangeColor(COLOR_WHITE, COLOR_BLACK, COLOR_BLACK);
	
	DisplaySlot();

	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		if(Keyboard_IsKeyPressed(KEY_1))
			DisplaySlot();
		else if(Keyboard_IsKeyPressed(KEY_2))
			DisplayMemory();



		SetPrintPos(g_LINL40 - 2, 1);
		static const u8 chrAnim[] = { '|', '\\', '-', '/' };
		u8 chr = count++ & 0x03;
		PrintChar(chrAnim[chr]);

		//VDP_WaitRetrace();
	}

	Bios_ChangeMode(SCREEN_0);
	Bios_ClearScreen();
}