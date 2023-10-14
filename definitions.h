#pragma once
#include <cstdio>
#include <cinttypes>
#include<fstream>
#include<string>
#include<vector>
//#include"zlib.h"
//#include"zlib/deflate.h"
//#include"zlib/inflate.h"

namespace Xertz
{
#define MODULE std::pair<std::wstring, unsigned long long>
#define MODULE_LIST std::vector<std::pair<std::wstring, unsigned long long>>
#define REGION_LIST std::vector<Xertz::MemoryRegion>
#define PROCESS_INFO Xertz::ProcessInfo
#define PROCESS_INFO_LIST std::vector<Xertz::ProcessInfo>
#define WINDOW_HANDLE_LIST std::vector<HWND>
	

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

	//a person with undefined behavior may create code with undefined behavior
	template<typename InType, typename OutType> static inline OutType ReinterpreteType(const InType val)
	{
		return *reinterpret_cast<OutType*>(&val);
	}

	static bool SaveBinary(const std::wstring& filePath, void* data, const uint64_t size, const bool append = false, const bool zip = false)
	{
		std::ofstream file(filePath, std::ios::binary | (append ? std::ios_base::app : std::ios::out));
		if (!file)
			return false;
		
		/*if (zip)
		{
			const int CHUNK_SIZE = 0x4000;
			char out_buffer[CHUNK_SIZE];

			z_stream strm;
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			deflateInit(&strm, Z_DEFAULT_COMPRESSION);

			strm.avail_in = size;
			strm.next_in = reinterpret_cast<Bytef*>((char*)data);

			int bytes_written;
			do {
				strm.avail_out = CHUNK_SIZE;
				strm.next_out = reinterpret_cast<Bytef*>(out_buffer);
				deflate(&strm, Z_FINISH);
				bytes_written = CHUNK_SIZE - strm.avail_out;
				file.write(out_buffer, bytes_written);
			} while (strm.avail_out == 0);

			deflateEnd(&strm);
			file.close();

			return true;
		}*/

		file.write((char*)data, size);
		file.close();

		return true;
	}

	static bool LoadBinary(const std::wstring& filePath, void*& out, uint64_t& size, const uint64_t startPos = 0)
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
				out = malloc(size);
			}
			else //read fraction
			{
				out = malloc(size);
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