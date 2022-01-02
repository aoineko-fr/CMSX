// ____________________________________________________________________________
// ██▀█▀██▀▀▀█▀▀███   ▄▄▄                ▄▄       
// █  ▄ █  ███  ███  ▀█▄  ▄▀██ ▄█▄█ ██▀▄ ██  ▄███ 
// █  █ █▄ ▀ █  ▀▀█  ▄▄█▀ ▀▄██ ██ █ ██▀  ▀█▄ ▀█▄▄ 
// ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀─────────────────▀▀─────────────────────────────────────────
//  48K ROM sample (page 0 still pointing to Main-ROM)

//=============================================================================
// INCLUDES
//=============================================================================
#include "cmsx.h"

//=============================================================================
// DEFINES
//=============================================================================

// Library's logo
#define MSX_GL		"\x01\x02\x03\x04\x05\x06"

#define BANK_ADDR	0x8000
#define MAPPER_Y	9
#define MAPPER_NUM	6

//=============================================================================
// READ-ONLY DATA
//=============================================================================

// Fonts data
#include "font\font_cmsx_sample6.h"

// Data in bank 2 & 3
#if (ROM_MAPPER != ROM_PLAIN)
#if (ROM_MAPPER == ROM_ASCII16)
__at(0x8000) const c8 g_DataBank1[] = "Segment #1 (default in Bank #1)";
#else
__at(0x8000) const c8 g_DataBank2[] = "Segment #2 (default in Bank #2)";
__at(0xA000) const c8 g_DataBank3[] = "Segment #3 (default in Bank #3)";
#endif
#endif

// Character animation data
const c8 g_ChrAnim[] = { '|', '\\', '-', '/' };
// Hexadecimal characters
const c8 hexChar[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
//=============================================================================
// MEMORY DATA
//=============================================================================
u8  g_Buffer[128];
u8  g_PageSlot[4];
#if (ROM_MAPPER != ROM_PLAIN)
u8  g_DisplayASCII = false;
u8  g_Segment = 0;
u16 g_Address = BANK_ADDR;
#endif

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
//
void Print_Slot(u8 slot)
{
	Print_DrawInt(Sys_SlotGetPrimary(slot));
	if(Sys_SlotIsExpended(slot))
	{
		Print_DrawChar('-');
		Print_DrawInt(Sys_SlotGetSecondary(slot));
	}
}

//-----------------------------------------------------------------------------
//
void InterSlotWritePage3(u8 slot, u16 addr, u8 value) __sdcccall(0)
{
	slot;  // IX+4
	addr;  // IX+6 IX+5
	value; // IX+7
	__asm
		di
		push	ix
		ld		ix, #0
		add		ix, sp
		ld		d, 4(ix)		// D=[E.|..|SS|PP] Get slot ID
		ld		l, 5(ix)
		ld		h, 6(ix)		// HL=Write address
		ld		a, 7(ix)		
		ld		i, a			// I=Write value
		
		// Backup primary slot
		in		a, (P_PPI_A)	// A=[P3|P2|P1|P0]
		ld		b, a			// B=[P3|P2|P1|P0] Save current primary slot in B

		// Switch primary slot
		ld		a, d			// A=[E.|..|SS|PP]
		rrca
		rrca					// A=[PP|E.|..|SS]
		and		#0b11000000		// A=[PP|00|00|00]
		ld		e, a			// E=[PP|00|00|00]
		ld		a, b			// A=[P3|P2|P1|P0]
		and		#0b00111111		// A=[00|P2|P1|P0]
		or		e				// A=[PP|P2|P1|P0] remplace ?? par le slot primaire (00,01,10 ou 11)
		out		(P_PPI_A), a	//                 write new primary slot

		// Backup secondary slot (of the new primary slot)
		ld		a, (M_SLTSL)	// A=[~3|~2|~1|~0]
		cpl						// A=[S3|S2|S1|S0]
		ld		c, a			// C=[S3|S2|S1|S0] Save current secondary slot in C

		// Switch secondary slot
		ld		a, d			// A=[E.|..|SS|PP]
		rlca
		rlca					// A=[..|SS|PP|E.]
		rlca
		rlca					// A=[SS|PP|E.|..]
		and		#0b11000000		// A=[SS|00|00|00]
		ld		e, a			// E=[SS|00|00|00]
		ld		a, c			// A=[S3|S2|S1|S0]
		and		#0b00111111		// A=[00|S2|S1|S0]
		or		e				// A=[SS|S2|S1|S0] remplace ?? par le slot secondaire (00,01,10 ou 11)
		ld		(M_SLTSL), a	//				   write new seconday slot

		// Write
		ld		a, i
		ld		(hl), a

		// Restore		
		ld		a, c			// C=[S3|S2|S1|S0]
		ld		(M_SLTSL), a
		ld		a, b			// B=[P3|P2|P1|P0]
		out		(P_PPI_A), a

		pop		ix
		ei
	__endasm;
}

//-----------------------------------------------------------------------------
//
u8 InterSlotReadPage3(u8 slot, u16 addr)
{
	slot;	// A  -> D
	addr;	// DE -> HL
	__asm
		di
		ld		d, a			// D=[E.|..|SS|PP] Get slot ID
		ld		l, d
		ld		h, e			// HL=Read address

		// Backup primary slot
		in		a, (P_PPI_A)	// A=[P3|P2|P1|P0]
		ld		b, a			// B=[P3|P2|P1|P0] Save current primary slot in B

		// Switch primary slot
		ld		a, d			// A=[E.|..|SS|PP]
		rrca
		rrca					// A=[PP|E.|..|SS]
		and		#0b11000000		// A=[PP|00|00|00]
		ld		e, a			// E=[PP|00|00|00]
		ld		a, b			// A=[P3|P2|P1|P0]
		and		#0b00111111		// A=[00|P2|P1|P0]
		or		e				// A=[PP|P2|P1|P0] Remplace ?? par le slot primaire (00,01,10 ou 11)
		out		(P_PPI_A), a

		// Backup secondary slot (of the new primary slot)
		ld		a, (M_SLTSL)	// A=[~3|~2|~1|~0]
		cpl						// A=[S3|S2|S1|S0]
		ld		c, a			// C=[S3|S2|S1|S0] Save current secondary slot in C

		// Switch secondary slot
		ld		a, d			// A=[E.|..|SS|PP]
		rlca					
		rlca					// A=[..|SS|PP|E.]
		rlca
		rlca					// A=[SS|PP|E.|..]
		and		#0b11000000		// A=[SS|00|00|00]
		ld		e, a			// E=[SS|00|00|00]
		ld		a, c			// A=[S3|S2|S1|S0]
		and		#0b00111111		// A=[S3|00|00|00]
		or		e				// A=[SS|S2|S1|S0] Remplace ?? par le slot secondaire (00,01,10 ou 11)
		ld		(M_SLTSL), a
		// Read
		ld		a, (hl)
		ld		l, a			// L=Return value

		// Restore		
		ld		a, c
		ld		(M_SLTSL), a
		ld		a, b
		out		(P_PPI_A), a

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
	
	if(page < 3)
	{
		u16 addr = page * 0x4000;
		if(Bios_InterSlotRead(slotId, addr) == 'A')
			if(Bios_InterSlotRead(slotId, ++addr) == 'B')
				return "ROM";
	}
		
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
const c8* GetTargetType(u8 target)
{
	switch(target)
	{
	case TYPE_BIN: return "BASIC";
	case TYPE_ROM: return "ROM";
	case TYPE_DOS: return "DOS";
	};
	return "Unknow";
}

//-----------------------------------------------------------------------------
//
const c8* GetROMMapper(u8 mapper)
{
	switch(mapper)
	{
	case ROM_PLAIN:      return "Plain";
	case ROM_ASCII8:     return "ASCII8";
	case ROM_ASCII16:    return "ASCII16";
	case ROM_KONAMI:     return "Konami";
	case ROM_KONAMI_SCC: return "Konami SCC";
	};
	return "Unknow";
}

//-----------------------------------------------------------------------------
//
const c8* GetROMSize(u8 size)
{
	switch(size)
	{
	case ROM_8K:	return "8K";
	case ROM_16K:	return "16K";
	case ROM_32K:	return "32K";
	case ROM_48K:	return "48K";
	case ROM_64K:	return "64K";
	case ROM_128K:	return "128K";
	case ROM_256K:	return "256K";
	case ROM_512K:	return "512K";
	case ROM_1M:	return "1M";
	case ROM_2M:	return "2M";
	case ROM_4M:	return "4M";
	};
	return "Unknow";
}

//-----------------------------------------------------------------------------
//
#if (ROM_MAPPER != ROM_PLAIN)
void DiplayMapper()
{
	Print_SetPosition(30, MAPPER_Y);
	Print_DrawText("\x81 ");
	Print_DrawHex16(g_Address);
	Print_DrawText(" \x80\n");

	Print_SetPosition(1, MAPPER_Y + 2);
	Print_DrawChar((g_Segment == 0) ? ' ' : '\x8E');
	Print_SetPosition(1, MAPPER_Y + 1 + MAPPER_NUM);
	Print_DrawChar((g_Segment == ROM_SEGMENTS - MAPPER_NUM) ? ' ' : '\x8F');


	for(u8 i = 0; i < MAPPER_NUM; i++)
	{
		Print_SetPosition(2, MAPPER_Y + 2 + i);
		Print_DrawInt(g_Segment + i);
		if(g_Segment + i < 100)
			Print_DrawChar(' ');

		Print_SetPosition(6, MAPPER_Y + 2 + i);
		#if (ROM_MAPPER == ROM_ASCII16)
			SET_BANK_SEGMENT(1, g_Segment + i);
		#else
			SET_BANK_SEGMENT(2, g_Segment + i);
		#endif
		u8 max = g_DisplayASCII ? 33 : 11;
		for(u8 j = 0; j < max; j++)
		{
			u8 chr = ((const u8*)(g_Address))[j];
			if(g_DisplayASCII)
				Print_DrawChar(chr);
			else
			{
				Print_DrawChar(hexChar[(chr >> 4) & 0x000F]);
				Print_DrawChar(hexChar[chr & 0x000F]);
				Print_DrawChar(' ');
			}
		}
		Print_Return();
	}
}
#endif // (ROM_MAPPER != ROM_PLAIN)

//=============================================================================
// MAIN LOOP
//=============================================================================
//-----------------------------------------------------------------------------
// Program entry point
void main()
{
	VDP_SetMode(VDP_MODE_SCREEN0);
	VDP_EnableVBlank(true);

	Print_Clear();
	Print_SetTextFont(g_Font_CMSX_Sample6, 1);
	Print_DrawText(MSX_GL "   Target Sample\n");
	Print_DrawLineH(0, 1, 40);

	Print_SetPosition(0, 2);
	Print_DrawText(TARGET_NAME);
	Print_Return();

	Print_DrawText("Target: ");
	Print_DrawHex16(TARGET);
	Print_Return();
	
	Print_DrawText("Type:   ");
	Print_DrawText(GetTargetType(TARGET_TYPE));
	Print_Return();
	
	#if (TARGET_TYPE == TYPE_ROM)
		Print_DrawText("Mapper: ");
		Print_DrawText(GetROMMapper(ROM_MAPPER));
		Print_Return();
		
		Print_DrawText("Size:   ");
		Print_DrawText(GetROMSize(ROM_SIZE));
		#if (ROM_MAPPER != ROM_PLAIN)
			Print_DrawChar('(');
			Print_DrawInt(ROM_SEGMENTS);
			Print_DrawChar(')');
		#endif
		Print_Return();
	#endif

	Print_DrawText("Addr:   ");
	Print_DrawHex16(Sys_GetFirstAddr());
	Print_DrawChar('~');
	Print_DrawHex16(Sys_GetLastAddr());

	for(u8 i = 0; i < 4; i++)
	{
		g_PageSlot[i] = Sys_GetPageSlot(i);
		Print_SetPosition(20, 3+i);
		Print_DrawText("Page #");
		Print_DrawInt(i);
		Print_DrawText(": ");
		Print_Slot(g_PageSlot[i]);
		Print_SetPosition(33, 3+i);
		Print_DrawChar('(');
		Print_DrawText(GetSlotName(g_PageSlot[i], i));
		Print_DrawChar(')');
	}

	#if (ROM_MAPPER != ROM_PLAIN)

		Print_SetPosition(0, MAPPER_Y);
		#if (ROM_MAPPER == ROM_ASCII16)
			Print_DrawText("Segments in Bank #1");
		#else
			Print_DrawText("Segments in Bank #2");
		#endif
		
		Print_SetPosition(0, MAPPER_Y + 1);
		Print_DrawText("\x18\x17\x17\x17\x17\x12");
		Print_DrawCharX('\x17', 33);
		Print_DrawChar('\x19');
		
		for(u8 i = 0; i < MAPPER_NUM; i++)
		{
			Print_SetPosition(0, MAPPER_Y + 2 + i);
			Print_DrawChar('\x16');
			g_PrintData.CursorX = 5;
			Print_DrawChar('\x16');
			g_PrintData.CursorX = 39;
			Print_DrawChar('\x16');
		}

		Print_SetPosition(0, MAPPER_Y + 2 + MAPPER_NUM);
		Print_DrawText("\x1A\x17\x17\x17\x17\x11");
		Print_DrawCharX('\x17', 33);
		Print_DrawChar('\x1B');

		Print_DrawLineH(0, 22, 40);
		Print_SetPosition(0, 23);
		Print_DrawText("F1:Hexa F2:ASCII \x81\x80:Addr \x82:Seg");

		DiplayMapper();

	#endif
	
	u8 count = 0;
	while(!Keyboard_IsKeyPressed(KEY_ESC))
	{
		Print_SetPosition(39, 0);
		Print_DrawChar(g_ChrAnim[count++ & 0x03]);
		
		// Handle user input
		u8 row6 = Keyboard_Read(6);
		u8 row8 = Keyboard_Read(8);

		#if (ROM_MAPPER != ROM_PLAIN)
			if(IS_KEY_PRESSED(row6, KEY_F1))
			{
				g_DisplayASCII = false;
				DiplayMapper();
			}
			if(IS_KEY_PRESSED(row6, KEY_F2))
			{
				g_DisplayASCII = true;
				DiplayMapper();
			}
			if(IS_KEY_PRESSED(row8, KEY_RIGHT))
			{
				g_Address++;
				DiplayMapper();
			}
			if(IS_KEY_PRESSED(row8, KEY_LEFT))
			{
				if(g_Address > BANK_ADDR)
					g_Address--;
				DiplayMapper();
			}
			if(IS_KEY_PRESSED(row8, KEY_DOWN))
			{
				if(g_Segment < ROM_SEGMENTS-MAPPER_NUM)
					g_Segment++;
				DiplayMapper();
			}
			if(IS_KEY_PRESSED(row8, KEY_UP))
			{
				if(g_Segment > 0)
					g_Segment--;
				DiplayMapper();
			}
		#endif // (ROM_MAPPER != ROM_PLAIN)

		Halt();
	}
	
	Bios_Exit(0);
}