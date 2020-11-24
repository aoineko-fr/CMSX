//-----------------------------------------------------------------------------
//     _____    _____________  ___.__ 
//    /     \  /   _____/\   \/  /|__|
//   /  \ /  \ \_____  \  \     / |  |
//  /    Y    \/        \ /     \ |  |
//  \____|__  /_______  //___/\  \|__| 
//          \/        \/       \_/     v1.7.1
//
// by Guillaume "Aoineko" Blanchard (aoineko@free.fr)
// available on GitHub (https://github.com/aoineko-fr/MSXImage)
// under CC-BY-AS license (https://creativecommons.org/licenses/by-sa/2.0/)
//-----------------------------------------------------------------------------
#pragma once

// Compression mode
enum Compressor
{
	COMPRESS_None			= 0b00000000, // No compression

	// Crop compression
	COMPRESS_Crop16			= 0b00000001, // Crop sprite to keep only the non-transparent area (max size 16x16)
	COMPRESS_Crop32			= 0b00000010, // Crop sprite to keep only the non-transparent area (max size 32x32)
	COMPRESS_Crop256		= 0b00000011, // Crop sprite to keep only the non-transparent area (max size 256x256)
	COMPRESS_CropLine_Mask	= 0b00001000, // Bits mask
	COMPRESS_CropLine16		= COMPRESS_Crop16 | COMPRESS_CropLine_Mask, // Crop each sprite line (max size 16x16)
	COMPRESS_CropLine32		= COMPRESS_Crop32 | COMPRESS_CropLine_Mask, // Crop each sprite line (max size 32x32)
	COMPRESS_CropLine256	= COMPRESS_Crop256 | COMPRESS_CropLine_Mask, // Crop each sprite line (max size 256x256)
	COMPRESS_Crop_Mask		= 0b00001111, // Bits mask

	// RLE compression
	COMPRESS_RLE0			= 0b00010000, // Run-length encoding of transparent blocs (7-bits for block length)
	COMPRESS_RLE4			= 0b00100000, // Run-length encoding for all colors (4-bits for block length)
	COMPRESS_RLE8			= 0b00110000, // Run-length encoding for all colors (8-bits for block length)
	COMPRESS_RLE_Mask		= 0b00110000, // Bits mask
};

struct MSXiHeader
{
	unsigned short posX;
	unsigned short posY;
	unsigned short sizeX;
	unsigned short sizeY;
	unsigned short numX;
	unsigned short numY;
	unsigned char bpc;
	unsigned char comp;
	unsigned char bSkipEmpty;
};