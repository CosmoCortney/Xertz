#pragma once
#include"definitions.h"
#include<string>
#include<Windows.h>

namespace Xertz
{
	class MemDump
	{
	private:
		void* _address = nullptr;
		uint64_t _memSize = 0;
		void* _memDump = nullptr;
		std::wstring _filePath;
		HANDLE _handle;
		bool _hasHandle = false;

		void DumpExRAM();

	public:
		MemDump(HANDLE handle, void* address, uint64_t memSize);
		MemDump(std::wstring& path, void* address, uint64_t size = 0, uint64_t startReading = 0);
		MemDump(std::string& path, void* address, uint64_t size = 0, uint64_t startReading = 0);
		MemDump() {};
		~MemDump() { free(_memDump); }
		bool SaveDump(std::wstring& filePath);
		bool SaveDump();
		template<typename T> T GetAddress() { return (T)_address; }
		template<typename T> T GetDump() { return (T)_memDump; }
		uint64_t GetSize() { return _memSize; }

		void operator=(const MemDump& other)
		{
			_address = other._address;
			_memSize = other._memSize;
			_filePath = other._filePath;
			_handle = other._handle;
			_hasHandle = other._hasHandle;
			free(_memDump);
			_memDump = malloc(_memSize);
			if (_memDump)
				memcpy(_memDump, other._memDump, _memSize);
		}
	};
}