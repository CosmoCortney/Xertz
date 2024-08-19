#pragma once
#include "definitions.hpp"
#include "Windows.h"
#include <type_traits>
#include <string>
#include <typeinfo>

namespace Xertz
{
	class MemoryRegion
	{
	private:
		uint64_t _baseAddress;
		uint64_t _allocationBase;
		uint32_t _allocationProtect;
		uint64_t _regionSize;
		int64_t _state;
		int64_t _protect;
		int64_t _type;
		int32_t _partitionId;

	public:
		MemoryRegion(const uint64_t baseAddress, const uint64_t allocationBase, const uint64_t allocationProtect, const uint64_t regionSize, const int64_t state, const int64_t protect, const int64_t type, const int32_t partitionId);
		
		template<typename T> T GetBaseAddress() const { return (T)_baseAddress; } //ToDo: make this typesafe to all ptrs and integers
		template<typename T> T GetAllocationBase() { return (T)_allocationBase; } //ToDo: make this typesafe to all ptrs and integers
		uint32_t GetAllocationProtect() const { return _allocationProtect; }
		uint64_t GetRegionSize() const { return _regionSize; }
		int64_t GetState() const { return _state; }
		int64_t GetProtect() const { return _protect; }
		int64_t GetType() const { return _type; }
		int32_t GetPartitionId() const { return _partitionId; }
		bool SetAllocationProtect(const DWORD protect) const;
		bool SetProtect(const HANDLE handle, const DWORD protect) const;
		//bool Deallocate(); //ToDo
		//bool Extend(const uint64_t size); //ToDo
		//bool ChangeAllocationMode(const int val); //ToDo
		//bool DumpRegion(const std::wstring filePath); //ToDo
	};
}