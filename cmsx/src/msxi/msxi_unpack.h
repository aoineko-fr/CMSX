//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// MSXi UPACK FUNCTIONS
//
// References:
// - https://github.com/aoineko-fr/MSXImage
//-----------------------------------------------------------------------------
#pragma once

#include "msxi/msxi.h"

enum MSXi_Destination
{
	DEST_RAM = 0,
	DEST_VRAM,
};

void MSXi_UnpackToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 compress, u8 screen);

#if USE_MSXi_COMP_NONE			
void MSXi_UnpackNoneToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROP16		
void MSXi_UnpackCrop16ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROP32		
void MSXi_UnpackCrop32ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROP256		
void MSXi_UnpackCrop256ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROPLINE16	
void MSXi_UnpackCropLine16ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROPLINE32	
void MSXi_UnpackCropLine32ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROPLINE256	
void MSXi_UnpackCropLine256ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_RLE0			
void MSXi_UnpackRLE0ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_RLE4			
void MSXi_UnpackRLE4ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_RLE8			
void MSXi_UnpackRLE8ToVRAM(u16 src, u8 destX, u8 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif
