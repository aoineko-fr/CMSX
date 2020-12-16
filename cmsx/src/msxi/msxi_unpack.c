//-----------------------------------------------------------------------------
//  █▀▀ █▀▄▀█ █▀ ▀▄▀
//  █▄▄ █ ▀ █ ▄█ █ █ v0.2
//-----------------------------------------------------------------------------
// MSXi UPACK FUNCTIONS
//
// References:
// - https://github.com/aoineko-fr/MSXImage
//-----------------------------------------------------------------------------
#include "core.h"
#include "bios.h"
#include "bios_main.h"
#include "vdp.h"
#include "msxi/msxi_unpack.h"

//-----------------------------------------------------------------------------
// Unpack compressed image into VRAM
void MSXi_UnpackToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 compress, u8 screen)
{
	switch(compress)
	{
#if USE_MSXi_COMP_NONE
	case COMPRESS_None:			return MSXi_UnpackNoneToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_CROP16
	case COMPRESS_Crop16:       return MSXi_UnpackCrop16ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_CROP32
	case COMPRESS_Crop32:       return MSXi_UnpackCrop32ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_CROP256
	case COMPRESS_Crop256:      return MSXi_UnpackCrop256ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_CROPLINE16
	case COMPRESS_CropLine16:   return MSXi_UnpackCropLine16ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_CROPLINE32
	case COMPRESS_CropLine32:   return MSXi_UnpackCropLine32ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_CROPLINE256
	case COMPRESS_CropLine256:  return MSXi_UnpackCropLine256ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_RLE0
	case COMPRESS_RLE0:         return MSXi_UnpackRLE0ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_RLE4
	case COMPRESS_RLE4:         return MSXi_UnpackRLE4ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
#if USE_MSXi_COMP_RLE8
	case COMPRESS_RLE8:         return MSXi_UnpackRLE8ToVRAM(src, destX, destY, sizeX, sizeY, numX, numY, screen);
#endif
	};
}

#if USE_MSXi_COMP_NONE			
//-----------------------------------------------------------------------------
void MSXi_UnpackNoneToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_NONE

#if USE_MSXi_COMP_CROP16		
//-----------------------------------------------------------------------------
void MSXi_UnpackCrop16ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_CROP16

#if USE_MSXi_COMP_CROP32		
//-----------------------------------------------------------------------------
void MSXi_UnpackCrop32ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_CROP32

#if USE_MSXi_COMP_CROP256		
//-----------------------------------------------------------------------------
void MSXi_UnpackCrop256ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_CROP256

#if USE_MSXi_COMP_CROPLINE16	
//-----------------------------------------------------------------------------
void MSXi_UnpackCropLine16ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	screen;
	u8* ptr = (u8*)src;
	u8 pageY = 0;
	if(destY >= 512)
	{
		destY -= 512;
		pageY = 1;
	}
	
	for(i8 j = 0; j < numY; j++)
	{
		for(i8 i = 0; i < numX; i++)
		{
			u8 minY = *ptr >> 4;
			u8 maxY = *ptr & 0x0F;
			ptr++;
			for(i8 y = minY; y <= maxY; y++)
			{
				u8 minX = *ptr >> 4;
				u8 maxX = *ptr & 0x0F;
				ptr++;
				u8 len = (maxX - minX + 1);
				if(len & 0x1)
					len++;
				len /= 2;
#if (RENDER_MODE == RENDER_VDP)
				VDP_WriteVRAM(ptr, ((destY + y + (j * sizeY)) * 128) + ((destX + (i * sizeX) + minX) >> 1), pageY, len);
#elif (RENDER_MODE == RENDER_BIOS)
				Bios_TransfertRAMtoVRAM((u16)ptr, ((destY + y + (j * sizeY)) * 128) + ((destX + (i * sizeX) + minX) >> 1), len);
#endif
				ptr += len;
			}
		}
	}
}
#endif // USE_MSXi_COMP_CROPLINE16

#if USE_MSXi_COMP_CROPLINE32	
//-----------------------------------------------------------------------------
void MSXi_UnpackCropLine32ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_CROPLINE32

#if USE_MSXi_COMP_CROPLINE256	
//-----------------------------------------------------------------------------
void MSXi_UnpackCropLine256ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_CROPLINE256

#if USE_MSXi_COMP_RLE0			
//-----------------------------------------------------------------------------
void MSXi_UnpackRLE0ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_RLE0

#if USE_MSXi_COMP_RLE4			
//-----------------------------------------------------------------------------
void MSXi_UnpackRLE4ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_RLE4

#if USE_MSXi_COMP_RLE8			
//-----------------------------------------------------------------------------
void MSXi_UnpackRLE8ToVRAM(void* src, u16 destX, u16 destY, u8 sizeX, u8 sizeY, u8 numX, u8 numY, u8 screen)
{
	src, destX, destY, sizeX, sizeY, numX, numY, screen;
}
#endif // USE_MSXi_COMP_RLE8
