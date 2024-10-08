#include "ProcessInfo.hpp"
#include <tlhelp32.h>
#include <locale>
#include <codecvt>
#include <Psapi.h>
#include <filesystem>
#include <fstream>
#include "SystemInfo.hpp"

Xertz::ProcessInfo::ProcessInfo(const int pid, const std::wstring& processNameW)
{
    _pid = pid;
    _processNameW = processNameW;
    _processName = std::string(processNameW.begin(), processNameW.end());
    
    if (_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid))
    {
        _isOpen = _handle != 0;
        IsWow64Process(_handle, reinterpret_cast<int*>(&_isX64));
        _isX64 = !_isX64;

        RefreshRegionList();
        RefreshModuleList();

        //Get program file path
        WCHAR path[MAX_PATH];
        GetModuleFileNameExW(_handle, nullptr, path, MAX_PATH);
        _filepath = path;
    }
}

bool Xertz::ProcessInfo::RefreshModuleList()
{
    if (_pid != -1)
    {
        _modules.clear();
        const HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _pid);
        MODULEENTRY32W moduleEntry;
        moduleEntry.dwSize = sizeof(moduleEntry);

        if (Module32FirstW(snap, &moduleEntry))
        {
            uint64_t moduleAddress = 0;
            do
            {
                std::wstring moduleName = moduleEntry.szModule;
                moduleAddress = reinterpret_cast<uint64_t>(moduleEntry.modBaseAddr);
                _modules.emplace_back(moduleName, moduleAddress);
            } while (Module32NextW(snap, &moduleEntry));
            return true;
        }
    }

    return false;
}

MODULE_LIST& Xertz::ProcessInfo::GetModuleList()
{
    return _modules;
}

uint64_t Xertz::ProcessInfo::GetModuleAddress(const std::wstring& moduleName) const
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
        _isOpen = _handle != 0;

        if (!_isOpen)
            return false;

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

            //check if a process cadaver sneaked into here somehow
            if (buffer.RegionSize == 0)
                if(buffer.BaseAddress == 0)
                    return false;

            _memoryRegions.push_back(temp_record);
            rangeIterator += buffer.RegionSize;
        }
        return true;
    }
    return false;
}

bool Xertz::ProcessInfo::IsOpen() const
{
    return _isOpen;
}

int Xertz::ProcessInfo::GetPID() const
{
    return _pid;
}

std::string& Xertz::ProcessInfo::GetProcessName()
{
    return _processName;
}

std::wstring& Xertz::ProcessInfo::GetProcessNameW()
{
    return _processNameW;
}

REGION_LIST& Xertz::ProcessInfo::GetRegionList()
{
    return _memoryRegions;
}

HANDLE Xertz::ProcessInfo::InitHandle(const int64_t accessMode, const bool inheritHandle) const
{
    return OpenProcess(accessMode, inheritHandle, _pid);
}

HANDLE Xertz::ProcessInfo::GetHandle() const
{
    return _handle;
}

std::wstring& Xertz::ProcessInfo::GetFilePath()
{
    return _filepath;
}

bool Xertz::ProcessInfo::IsX64() const
{
    return _isX64;
}

bool Xertz::ProcessInfo::IsRunning()
{
    DWORD lpExitCode;
    GetExitCodeProcess(_handle, &lpExitCode);
    _isRunning = lpExitCode == STILL_ACTIVE;
    return _isRunning;
}

bool Xertz::ProcessInfo::DumpMemory(void* address, std::wstring& path, const uint64_t size) const
{
    std::wstring directoryPath = path.substr(0, path.find_last_of(L"\\"));
    
    if (!std::filesystem::is_directory(directoryPath))
        if (!std::filesystem::create_directory(directoryPath))
            return false;

    char* dump = new char[size];
    ReadMemorySafe(dump, address, size);

    std::ofstream file(path, std::ios::binary | std::ios::out);
    if (!file)
        return false;

    file.write(dump, size);
    file.close();
    delete[] dump;
    return true;
}

bool Xertz::ProcessInfo::DumpMemory(MemoryRegion& region, std::wstring& path) const
{
    return DumpMemory(region.GetBaseAddress<void*>(), path, region.GetRegionSize());
}

void Xertz::ProcessInfo::ReadMemoryFast(void* out, void* address, const unsigned long long size, const uint64_t forceSteps) const
{
    static std::string functionName = "NtReadVirtualMemory";
    /*NTSTATUS status;*/

    if (forceSteps)
    {
        int64_t remaining = size;
        char* outInc = reinterpret_cast<char*>(out);
        char* addressInc = (char*)address;

        for (uint64_t offset = 0; offset < size; offset += forceSteps)
        {
            pNtReadVirtualMemory NtReadVirtualMemory = (pNtReadVirtualMemory)GetProcAddress(SystemInfo::GetNtDllHandle(), functionName.c_str());
            /*status = */NtReadVirtualMemory(_handle, addressInc, outInc, remaining < forceSteps ? remaining : forceSteps, nullptr);
            outInc += forceSteps;
            addressInc += forceSteps;
            remaining -= forceSteps;
        }
    }
    else
    {
        pNtReadVirtualMemory NtReadVirtualMemory = (pNtReadVirtualMemory)GetProcAddress(SystemInfo::GetNtDllHandle(), functionName.c_str());
        /*status = */NtReadVirtualMemory(_handle, address, out, size, nullptr);
    }
}

void Xertz::ProcessInfo::ReadMemorySafe(void* out, const void* address, const unsigned long long size, const uint64_t forceSteps) const
{
    if (forceSteps)
    {
        int64_t remaining = size;
        char* outInc = reinterpret_cast<char*>(out);
        char* addressInc = (char*)address;

        for (uint64_t offset = 0; offset < size; offset += forceSteps)
        {
            ReadProcessMemory(_handle, addressInc, outInc, remaining < forceSteps ? remaining : forceSteps, nullptr);
            outInc += forceSteps;
            addressInc += forceSteps;
            remaining -= forceSteps;
        }
    }
    else
        ReadProcessMemory(_handle, address, out, size, nullptr);
}

void Xertz::ProcessInfo::WriteMemoryFast(void* in, void* address, const unsigned long long size, const uint64_t forceSteps) const
{
    static std::string functionName = "NtWriteVirtualMemory";
    /*NTSTATUS status;*/
    if (forceSteps)
    {
        int64_t remaining = size;
        char* inInc = (char*)in;
        char* addressInc = reinterpret_cast<char*>(address);

        for (uint64_t offset = 0; offset < size; offset += forceSteps)
        {
            pNtWriteVirtualMemory NtWriteVirtualMemory = (pNtWriteVirtualMemory)GetProcAddress(SystemInfo::GetNtDllHandle(), functionName.c_str());
            /*status = */NtWriteVirtualMemory(_handle, addressInc, inInc, remaining < forceSteps ? remaining : forceSteps, nullptr);
            inInc += forceSteps;
            addressInc += forceSteps;
            remaining -= forceSteps;
        }
    }
    else
    {
        pNtWriteVirtualMemory NtWriteVirtualMemory = (pNtWriteVirtualMemory)GetProcAddress(SystemInfo::GetNtDllHandle(), functionName.c_str());
        /*status = */NtWriteVirtualMemory(_handle, address, in, size, nullptr);
    }
}

void Xertz::ProcessInfo::WriteMemorySafe(const void* in, void* address, const unsigned long long size, const uint64_t forceSteps) const
{
    if (forceSteps)
    {
        int64_t remaining = size;
        char* inInc = (char*)in;
        char* addressInc = reinterpret_cast<char*>(address);

        for (uint64_t offset = 0; offset < size; offset += forceSteps)
        {
            WriteProcessMemory(_handle, addressInc, inInc, remaining < forceSteps ? remaining : forceSteps, nullptr);
            inInc += forceSteps;
            addressInc += forceSteps;
            remaining -= forceSteps;
        }
    }
    else
        WriteProcessMemory(_handle, address, in, size, nullptr);
}