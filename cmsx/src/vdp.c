//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
#include "vdp.h"
#include "ports.h"

enum VDP_MODE
{
	// VDP modes
	VDP_MODE_TEXT1 = 0,		// 00001	40 characters per line of text, one colour for each characte
	VDP_MODE_MULTICOLOR,	// 00010	pseudo-graphic, one character divided into four block
	VDP_MODE_GRAPHIC1,		// 00000	32 characters per one line of text, the COLOURed character available
	VDP_MODE_GRAPHIC2,		// 00100	256 x 192, the colour is specififed for each 8 dots
#if (MSX_VERSION >= MSX_2)
	VDP_MODE_TEXT2,			// 01001	80 characters per line of text, character blinkable selection
	VDP_MODE_GRAPHIC3,		// 01000	GRAPHIC 2 which can use sprite mode 2
	VDP_MODE_GRAPHIC4,		// 01100	256 x 212; 16 colours are available for each dot
	VDP_MODE_GRAPHIC5,		// 10000	512 x 212; 4 colours are available for each dot
	VDP_MODE_GRAPHIC6,		// 10100	512 x 212; 16 colours are available for each dot
	VDP_MODE_GRAPHIC7,		// 11100	256 x 212; 256 colours are available for each dot
#endif

	// BASIC screens
	VDP_MODE_SCREEN0,		// VDP_MODE_TEXT1
	VDP_MODE_SCREEN0_W40 = VDP_MODE_SCREEN0,
	VDP_MODE_SCREEN3,		// VDP_MODE_MULTICOLOR
	VDP_MODE_SCREEN1,		// VDP_MODE_GRAPHIC1
	VDP_MODE_SCREEN2,		// VDP_MODE_GRAPHIC2
#if (MSX_VERSION >= MSX_2)
	VDP_MODE_SCREEN0_W80,	// VDP_MODE_TEXT2,	
	VDP_MODE_SCREEN4,       // VDP_MODE_GRAPHIC3
	VDP_MODE_SCREEN5,       // VDP_MODE_GRAPHIC4
	VDP_MODE_SCREEN6,       // VDP_MODE_GRAPHIC5
	VDP_MODE_SCREEN7,       // VDP_MODE_GRAPHIC6
	VDP_MODE_SCREEN8,       // VDP_MODE_GRAPHIC7
	VDP_MODE_SCREEN9,
	VDP_MODE_SCREEN9_40 = VDP_MODE_SCREEN9,
	VDP_MODE_SCREEN9_80,
#endif
#if (MSX_VERSION >= MSX_2Plus)
	VDP_MODE_SCREEN10,
	VDP_MODE_SCREEN11,
	VDP_MODE_SCREEN12,
#endif
};

#define VDP_REG(_r) (0x80 + _r)

u8 REG00SAV;
u8 REG01SAV;
u8 REG02SAV;
u8 REG03SAV;
u8 REG04SAV;
u8 REG05SAV;
u8 REG06SAV;
u8 REG07SAV;
#if (MSX_VERSION >= MSX_2)
u8 REG08SAV;
u8 REG09SAV;
u8 REG10SAV;
u8 REG11SAV;
u8 REG12SAV;
u8 REG13SAV;
u8 REG14SAV;
u8 REG15SAV;
u8 REG16SAV;
u8 REG17SAV;
u8 REG18SAV;
u8 REG19SAV;
u8 REG20SAV;
u8 REG21SAV;
u8 REG22SAV;
u8 REG23SAV;
u8 REG24SAV;
#endif
#if (MSX_VERSION >= MSX_2Plus)
u8 REG25SAV;
u8 REG26SAV;
u8 REG27SAV;
#endif

//-----------------------------------------------------------------------------
//
static const u8 modeT1[] = 
{
	0x00, VDP_REG(0), // Text Mode, No External Video
	0xD0, VDP_REG(1), // 16K, Enable Disp., Disable Int.
	0x02, VDP_REG(2), // Address of Name Table in VRAM = 0800h
	//    VDP_REG(3), // (not used) Color is defined in Reg. 7
	0x00, VDP_REG(4), // Address of Patern Table in VRAM = 0000h
	//    VDP_REG(5), // (not used)
	//    VDP_REG(6), // (not used)
	0xF5, VDP_REG(7), // White Text on Light Blue Background
};
void VDP_SetModeText1()
{
	__asm
		ld		hl, #_modeT1
		ld		b, #10
		ld		c, P_VDP_ADDR
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
static const u8 modeMC[] = 
{
	0x00, VDP_REG(0), // Multicolor Mode, No External Video
	0xCB, VDP_REG(1), // 16K, Enable Disp., Disable Int., 16x16 Sprites, Mag. On
	0x05, VDP_REG(2), // Address of Name Table in VRAM = 1400h
	//    VDP_REG(3), // (not used)
	0x01, VDP_REG(4), // Address of Patern Table in VRAM = 0800h
	0x20, VDP_REG(5), // Address of Sprite Attribute Table in VRAM = 1000h
	0x00, VDP_REG(6), // Address of Sprite Pattern Table in VRAM = 0000h
	0x04, VDP_REG(7), // Backdrop Color = Dark Blue
};
void VDP_SetModeMultiColor()
{
	__asm
		ld		hl, #_modeMC
		ld		b, #14
		ld		c, P_VDP_ADDR
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
static const u8 modeG1[] = 
{
	0x00, VDP_REG(0), // Graphics 1 Mode, No External Video
	0xC0, VDP_REG(1), // 16K, Enable display, Disable Int, 8x8 Srpites, Mag. off
	0x05, VDP_REG(2), // Address of Name Table in VRAM = 1400h
	0x80, VDP_REG(3), // Address of Color Table in VRAM = 2000h
	0x01, VDP_REG(4), // Address of Patern Table in VRAM = 0800h
	0x20, VDP_REG(5), // Address of Sprite Attribute Table in VRAM = 1000h
	0x00, VDP_REG(6), // Address of Sprite Pattern Table in VRAM = 0000h
	0x01, VDP_REG(7), // Backdrop Color = Black
};
void VDP_SetModeGraphic1()
{
	__asm
		ld		hl, #_modeG1
		ld		b, #16
		ld		c, P_VDP_ADDR
		otir
	__endasm;
}

//-----------------------------------------------------------------------------
//
static const u8 modeG2[] = 
{
	0x02, VDP_REG(0), // Graphics 2 Mode, No External Video
	0xC2, VDP_REG(1), // 16K, Enable Disp., Disable Int., 16x16 Sprites, Mag. Off
	0x0E, VDP_REG(2), // Address of Name Table in VRAM = 3800h
	0xFF, VDP_REG(3), // Address of Color Table in VRAM = 2000h
	0x03, VDP_REG(4), // Address of Patern Table in VRAM = 0000h
	0x76, VDP_REG(5), // Address of Sprite Attribute Table in VRAM = 3B00h
	0x03, VDP_REG(6), // Address of Sprite Pattern Table in VRAM = 1800h
	0x0F, VDP_REG(7), // Backdrop Color = White
};
void VDP_SetModeGraphic2()
{
	__asm
		ld		hl, #_modeG2
		ld		b, #16
		ld		c, P_VDP_ADDR
		otir
	__endasm;
}

#if (MSX_VERSION >= MSX_2)

//-----------------------------------------------------------------------------
//
void VDP_SetModeText2()
{
}

//-----------------------------------------------------------------------------
//
void VDP_SetModeGraphic3()
{
}

//-----------------------------------------------------------------------------
//
void VDP_SetModeGraphic4()
{
}

//-----------------------------------------------------------------------------
//
void VDP_SetModeGraphic5()
{
}

//-----------------------------------------------------------------------------
//
void VDP_SetModeGraphic6()
{
}

//-----------------------------------------------------------------------------
//
void VDP_SetModeGraphic7()
{
}

#endif

//-----------------------------------------------------------------------------
//
inline void VDP_SetScreen(u8 mode)
{
	switch(mode)
	{
	//case VDP_MODE_SCREEN0:
	case VDP_MODE_SCREEN0_W40:
	case VDP_MODE_TEXT1:
		VDP_SetModeText1();
		return;
	
	case VDP_MODE_SCREEN3:
	case VDP_MODE_MULTICOLOR:
		VDP_SetModeMultiColor();
		return;

	case VDP_MODE_SCREEN1:
	case VDP_MODE_GRAPHIC1:
		VDP_SetModeGraphic1();
		return;

	case VDP_MODE_SCREEN2:
	case VDP_MODE_GRAPHIC2:
		VDP_SetModeGraphic2();
		return;

#if (MSX_VERSION >= MSX_2)
	case VDP_MODE_SCREEN0_W80:
	case VDP_MODE_TEXT2:
		VDP_SetModeText2();
		return;

	case VDP_MODE_SCREEN4:
	case VDP_MODE_GRAPHIC3:
		VDP_SetModeGraphic3();
		return;
	
	case VDP_MODE_SCREEN5:
	case VDP_MODE_GRAPHIC4:
		VDP_SetModeGraphic4();
		return;
	
	case VDP_MODE_SCREEN6:
	case VDP_MODE_GRAPHIC5:
		VDP_SetModeGraphic5();
		return;
	
	case VDP_MODE_SCREEN7:
	case VDP_MODE_GRAPHIC6:
		VDP_SetModeGraphic6();
		return;
		
	case VDP_MODE_SCREEN8:
	case VDP_MODE_GRAPHIC7:
		VDP_SetModeGraphic7();
		return;
		
	// case VDP_MODE_SCREEN9:
	case VDP_MODE_SCREEN9_40:
		VDP_SetModeGraphic5();
		// @todo Further setting needed
		return;
	
	case VDP_MODE_SCREEN9_80:
		VDP_SetModeGraphic4();
		// @todo Further setting needed
		return;

#endif
#if (MSX_VERSION >= MSX_2Plus)
	case VDP_MODE_SCREEN10:
		VDP_SetModeGraphic7();
		// @todo Further setting needed
		return;

	case VDP_MODE_SCREEN11:
		VDP_SetModeGraphic7();
		// @todo Further setting needed
		return;

	case VDP_MODE_SCREEN12:
		VDP_SetModeGraphic7();
		// @todo Further setting needed
		return;
#endif
	}
}