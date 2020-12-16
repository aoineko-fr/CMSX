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

void MSXi_UnpackToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 compress, u8 screen);

#if USE_MSXi_COMP_NONE			
void MSXi_UnpackNoneToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROP16		
void MSXi_UnpackCrop16ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROP32		
void MSXi_UnpackCrop32ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROP256		
void MSXi_UnpackCrop256ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROPLINE16	
void MSXi_UnpackCropLine16ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROPLINE32	
void MSXi_UnpackCropLine32ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_CROPLINE256	
void MSXi_UnpackCropLine256ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_RLE0			
void MSXi_UnpackRLE0ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_RLE4			
void MSXi_UnpackRLE4ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif

#if USE_MSXi_COMP_RLE8			
void MSXi_UnpackRLE8ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen);
#endif
