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

	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);
	
	if (file.is_open())
	{
		if (size == 0 && pos == 0)
		{
			file.seekg(0, std::ios::end);
			_memSize = file.tellg();
			file.seekg(0, std::ios::beg);
			_memDump = malloc(_memSize);

			if (!file.read((char*)_memDump, _memSize))
			{
				_baseAddress = _startingAddress = 0;
				_memDump = nullptr;
			}
		}
		else
		{
			file.seekg(0, std::ios::end);
			uint64_t end = file.tellg();
			uint64_t readSize = end - pos;
			if (readSize < size || size == 0)
			{
				size = readSize;
			}

			file.seekg(0, pos);
			_memDump = malloc(size);

			if (!file.read((char*)_memDump, size))
			{
				_baseAddress = _startingAddress = 0;
				_memDump = nullptr;
			}
		}
	}
	file.close();
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
	std::ofstream file(filePath, std::ios::out | std::ios::binary);
	if (file)
	{
		file.write((char*)_memDump, _memSize);
		file.close();
		return true;
	}
	return false;
}