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
        bool _isX64;
        bool _isRunning;
		int _pid = -1;
        HANDLE _handle;
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

        ProcessInfo(int pid, std::wstring processName);
        //~ProcessInfo();

        int GetPID();
        std::wstring& GetProcessName();
        uint64_t GetModuleAddress(std::wstring moduleName);
        MODULE_LIST& GetModuleList();
        REGION_LIST& GetRegionList();
        HANDLE InitHandle(int64_t accessMode, bool inheritHandle);
        HANDLE GetHandle();
        bool IsX64();
        bool IsRunning();
        std::wstring& GetFilePath();
        void ReadExRAM(void* out, void* address, unsigned long long size);
        void WriteExRAM(void* in, void* address, unsigned long long size);
        MemDump DumpMemory(void* address, uint64_t size);
        template<typename T> bool FillProcessMemory(uint64_t start, uint64_t writeSize, T val, uint64_t valSize);
        template<typename T> T AllocateRegion(uint64_t size, uint64_t allocationType, uint64_t protection, uint64_t address = NULL) //ToDo: make this typesafe
        {
            LPVOID ptr = VirtualAllocEx(_handle, (void*)address, size, MEM_COMMIT, protection);
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
