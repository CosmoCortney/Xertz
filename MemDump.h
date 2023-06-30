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
		HANDLE _handle = nullptr;
		bool _hasHandle = false;

		void DumpExRAM();

	public:
		MemDump(const HANDLE handle, void* address, const uint64_t memSize);
		MemDump(const std::wstring& path, void* address, const uint64_t size = 0, const uint64_t startReading = 0);
		//MemDump(const std::string& path, void* address, const uint64_t size = 0, const uint64_t startReading = 0);//ToDo
		MemDump() {}
		~MemDump()
		{
			free(_memDump);
			_memDump = nullptr;
		}
		bool SaveDump(const std::wstring& filePath) const;
		bool SaveDump() const;
		template<typename T> T GetAddress() const { return (T)_address; }
		template<typename T> T GetDump() const { return (T)_memDump; }
		uint64_t GetSize() const { return _memSize; }

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