#pragma once
#include<string>
#include<Windows.h>
#include<vector>
#include<utility>
#include"MemoryRegion.h"
#include"definitions.h"
#include"MemDump.h"

namespace Xertz
{
    class ProcessInfo
	{
	private:
        bool _isX64 = false;
        bool _isRunning = false;
		int _pid = -1;
        HANDLE _handle = 0;
        std::wstring _processName;
        REGION_LIST _memoryRegions;
        MODULE_LIST _modules;
        std::wstring _filepath;
        bool RefreshRegionList();
        bool RefreshModuleList();

    public:
        ProcessInfo() {}
        ProcessInfo(const ProcessInfo& other)
        {
            *this = other;
        }

        ProcessInfo(const int pid, const std::wstring& processName);
        //~ProcessInfo();

        int GetPID() const;
        std::wstring& GetProcessName();
        uint64_t GetModuleAddress(const std::wstring& moduleName) const;
        MODULE_LIST& GetModuleList();
        REGION_LIST& GetRegionList();
        HANDLE InitHandle(const int64_t accessMode, const bool inheritHandle) const;
        HANDLE GetHandle() const;
        bool IsX64() const;
        bool IsRunning();
        std::wstring& GetFilePath();
        void ReadExRAM(void* out, const void* address, const uint64_t size) const;
        void WriteExRAM(const void* in, void* address, const uint64_t size) const;
        MemDump DumpMemory(void* address, const uint64_t size) const;
        template<typename T> bool FillProcessMemory(const uint64_t start, const uint64_t writeSize, const T val, const uint64_t valSize); //ToDo
        template<typename T> T AllocateRegion(const uint64_t size, const uint64_t allocationType, const uint64_t protection, const uint64_t address = NULL) //ToDo: make this typesafe
        {
            LPVOID ptr = VirtualAllocEx(_handle, reinterpret_cast<void*>(address), size, MEM_COMMIT, protection);
            RefreshRegionList();
            return (T)ptr;
        }

        void operator=(const ProcessInfo& other)
        {
            _isX64 = other._isX64;
            _isRunning = other._isRunning;
            _pid = other._pid;
            _handle = other._handle;
            _processName = other._processName;
            _memoryRegions = other._memoryRegions;
            _modules = other._modules;
            _filepath = other._filepath;

            if (!_handle)
                _handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _pid);
        }
	};

}
