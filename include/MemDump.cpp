#include "MemDump.h"
#include<fstream>

Xertz::MemDump::MemDump(HANDLE handle, uint64_t baseAddress, uint64_t startingAddress, uint64_t memSize)
{
	_handle = handle;
	_hasHandle = true;
	_baseAddress = baseAddress;
	_startingAddress = startingAddress;
	_memSize = memSize;
	DumpExRAM();
}

Xertz::MemDump::MemDump(std::wstring& path, uint64_t baseAddress, uint64_t startingAddress, uint64_t size, uint64_t pos)
{
	_hasHandle = false;
	_baseAddress = baseAddress;
	_startingAddress = startingAddress;

	_memDump = malloc(_memSize);
	if (!Xertz::LoadBinary(path, _memDump, 0, 0))
	{
		free(_memDump);
		_baseAddress = _startingAddress = 0;
		_memDump = nullptr;
	}
}

void Xertz::MemDump::DumpExRAM()
{
	_memDump = malloc(_memSize);
	if (!ReadProcessMemory(_handle, (LPCVOID)_baseAddress, _memDump, _memSize, NULL))
	{
		free(_memDump);
		_memDump = nullptr;
	}
}

bool Xertz::MemDump::SaveDump(std::wstring& filePath)
{
	return Xertz::SaveBinary(filePath, _memDump, _memSize);
}