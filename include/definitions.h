#pragma once
#include <cstdio>
#include <cinttypes>
#include<fstream>
#include<string>

namespace Xertz
{
#define MODULE std::pair<std::wstring, unsigned long long>
#define MODULE_LIST std::vector<std::pair<std::wstring, unsigned long long>>
#define REGION_LIST std::vector<Xertz::MemoryRegion>
#define PROCESS_INFO Xertz::ProcessInfo
#define PROCESS_INFO_LIST std::vector<Xertz::ProcessInfo>

	enum StringDefs
	{
		NO_SUBSTRING = 0,
		IS_SUBSTRING = 1,
		CASE_SENSITIVE = 2,
		CASE_INSENSITIVE = 3
	};

	enum Audio
	{
		SCALAR = 0,
		DECIBEL = 1,
		MUTE = 2,
		UNMUTE = 3
	};

	template<typename T> static inline T SwapBytes(const T val)
	{
		T temp = 0;
		char buf;

		for (int i = 0; i < sizeof(val); ++i)
		{
			buf = *((char*)(&val) + i);
			*(reinterpret_cast<char*>(&temp) + sizeof(val) - (1 + i)) = buf;
		}
		return temp;
	}

	template<typename InType, typename OutType> static inline OutType ReinterpreteType(InType val)
	{
		return *reinterpret_cast<OutType*>(&val);
	}

	static bool SaveBinary(std::wstring& filePath, void* data, uint64_t size, bool append = false)
	{
		std::ofstream file(filePath, std::ios::binary | (append ? std::ios_base::app : std::ios::out));
		if (file)
		{
			file.write((char*)data, size);
			file.close();
			return true;
		}
		return false;
	}

	static bool LoadBinary(std::wstring& filePath, void* out, uint64_t size, uint64_t startPos)
	{
		std::ifstream file;
		file.open(filePath, std::ios::binary | std::ios::in);

		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			uint64_t end = file.tellg();
			file.seekg(0, std::ios::beg);

			if (size == 0 && startPos == 0) //read all
			{
				size = end;
			}
			else //read fraction
			{
				uint64_t readableSize = end - startPos;
				file.seekg(startPos, std::ios::cur);
				
				if (readableSize < size || size == 0)
				{
					size = readableSize;
				}
			}

			if (!file.read((char*)out, size))
			{
				return false;
			}
		}
		file.close();
		return true;
	}
}