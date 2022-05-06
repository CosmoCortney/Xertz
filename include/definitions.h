#pragma once
#include <cstdio>
#include <cinttypes>

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
}