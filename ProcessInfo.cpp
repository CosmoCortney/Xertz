#include"ProcessInfo.h"
#include<tlhelp32.h>
#include <locale>
#include <codecvt>
#include<Psapi.h>

Xertz::ProcessInfo::ProcessInfo(int pid, std::wstring processName)
{
    _pid = pid;
    _processName = processName;
    
    if (_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))
    {
        IsWow64Process(_handle, (int*)&_isX64);
        _isX64 = !_isX64;

        bool RefreshRegionList();
        bool RefreshModuleList();

        //Get program file path
        WCHAR path[MAX_PATH];
        GetModuleFileNameEx(_handle, NULL, path, MAX_PATH);
        _filepath = path;
    }
}

bool Xertz::ProcessInfo::RefreshModuleList()
{
    if (_pid != -1)
    {
        _modules.clear();
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _pid);
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(moduleEntry);

        if (Module32FirstW(snap, &moduleEntry))
        {
            std::wstring moduleName;
            uint64_t moduleAddress = 0;
            do
            {
                moduleName = moduleEntry.szModule;
                moduleAddress = (uint64_t)moduleEntry.modBaseAddr;
                _modules.push_back(MODULE(moduleName, moduleAddress));
            } while (Module32NextW(snap, &moduleEntry));
            return true;
        }
    }

    return false;
}

MODULE_LIST& Xertz::ProcessInfo::GetModuleList()
{
    RefreshModuleList();
    return _modules;
}

uint64_t Xertz::ProcessInfo::GetModuleAddress(std::wstring moduleName)
{
    for (int i = 0; i < _modules.size(); ++i)
    {
        if (_modules[i].first.compare(moduleName) == 0)
        {
            return _modules[i].second;
        }
    }

    return -1;
}

bool Xertz::ProcessInfo::RefreshRegionList()
{
    if (_pid != -1)
    {
        _memoryRegions.clear();
        _handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _pid);
        MEMORY_BASIC_INFORMATION buffer = { 0 };
        uint64_t addressSpace;

        if (_isX64) { addressSpace = 0x7FFFFF00000; }
        else { addressSpace = 0xFF000000; }

        unsigned long long rangeIterator = 0;
        for (int i = 0; (buffer.RegionSize + reinterpret_cast<uint64_t>(buffer.BaseAddress)) < addressSpace; ++i)
        {
            VirtualQueryEx(_handle, reinterpret_cast<LPVOID>(rangeIterator), &buffer, sizeof(buffer));
            MemoryRegion temp_record(reinterpret_cast<uint64_t>(buffer.BaseAddress),
                                    reinterpret_cast<uint64_t>(buffer.AllocationBase),
                                    buffer.AllocationProtect,
                                    buffer.RegionSize,
                                    buffer.State,
                                    buffer.Protect,
                                    buffer.Type,
                                    buffer.PartitionId);

            _memoryRegions.push_back(temp_record);
            rangeIterator += buffer.RegionSize;
        }
        return true;
    }
    return false;
}

int Xertz::ProcessInfo::GetPID()
{
    return _pid;
}

std::wstring& Xertz::ProcessInfo::GetProcessName()
{
    return _processName;
}

REGION_LIST& Xertz::ProcessInfo::GetRegionList()
{
    RefreshRegionList();
    return _memoryRegions;
}

HANDLE Xertz::ProcessInfo::InitHandle(int64_t accessMode, bool inheritHandle)
{
    return OpenProcess(accessMode, inheritHandle, _pid);
}

HANDLE Xertz::ProcessInfo::GetHandle()
{
    return _handle;
}

std::wstring& Xertz::ProcessInfo::GetFilePath()
{
    return _filepath;
}

bool Xertz::ProcessInfo::IsX64()
{
    return _isX64;
}

bool Xertz::ProcessInfo::IsRunning()
{
    DWORD lpExitCode;
    _isRunning = (GetExitCodeProcess(_handle, &lpExitCode) == STILL_ACTIVE) ? true : false;
    return _isRunning;
}

Xertz::MemDump Xertz::ProcessInfo::DumpMemory(uint64_t baseAddress, uint64_t startingAddress, uint64_t size)
{

    return Xertz::MemDump(_handle, baseAddress, startingAddress, size);
}