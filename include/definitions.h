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
}