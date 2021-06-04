//_____________________________________________________________________________
//   ▄▄   ▄ ▄  ▄▄▄ ▄▄ ▄   ▄▄▄▄           ▄▄         ▄▄ ▄ ▄  ▄▄
//  ██ ▀ ██▀█ ▀█▄  ▀█▄▀    ██  ▄█▀▄ ▄█▀▄ ██   ██▀   ██▄▀ ▄  ██▀
//  ▀█▄▀ ██ █ ▄▄█▀ ██ █    ██  ▀█▄▀ ▀█▄▀ ▀█▄ ▄██    ██ █ ██ ▀█▄
//_____________________________________________________________________________
//
// by Guillaume "Aoineko" Blanchard (aoineko@free.fr)
// available on GitHub (https://github.com/aoineko-fr/CMSXtk)
// under CC-BY-AS license (https://creativecommons.org/licenses/by-sa/2.0/)
//_____________________________________________________________________________
#pragma once

/// Format of the data
enum CMSX_FileFormat
{
	FORMAT_Auto,                    // Auto-detection
	FORMAT_C,					    // C header file (text)
	FORMAT_Asm,					    // Assembler header file (text)
	FORMAT_Bin,					    // Binary data file
};

/// Format of the data
enum CMSX_DataFormat
{
	DATA_Decimal,					// 0, 210, 65535
	DATA_Hexa,					    //
	DATA_HexaC,					    // 0x00, 0xD2, 0xFFFF
	DATA_HexaASM,					// 00h, 0D2h, 0FFFFh
	DATA_HexaPascal,				// $00, $D2, $FFFF
	DATA_HexaBasic,				    // &H00, &HD2, &HFFFF
	DATA_HexaAnd,					// &00, &D2, &FFFF
	DATA_HexaSharp,				    // #00, #D2, #FFFF
	DATA_HexaRaw,					// 00, D2, FFFF
	DATA_Binary,					//
	DATA_BinaryC,					// 0b00000000, 0b11010010, 0b1111111111111111
	DATA_BinaryASM,				    // 00000000b, 11010010b, 1111111111111111b
};

/// Size of the data
enum CMSX_DataSize
{
	SIZE_8bits  = 1,
	SIZE_16bits = 2,
	SIZE_32bits = 4,
	SIZE_64bits = 8,
};

/// 
inline const c8* CMSX_GetDataFormat(CMSX_DataFormat format, u8 size = 1)
{
	if (size == SIZE_64bits)
		format = DATA_Decimal;

	switch (format)
	{
	default:
	case DATA_Decimal:
		return "%d";

	case DATA_Hexa:
	case DATA_HexaC:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "0x%02X";
		case SIZE_16bits:	return "0x%04X";
		case SIZE_32bits:	return "0x%08X";
		}

	case DATA_HexaASM:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "0%02Xh";
		case SIZE_16bits:	return "0%04Xh";
		case SIZE_32bits:	return "0%08Xh";
		}
	case DATA_HexaPascal:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "$%02X";
		case SIZE_16bits:	return "$%04X";
		case SIZE_32bits:	return "$%08X";
		}
	case DATA_HexaBasic:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "&H%02X";
		case SIZE_16bits:	return "&H%04X";
		case SIZE_32bits:	return "&H%08X";
		}
	case DATA_HexaAnd:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "&%02X";
		case SIZE_16bits:	return "&%04X";
		case SIZE_32bits:	return "&%08X";
		}
	case DATA_HexaSharp:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "#%02X";
		case SIZE_16bits:	return "#%04X";
		case SIZE_32bits:	return "#%08X";
		}
	case DATA_HexaRaw:
		switch (size)
		{
		default:
		case SIZE_8bits:	return "%02X";
		case SIZE_16bits:	return "%04X";
		case SIZE_32bits:	return "%08X";
		}


	case DATA_Binary:
	case DATA_BinaryC:
	case DATA_BinaryASM:
		return "%u";
	}
}

/// 
enum DataSeparator
{
	SEPARATOR_Comma,				// 0x00, 0xD2, 0xFF
	SEPARATOR_Space,				// 0x00 0xD2 0xFF
};

/// 
enum CMSX_Section
{
	SECTION_U8,						// 8-bits unsigned interger
	SECTION_U16,					// 16-bits unsigned interger
	SECTION_U32,					// 32-bits unsigned interger
	SECTION_U64,					// 64-bits unsigned interger
	SECTION_S8,						// 8-bits signed interger
	SECTION_S16,					// 16-bits signed interger
	SECTION_S32,					// 32-bits signed interger
	SECTION_S64,					// 64-bits signed interger
	SECTION_F32,					// 32-bits floating-point
	SECTION_F64,					// 64-bits floating-point
};

/// 
inline const c8* CMSX_GetSectionC(CMSX_Section format)
{
	switch(format)
	{
	default:
	case SECTION_U8:  return "const unsigned char";
	case SECTION_U16: return "const unsigned short";
	case SECTION_U32: return "const unsigned long";
	case SECTION_U64: return "const unsigned _int64";
	case SECTION_S8:  return "const signed char";
	case SECTION_S16: return "const signed short";
	case SECTION_S32: return "const signed long";
	case SECTION_S64: return "const signed _int64";
	case SECTION_F32: return "const float";
	case SECTION_F64: return "const double";
	}
 }