#include "MemDump.h"
#include<fstream>

Xertz::MemDump::MemDump(HANDLE handle, void* address, uint64_t memSize)
{
	_handle = handle;
	_hasHandle = true;
	_address = address;
	_memSize = memSize;
	DumpExRAM();
}

Xertz::MemDump::MemDump(std::wstring& path, void* address, uint64_t size, uint64_t startReading)
{
	_hasHandle = false;
	_address = address;

	_memDump = malloc(_memSize);
	if (!Xertz::LoadBinary(path, _memDump, _memSize, startReading))
	{
		free(_memDump);
		_address = 0;
		_memDump = nullptr;
	}
}

void Xertz::MemDump::DumpExRAM()
{
	_memDump = malloc(_memSize);
	if (!ReadProcessMemory(_handle, _address, _memDump, _memSize, NULL))
	{
		free(_memDump);
		_memDump = nullptr;
	}
}

bool Xertz::MemDump::SaveDump(std::wstring& filePath)
{
	return Xertz::SaveBinary(filePath, _memDump, _memSize);
}

bool Xertz::MemDump::SaveDump()
{
	return Xertz::SaveBinary(_filePath, _memDump, _memSize);
}