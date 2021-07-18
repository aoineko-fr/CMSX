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

// std
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdarg>

namespace CMSX {

#define BUFFER_SIZE 1024

	/// Exporter interface
	class ExporterInterface
	{
	public:
		ExporterInterface() {}

		virtual void AddComment(std::string comment = "") = 0;
		virtual void StartSection(std::string name, std::string comment = "") = 0;
		virtual void EndSection(std::string comment = "") = 0;
		virtual void StartLine(std::string name, std::string comment = "") = 0;
		virtual void EndLine(std::string comment = "") = 0;
		virtual void AddByte(u8 data) = 0;
		virtual void AddWord(u16 data) = 0;
		virtual void AddDouble(u32 data) = 0;
		virtual void AddByteList(std::vector<u8> data, std::string comment = "") = 0;
		virtual void AddWordList(std::vector<u16> data, std::string comment = "") = 0;
		virtual void AddDoubleList(std::vector<u32> data, std::string comment = "") = 0;

		virtual bool Export(std::string filename) const = 0;
		virtual u32 GetTotalSize() const = 0;
	};

	/// Text exporter configuration
	struct ExporterConfig
	{
		DataType		Type;
		DataFormat		Format;
		DataSeparator	Separator;
		u64				Address = -1;
	};

	/// Text exporter interface
	class ExporterText : public ExporterInterface
	{
	protected:
		ExporterConfig   Config;
		std::string      OutData;
		u32              DataSize;

	public:
		ExporterText(const ExporterConfig& c) : ExporterInterface(), Config(c) {}

		virtual DataType GetDataType() const { return Config.Type; }
		virtual DataFormat GetDataFormat() const { return Config.Format; }

		virtual void AddComment(std::string comment = "") = 0;
		virtual void StartSection(std::string name, std::string comment = "") = 0;
		virtual void EndSection(std::string comment = "") = 0;
		virtual void StartLine(std::string name, std::string comment = "") = 0;
		virtual void EndLine(std::string comment = "") = 0;
		virtual void AddByte(u8 data) = 0;
		virtual void AddWord(u16 data) = 0;
		virtual void AddDouble(u32 data) = 0;
		virtual void AddByteList(std::vector<u8> list, std::string comment = "")
		{
			for (u8& data : list)
			{
				AddByte(data);
			}
			if (!comment.empty())
				AddComment(comment);
		}
		virtual void AddWordList(std::vector<u16> list, std::string comment = "")
		{
			for (u16& data : list)
			{
				AddWord(data);
			}
			if (!comment.empty())
				AddComment(comment);
		}
		virtual void AddDoubleList(std::vector<u32> list, std::string comment = "")
		{
			for (u32& data : list)
			{
				AddDouble(data);
			}
			if (!comment.empty())
				AddComment(comment);
		}

		virtual bool Export(std::string filename) const
		{
			// Write header file
			FILE* file;
			if (fopen_s(&file, filename.c_str(), "wb") != 0)
			{
				printf("Error: Fail to create %s\n", filename.c_str());
				return false;
			}
			fwrite(OutData.c_str(), 1, OutData.size(), file);
			fclose(file);
			return true;
		}

		virtual u32 GetTotalSize() const { return DataSize; }
	};

	/// C langage exporter
	class ExporterC : public ExporterText
	{
	public:
		ExporterC(ExporterConfig c) : ExporterText(c) {}

		///
		virtual void AddComment(std::string comment = "")
		{
			OutData += CMSX::Format("//%s\n", comment.c_str());
		}

		///
		virtual void StartSection(std::string name, std::string comment = "")
		{
			if (!comment.empty())
				AddComment(comment);
			if(Config.Address != -1)
				OutData += CMSX::Format("__at(0x%X) ", Config.Address + GetTotalSize());
			OutData += CMSX::Format("%s %s[] = {\n", GetCTableType(Config.Type), name.c_str());
		}

		///
		virtual void EndSection(std::string comment = "")
		{
			OutData += "};\n";
			if (!comment.empty())
				AddComment(comment);
		}

		///
		virtual void StartLine(std::string name, std::string comment = "")
		{
		}

		///
		virtual void EndLine(std::string comment = "")
		{
		}

		///
		virtual void AddByte(u8 data)
		{
			DataSize++;
		}

		///
		virtual void AddWord(u16 data)
		{
			DataSize += 2;
		}

		///
		virtual void AddDouble(u32 data)
		{
			DataSize += 4;
		}
	};


	/// Assembler langage exporter
	/*class ExporterASM: public ExporterText
	{
	public:
		ExporterASM(DataFormat f, ExportParameters* p): ExporterText(f, p) {}

		virtual void WriteTableBegin(TableFormat format, std::string name, std::string comment = "")
		{
			sprintf_s(strData, BUFFER_SIZE,
				"\n"
				"; %s\n"
				"%s:\n",
				comment.c_str(), name.c_str());
			outData += strData;
		}

		virtual void WriteSpriteHeader(i32 number)
		{
			sprintf_s(strData, BUFFER_SIZE,
				"; Sprite[%i] (offset:%i)\n", number, TotalBytes);
			outData += strData;
		}

		virtual void WriteCommentLine(std::string comment = "")
		{
			sprintf_s(strData, BUFFER_SIZE, "; %s\n", comment.c_str());
			outData += strData;
		}

		virtual void Write4BytesLine(u8 a, u8 b, u8 c, u8 d, std::string comment = "")
		{
			sprintf_s(strFormat, BUFFER_SIZE,
				"\t.db %s %s %s %s ; %s\n", GetNumberFormat(), GetNumberFormat(), GetNumberFormat(), GetNumberFormat(), comment.c_str());
			sprintf_s(strData, BUFFER_SIZE, strFormat, a, b, c, d);
			outData += strData;
			TotalBytes += 4;
		}

		virtual void Write2BytesLine(u8 a, u8 b, std::string comment = "")
		{
			sprintf_s(strFormat, BUFFER_SIZE,
				"\t.db %s %s ; %s\n", GetNumberFormat(), GetNumberFormat(), comment.c_str());
			sprintf_s(strData, BUFFER_SIZE, strFormat, a, b);
			outData += strData;
			TotalBytes += 2;
		}

		virtual void Write1ByteLine(u8 a, std::string comment = "")
		{
			sprintf_s(strFormat, BUFFER_SIZE,
				"\t.db %s ; %s\n", GetNumberFormat(), comment.c_str());
			sprintf_s(strData, BUFFER_SIZE, strFormat, a);
			outData += strData;
			TotalBytes += 1;
		}

		virtual void Write1WordLine(u16 a, std::string comment = "")
		{
			sprintf_s(strFormat, BUFFER_SIZE,
				"\t.dw %s ; %s\n", GetNumberFormat(2), comment.c_str());
			sprintf_s(strData, BUFFER_SIZE, strFormat, a);
			outData += strData;
			TotalBytes += 2;
		}

		virtual void Write2WordsLine(u16 a, u16 b, std::string comment = "")
		{
			sprintf_s(strFormat, BUFFER_SIZE,
				"\t.dw %s %s ; %s\n", GetNumberFormat(), GetNumberFormat(), comment.c_str());
			sprintf_s(strData, BUFFER_SIZE, strFormat, a, b);
			outData += strData;
			TotalBytes += 4;
		}

		virtual void WriteLineBegin()
		{
			outData += "\t.db ";
		}

		virtual void Write1ByteData(u8 data)
		{
			sprintf_s(strFormat, BUFFER_SIZE, "%s ", GetNumberFormat());
			sprintf_s(strData, BUFFER_SIZE, strFormat, data);
			outData += strData;
			TotalBytes += 1;
		}

		virtual void Write8BitsData(u8 data)
		{
			sprintf_s(strFormat, BUFFER_SIZE, "%s ", GetNumberFormat());
			sprintf_s(strData, BUFFER_SIZE, strFormat, data);
			outData += strData;
			TotalBytes += 1;
		}

		virtual void WriteLineEnd()
		{
			outData += "\n";
		}

		virtual void WriteTableEnd(std::string comment = "")
		{
			if (comment != "")
			{
				sprintf_s(strData, BUFFER_SIZE,
					"; %s\n", comment.c_str());
				outData += strData;
			}
		}
	};*/

	/// Binary exporter
	/*class ExporterBin: public ExporterInterface
	{
	protected:
	#define BUFFER_SIZE 1024
		std::vector<u8> outData;

	public:
		ExporterBin(DataFormat f, ExportParameters* p) : ExporterInterface(f, p) {}
		virtual void WriteHeader() {}
		virtual void WriteTableBegin(TableFormat format, std::string name, std::string comment = "") {}
		virtual void WriteSpriteHeader(i32 number) {}
		virtual void WriteCommentLine(std::string comment = "") {}
		virtual void Write1ByteLine(u8 a, std::string comment = "")
		{
			outData.push_back(a);
			TotalBytes += 1;
		}
		virtual void Write2BytesLine(u8 a, u8 b, std::string comment = "")
		{
			outData.push_back(a);
			outData.push_back(b);
			TotalBytes += 2;
		}
		virtual void Write4BytesLine(u8 a, u8 b, u8 c, u8 d, std::string comment = "")
		{
			outData.push_back(a);
			outData.push_back(b);
			outData.push_back(c);
			outData.push_back(d);
			TotalBytes += 4;
		}
		virtual void Write1WordLine(u16 a, std::string comment = "")
		{
			outData.push_back(a & 0x00FF);
			outData.push_back(a >> 8);
			TotalBytes += 2;
		}
		virtual void Write2WordsLine(u16 a, u16 b, std::string comment = "")
		{
			outData.push_back(a & 0x00FF);
			outData.push_back(a >> 8);
			outData.push_back(b & 0x00FF);
			outData.push_back(b >> 8);
			TotalBytes += 4;
		}
		virtual void WriteLineBegin() {}
		virtual void Write1ByteData(u8 data)
		{
			outData.push_back(data);
			TotalBytes += 1;
		}
		virtual void Write8BitsData(u8 data)
		{
			outData.push_back(data);
			TotalBytes += 1;
		}
		virtual void WriteLineEnd() {}
		virtual void WriteTableEnd(std::string comment = "") {}

		virtual const c8* GetNumberFormat(u8 bytes = 1) { return NULL; }

		virtual bool Export()
		{
			// Write header file
			FILE* file;
			if (fopen_s(&file, Param->outFile.c_str(), "wb") != 0)
			{
				printf("Error: Fail to create %s\n", Param->outFile.c_str());
				return false;
			}
			fwrite(outData.data(), 1, outData.size(), file);
			fclose(file);
			return true;
		}
	};*/

	/// Dummy exporter
	/*class ExporterDummy : public ExporterInterface
	{
	public:
		ExporterDummy(DataFormat f, ExportParameters* p) : ExporterInterface(f, p) {}
		virtual void WriteHeader() {}
		virtual void WriteTableBegin(TableFormat format, std::string name, std::string comment = "") {}
		virtual void WriteSpriteHeader(i32 number) {}
		virtual void WriteCommentLine(std::string comment = "") {}
		virtual void Write1ByteLine(u8 a, std::string comment = "") { TotalBytes += 1; }
		virtual void Write2BytesLine(u8 a, u8 b, std::string comment = "") { TotalBytes += 2; }
		virtual void Write4BytesLine(u8 a, u8 b, u8 c, u8 d, std::string comment = "") { TotalBytes += 4; }
		virtual void Write1WordLine(u16 a, std::string comment = "") { TotalBytes += 2; }
		virtual void Write2WordsLine(u16 a, u16 b, std::string comment = "") { TotalBytes += 4; }
		virtual void WriteLineBegin() {}
		virtual void Write1ByteData(u8 data) { TotalBytes += 1; }
		virtual void Write8BitsData(u8 data) { TotalBytes += 1;	}
		virtual void WriteLineEnd() {}
		virtual void WriteTableEnd(std::string comment = "") {}
		virtual const c8* GetNumberFormat(u8 bytes = 1) { return NULL; }
		virtual bool Export() { return true; }
	};*/

} // namespace CMSX