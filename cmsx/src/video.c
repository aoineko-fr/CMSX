//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "core.h"
#include "video.h"
#include "bios.h"
#include "ports.h"

VDP __at(0xC000) vdp;

/** Initialize video library */
void VideoInitialize()
{
	//vdp.cmd.ARG = 0; 
	vdp.cmd.ARG = 0; 

	// Clear all VRAM
	FillVRAM(0,   0, 256, 256, 0);
	//testHMMV(0,   0, 256, 256, 0);
	FillVRAM(0, 256, 256, 256, 0);
}

void ClearScreen8(u8 color)
{
	FillVRAM(0,   0, 256, 212, color);
	FillVRAM(0, 256, 256, 212, color);
}

/**
 *
 */
void SetScreen8(u8 flag)
{
	flag;

	WaitForVDP();

	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		//; Passage en SCREEN 8
		di //; on interdit les interruptions

		//; - modification registre 0 -
		ld		a, (M_RG0SAV)
		set		#3, a           //; bit 3 a 1 (mode 8)
		set		#2, a           //; bit 2 a 1 (mode 8)
		set		#1, a           //; bit 1 a 1 (mode 8)
		ld		(M_RG0SAV), a
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(0)
		out		(P_VDP_ADDR), a

		//; - modification registre 1 -
		ld		a, (M_RG1SAV)
		res		#4, a           //; bit 4 a 0 (mode 8)
		res		#3, a           //; bit 3 a 0 (mode 8)
		ld		(M_RG1SAV), a
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(1)
		out		(P_VDP_ADDR), a

		//; - modification registre 2 -
		ld		a, #0x1F ;// 00011111b (addr de la page graphique 0)
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(2)
		out		(P_VDP_ADDR), a

		//; - modification registre 7 -
		xor		a ;// Set black for text & background color
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(7)
		out		(P_VDP_ADDR), a

		//; - modification registre 9 -
		ld		a, (M_RG09SAV)
		res		#7, a           //; bit 7 a 0
		or		4(ix) ;// 192 (0x00) or 212 lines (0x80)?
		ld		(M_RG09SAV), a
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(9)
		out		(P_VDP_ADDR), a

		//; - modification registre 16 -
		//xor		a
		//out		(P_VDP_ADDR), a

		//ld		a, VDP_REG(16)
		//out		(P_VDP_ADDR), a

		ld		hl, #0xFCAF
		ld		(hl), #8

		ei //; on autorise les interruptions
		
		pop		ix

	
	__endasm;
}

//void SetBorderColor(u8 color)
//{
//	color;
//
//	WaitForVDP();
//
//	__asm
//
//		di //; on interdit les interruptions
//
//		//; - modification registre 7 -
//;//		ld		a, 4(ix) ;// border color
//;//		out		(P_VDP_ADDR), a
//;//
//;//		ld		a, VDP_REG(7)
//;//		out		(P_VDP_ADDR), a
//
//		ld		a, 4(ix) ;// border color
//		ld		(BDRCLR), a
//
//	ei //; on autorise les interruptions
//
//	__endasm;
//}

/***/
//                    4            5         7-6      9-8
void SetSpriteMode(u8 activate, u8 flag, u16 tgs, u16 tas)
{
	activate; flag; tgs; tas;

	WaitForVDP();

	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		//; Passage en SCREEN 8
		di //; on interdit les interruptions

		//; - modification registre 1 -
		ld		a, (M_RG1SAV)
		and		#0xFC ;// 1111 1100
		or		5(ix)
		ld		(M_RG1SAV), a
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(1)
		out		(P_VDP_ADDR), a

		//; - modification registre 5 -
		ld		a, 8(ix) ;// TAS addr (low)
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(5)
		out		(P_VDP_ADDR), a

		//; - modification registre 6 -
		ld		a, 6(ix) ;// TGS addr
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(6)
		out		(P_VDP_ADDR), a

		//; - modification registre 8 -
		ld		a, (M_RG08SAV)
		and		#0xFD ;// 1111 1101
		or		4(ix)
		ld		(M_RG08SAV), a
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(8)
		out		(P_VDP_ADDR), a

		//; - modification registre 11 -
		ld		a, 9(ix) ;// TAS addr (high)
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(11)
		out		(P_VDP_ADDR), a

		ei //; on autorise les interruptions
		
		pop		ix
	__endasm;
}


/** Set frequence 50/60Hz */
void SetFreq(u8 freq)
{
	freq;

	WaitForVDP();

	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		//; - modification registre 9 -
		ld		a, (M_RG09SAV)
		//and		#FREQ_MASK ;// 1111 1101
		res		#1, a ;// bit 1 à 0
		or		4(ix)
		ld		(M_RG09SAV), a
		di //; on interdit les interruptions
		out		(P_VDP_ADDR), a

		ld		a, VDP_REG(9)
		ei //; on autorise les interruptions
		out		(P_VDP_ADDR), a

		pop		ix
	__endasm;
}

/** Set current page for mode 8 */
void SetPage8(u8 page)
{
	page;

	WaitForVDP();

	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		//; - modification registre 2 -
		ld      a, 4(ix)       //; donnee
		rrca
		rrca
		rrca
		or		#0x1F         //; += 00011111b
	di //; on interdit les interruptions
		out		(P_VDP_ADDR), a  
		ld		a, VDP_REG(2)     //; write to register R#2
	ei //; on autorise les interruptions
		out		(P_VDP_ADDR), a  

		pop		ix
	__endasm;
}

/**
 * PSET (128,96),255
 */
#ifdef USE_DRAWPOINT8
void DrawPoint8(u8 posX, u8 posY, u8 color)
{
	VdpBuffer36 buffer;
	
	buffer.DX = posX;
	buffer.DY = posY;
	buffer.NX = 0;
	buffer.NY = 0;
	buffer.CLR = color;
	buffer.ARG = 0;
	buffer.CMD = 0x50;
	VPDCommand36((u16)&buffer);
}
#endif

void Line(u16 x1, u16 y1, u16 x2, u16 y2, u8 color, u8 op)
{
	u16 dx, dy, maj, min;
	u8 arg;

	arg = 0;
	if(x1 > x2)
	{
		arg |= ARG_DIX_LEFT;
		dx = x1 - x2;
	}
	else // (x1 <= x2)
	{
		arg |= ARG_DIX_RIGHT;
		dx = x2 - x1;
	}
	if(y1 > y2)
	{
		arg |= ARG_DIY_UP;
		dy = y1 - y2;
	}
	else // (y1 <= y2)
	{
		arg |= ARG_DIY_DOWN;
		dy = y2 - y1;
	}
	if(dx > dy)
	{
		arg |= ARG_MAJ_H;
		maj = dx;
		min = dy;
	}
	else // (dx <= dy)
	{
		arg |= ARG_MAJ_V;
		maj = dy;
		min = dx;
	}

	LINE(x1, y1, maj, min, color, arg, op);
}

void LineH(u16 x1, u16 x2, u16 y, u8 color, u8 op)
{
	u16 dx, nx;
	if(x1 > x2)
	{
		dx = x2;
		nx = x2 - x1;
	}
	else
	{
		dx = x1;
		nx = x1 - x2;
	}
	LMMV(dx, y, nx, 1, color, op);
}

void LineV(u16 x, u16 y1, u16 y2, u8 color, u8 op)
{
	u16 dy, ny;
	if(y1 > y2)
	{
		dy = y2;
		ny = y2 - y1;
	}
	else
	{
		dy = y1;
		ny = y1 - y2;
	}
	LMMV(x, dy, 1, ny, color, op);
}

/**
 * LINE (128,96),(190,56),255
 */
#ifdef USE_DRAWLINE8

void DrawLine8(char posX1, char posY1, char posX2, char posY2, char color)
{
	posX1; posY1; posX2; posY2; color;

	WaitForVDP();
/*

	__asm

		//; a:							f: 
		//; b: DX						c: DY
		//; d: Maj						e: Min
		//; h: VDP Command argument		l:

		//; - X -
		ld    a, 4(ix)                  //; input X1 parameter
		di
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#36              //; write to register R#36
		out   (P_VDP_ADDR), a

		xor   a                        //; set A to 0 (only use 8 bits coordinate)
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#37              //; write to register R#37
		out   (P_VDP_ADDR), a

		//; - Y -
		ld    a, 5(ix)                  //; input Y1 parameter
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#38              //; write to register R#38
		out   (P_VDP_ADDR), a

		xor   a                        //; set A to 0 (only use 8 bits coordinate)
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#39              //; write to register R#39
		out   (P_VDP_ADDR), a

		//; - Color -
		ld    a, 8(ix)                  //; input color parameter
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#44              //; write to register R#44
		out   (P_VDP_ADDR), a

		ld    h, #0
		//; - Compute DX -
		ld    a, 6(ix)                  //; input X1 parameter
		ld    b, 4(ix)                  //; input X2 parameter
		sbc   a, b                      //; compute DX
		ld    b, a                      //; Put result in B
		//; - Check DX sign -
		cp    #0x80                    //; comparing the unsigned A to 128
		jr    c, DX_Is_Positive         //; if it is less, then jump to the label given
		neg                            //; multiplying A by -1
		ld    b, a                      //; Put result in B
		ld    a, h
		add   a, #0x04                  //; set DIX flag for command argument register
		ld    h, a
	DX_Is_Positive:                    //; after this label, A is between 0 and 128
	
		//; - Compute DY -
		ld    a, 7(ix)                  //; input Y1 parameter
		ld    c, 5(ix)                  //; input Y2 parameter
		sbc   a, c                      //; compute DY
		ld    c, a                      //; Put result in C
		//; - Check DY sign -
		cp    #0x80                    //; comparing the unsigned A to 128
		jr    c, DY_Is_Positive         //; if it is less, then jump to the label given
		neg                            //; multiplying A by -1
		ld    c, a                      //; Put result in C
		ld    a, h
		add   a, #0x08                  //; set DIY flag for command argument register
		ld    h, a
	DY_Is_Positive:                    //; after this label, A is between 0 and 128

		//; - Check DX > DY -
		ld    a, c
		cp    a, b
		jr    c, DX_Superior_DY

	DX_Superior_DY: // DX > DY

		ld    d, b
		ld    e, a
		jp    End_Compare

	DY_Superior_DX: // DX <= DY

		ld    d, a
		ld    e, b
		ld    a, h
		add   a, #0x01                  //; set MAJ flag for command argument register
		ld    h, a

	End_Compare:

		//; - Maj - 
		ld    a, d                      //; abs(max(DX, DY))
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#40              //; write to register R#40
		out   (P_VDP_ADDR), a

		xor   a                        //; set 'a' to 0 (only use 8 bits coordinate)
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#41              //; write to register R#41
		out   (P_VDP_ADDR), a

		//; - Min - 
		ld    a, e                      //; abs(min(DX, DY))
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#42              //; write to register R#42
		out   (P_VDP_ADDR), a

		xor   a                        //; set 'a' to 0 (only use 8 bits coordinate)
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#43              //; write to register R#43
		out   (P_VDP_ADDR), a

		//; - Set arguments -
		ld    a, h                      //; argument flags
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#45              //; write to register R#45
		out   (P_VDP_ADDR), a

		//; Execute VDP macro
		ld    a, #0x70                  //; Executing the LINE command (01110000b)
		out   (P_VDP_ADDR), a
		ld    a, #0x80+#46              //; write to register R#46
		out   (P_VDP_ADDR), a

		ei

	__endasm;
*/
}
#endif

/**
 *
 */
void VDP_WaitRetrace()
{
	__asm

		di
	WAIT_RETRACE:
		in		a, (P_VDP_STAT)
		and		#0x80
		cp		#0
		jr		z, WAIT_RETRACE
		ei

	__endasm;
}

/** Attente de libération du VDP */ 
void WaitForVDP()
{
	__asm
		
		//; Attente libération VDP. Test bit CE sur registre S2
		ld		a, #2
		di //; on interdit les interruptions
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(15)
		out		(P_VDP_ADDR), a
	WAIT_VDP:	
		nop
		in		a, (P_VDP_ADDR)
		rra
		jr		c, WAIT_VDP		
		xor		a
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(15)
		ei //; on autorise les interruptions
		out		(P_VDP_ADDR), a		//; RAZ du registre 15

	__endasm;
}

/** Write a byte in the VRAM */
void WriteVRAM(u8 page, u16 addr, u8 value)
{
	page; addr; value;

	WaitForVDP();
	
	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		;// Set 0 to register 14 (we don't use address bits 14-16)
		ld		a, 4(ix)     ;// Bits 15
        rla
        rla
		ld		c, a
		ld		a, 6(ix)     ;// Bits 14
		and		#0xC0		;// Keep only 2 last bits
        rla
        rla
        rla
		add		a, c
	di //; on interdit les interruptions
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(14)
		out		(P_VDP_ADDR), a
		
		ld		a, 5(ix)     ;// Bits 0-7
		out		(P_VDP_ADDR), a

		ld		a, 6(ix)     ;// Bits 8-13
		and		#0x3F		;// Set 2 last bits to 0
		or		#0x40		;// write access
		out		(P_VDP_ADDR), a

		;// Write value
		ld		a, 7(ix)
	ei //; on autorise les interruptions
		out		(P_VDP_DATA), a

		pop		ix
	__endasm;
}

/** Read a byte in the VRAM */
u8 ReadVRAM(u8 page, u16 addr)
{
	page; addr;

	WaitForVDP();
	
	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		;// Set 0 to register 14 (we don't use address bits 14-16)
		ld		a, 4(ix)     ;// Bits 15
        rla
        rla
		ld		c, a
		ld		a, 6(ix)     ;// Bits 14
		and		#0xC0		;// Keep only 2 last bits
        rla
        rla
        rla
		add		a, c
	di //; on interdit les interruptions
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(14)
		out		(P_VDP_ADDR), a
		
		ld		a, 5(ix)     ;// Bits 0-7
		out		(P_VDP_ADDR), a

		ld		a, 6(ix)     ;// Bits 8-13
		and		#0x3F		;// Set 2 last bits to 0; read access
		out		(P_VDP_ADDR), a

		;// Write value
		in		a, (P_VDP_DATA)
	ei //; on autorise les interruptions
		ld		l, a
		
		pop		ix

	__endasm;
}

/**
 *
 */ 
void VPDCommandLoop(u16 address)
{
	address;

	__asm

		push	ix
		ld		ix, #0
		add		ix, sp

		ld		l, 4(ix)
		ld		h, 5(ix)

		di

	SEND_NEXT_COLOR:

		inc		hl

		;// 3 - envoyer l'octet suivant à mettre en VRAM dans le registre 45 (le premier octet a été traité à l'étape 1) par un OUT du Z80
		;// Send next color
		ld		a, (hl)
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(44)
		out		(P_VDP_ADDR), a

	WAIT_REG2:	

		;// 4 - lire le registre d'état 2 (status)
		;// Get status ragister #2
		ld		a, #2
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(15)
		out		(P_VDP_ADDR), a

		;// 5 - lire le registre d'état du bit CE, si celui-ci est à 0, alors l'instruction est terminée, sinon on passe à l'étape 6
		nop
		in		a, (P_VDP_ADDR)
		ld		b, a ;// backup reg#2
		rra     ;// send CE bit into Carry
		jr		nc, COLOR_COPY_END

		;// 6 - tester l'état du bit TR, si celui-ci se trouve à 0, alors le processeur vidéo n'est pas
		;// prêt à recevoir l'octet suivant, recommencer en 4. Si par contre, ce bit est à 1, reprendre toute l'opération au niveau 3
		ld		a, b ;// restore reg#2
		rla     ;// send TR bit in the Carry
		jr		nc, WAIT_REG2
		jp		SEND_NEXT_COLOR

	COLOR_COPY_END:

		;// Clean status ragister #2
		xor		a
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(15)
		out		(P_VDP_ADDR), a

		ei
		
		pop		ix

	__endasm;
}

///** Should be inline */
//void RAMtoVRAM(u16 dx, u16 dy, u16 nx, u16 ny, u16 ram)
//{
//	VdpBuffer36 buffer;
//	
//	buffer.DX = dx;
//	buffer.DY = dy;
//	buffer.NX = nx;
//	buffer.NY = ny;
//	buffer.CLR = ((u8*)ram)[0];
//	buffer.ARG = 0;
//	buffer.CMD = 0xF0;
//	VPDCommand36((u16)&buffer);
//	VPDCommandLoop(ram);
//}

///** Should be inline */
//void RAMtoVRAMTrans(u16 dx, u16 dy, u16 nx, u16 ny, u16 ram)
//{
//	VdpBuffer36 buffer;
//	
//	buffer.DX = dx;
//	buffer.DY = dy;
//	buffer.NX = nx;
//	buffer.NY = ny;
//	buffer.CLR = ((u8*)ram)[0];
//	buffer.ARG = 0;
//	buffer.CMD = 0xB8;
//	VPDCommand36((u16)&buffer);
//	VPDCommandLoop(ram);
//}

///** Should be inline */
//void FillVRAM(u16 dx, u16 dy, u16 nx, u16 ny, u8 color)
//{
//	VdpBuffer36 buffer;
//	
//	buffer.DX = dx;
//	buffer.DY = dy;
//	buffer.NX = nx;
//	buffer.NY = ny;
//	buffer.CLR = color;
//	buffer.ARG = 0;
//	buffer.CMD = 0xC0;
//	VPDCommand36((u16)&buffer);
//}

///** Should be inline */
//void VRAMtoVRAM(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny)
//{
//	VdpBuffer32 buffer;
//	
//	buffer.SX = sx;
//	buffer.SY = sy;
//	buffer.DX = dx;
//	buffer.DY = dy;
//	buffer.NX = nx;
//	buffer.NY = ny;
//	buffer.CLR = 0;
//	buffer.ARG = 0;
//	buffer.CMD = 0xD0;
//	VPDCommand32((u16)&buffer);
//}

///** Should be inline */
//void VRAMtoVRAMTrans(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny)
//{
//	VdpBuffer32 buffer;
//	
//	buffer.SX = sx;
//	buffer.SY = sy;
//	buffer.DX = dx;
//	buffer.DY = dy;
//	buffer.NX = nx;
//	buffer.NY = ny;
//	buffer.CLR = 0;
//	buffer.ARG = 0;
//	buffer.CMD = 0x98;
//	VPDCommand32((u16)&buffer);
//}

/**
 * Commande VDP (écriture registres 32 à 46)
 */ 
void VPDCommand32(u16 address)
{
	address;

	WaitForVDP();

	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		ld 		l, 4(ix)
		ld 		h, 5(ix)
		//; Envoi données VDP
		ld		a, #32		//; R32 avec incrémentation
	di
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(17)
		out		(P_VDP_ADDR), a         //; Ecriture séquentielle
		ld		c, P_VDP_ADDR+#2         //; Port séquentiel
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
	ei                      //; "EI" anticipé
		outi

		pop		ix

	__endasm;
}

/**
 * Commande VDP (écriture registres 36 à 46)
 */ 
void VPDCommand36(u16 address)
{
	address;

	WaitForVDP();

	__asm
		push	ix
		ld		ix, #0
		add		ix, sp

		ld		l, 4(ix)
		ld		h, 5(ix)
		;// Envoi données VDP
		ld		a, #36		;// R36 avec incrémentation
	di
		out		(P_VDP_ADDR), a
		ld		a, VDP_REG(17)
		out		(P_VDP_ADDR), a         ;// Ecriture séquentielle
		ld		c, P_VDP_ADDR+#2         ;// Port séquentiel
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
		outi
	ei                      ;// "EI" anticipé
		outi

		pop		ix
	__endasm;
}

void PrintSprite(u8 X, u8 Y, const char* text, u16 colorTab)
{
	u8 textIdx = 0, sprtIdx = 0;
	u8 curX = X;
	u8 curY = Y;
	while(text[textIdx] != 0)
	{
		if(text[textIdx] == '\n')
		{
			curX = X;
			curY += 9;
		}
		else
		{
			if(text[textIdx] != ' ')
			{
				SetSpriteMultiColor(sprtIdx, curX, curY, text[textIdx] - ' ', colorTab);
				sprtIdx++;
			}
			curX += 8;
		}
		textIdx++;
	}
	SetSpriteUniColor(sprtIdx, 0, 216, 0, 0);
}

void ClearSprite()
{
	SetSpriteUniColor(0, 0, 216, 0, 0);
}

void SetSpriteMultiColor(u8 index, u8 X, u8 Y, u8 shape, u16 ram)
{
	EntryTAS sprt;
	sprt.posX = X;
	sprt.posY = Y;
	sprt.index = shape;
	RAMtoVRAM((index * 16) & 0x00FF, 244 + (index / 16), 8, 1, ram);
	RAMtoVRAM((index * 4) & 0x00FF, 246 + (index / 64), 3, 1, (u16)&sprt);
}

void SetSpriteUniColor(u8 index, u8 X, u8 Y, u8 shape, u8 color)
{
	EntryTAS sprt;
	sprt.posX = X;
	sprt.posY = Y;
	sprt.index = shape;
	FillVRAM((index * 16) & 0x00FF, 244 + (index / 16), 8, 1, color);
	RAMtoVRAM((index * 4) & 0x00FF, 246 + (index / 64), 3, 1, (u16)&sprt);
}

void RAMtoVRAM(u16 dx, u16 dy, u16 nx, u16 ny, u16 ram) { HMMC(dx, dy, nx, ny, ram); }
void RAMtoVRAMop(u16 dx, u16 dy, u16 nx, u16 ny, u16 ram, u8 op) { LMMC(dx, dy, nx, ny, ram, op); }
void VRAMtoVRAM(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny) { HMMM(sx, sy, dx, dy, nx, ny); }
void VRAMtoVRAMop(u16 sx, u16 sy, u16 dx, u16 dy, u16 nx, u16 ny, u8 op) { LMMM(sx, sy, dx, dy, nx, ny, op); }
//void FillVRAM(u16 dx, u16 dy, u16 nx, u16 ny, u8 col) { HMMV(dx, dy, nx, ny, col); }

void testHMMV(u16 dx, u16 dy, u16 nx, u16 ny, u8 col) { HMMV(dx, dy, nx, ny, col); }
