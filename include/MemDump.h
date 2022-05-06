#pragma once
#include"definitions.h"
#include<string>
#include<Windows.h>

namespace Xertz
{
	class MemDump
	{
	private:
		uint64_t _baseAddress = 0;
		uint64_t _startingAddress = 0;
		uint64_t _memSize = 0;
		void* _memDump = nullptr;
		std::wstring _filePath;
		HANDLE _handle;
		bool _hasHandle = false;

		void DumpExRAM();
		MemDump() = delete;

	public:
		MemDump(HANDLE handle, uint64_t baseAddress, uint64_t startingAddress, uint64_t memSize);
		MemDump(std::wstring& path, uint64_t baseAddress, uint64_t startingAddress, uint64_t size = 0, uint64_t pos = 0);
		~MemDump() { free(_memDump); }
		bool SaveDump(std::wstring& filePath);
		template<typename T> T GetBaseAddress() { return (T)_baseAddress; }
		template<typename T> T GetStartingAddress() { return (T)_startingAddress; }
		template<typename T> T GetDump() { return (T)_memDump; }
	};
}

