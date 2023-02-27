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
        ProcessInfo(int pid, std::wstring processName);
        //~ProcessInfo();

        int GetPID();
        std::wstring& GetProcessName();
        uint64_t GetModuleAddress(std::wstring moduleName);
        MODULE_LIST& GetModuleList();
        REGION_LIST& GetRegionList();
        HANDLE GetHandle(int64_t accessMode, bool inheritHandle);
        bool IsX64();
        bool IsRunning();
        std::wstring& GetFilePath();

        MemDump DumpMemory(uint64_t baseAddress, uint64_t startingAddress, uint64_t size);
        template<typename T> bool FillProcessMemory(uint64_t start, uint64_t writeSize, T val, uint64_t valSize);
        template<typename T> T AllocateRegion(uint64_t size, uint64_t allocationType, uint64_t protection, uint64_t address = NULL) //ToDo: make this typesafe
        {
            LPVOID ptr = VirtualAllocEx(_handle, (void*)address, size, MEM_COMMIT, protection);
            RefreshRegionList();
            return (T)ptr;
        }
	};

}
