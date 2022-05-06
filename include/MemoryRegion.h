#pragma once
#include"definitions.h"
#include <type_traits>
#include<string>
#include <typeinfo>

namespace Xertz
{
	class MemoryRegion
	{
	private:
		uint64_t _baseAddress;
		uint64_t _allocationBase;
		uint64_t _allocationProtect;
		uint64_t _regionSize;
		int64_t _state;
		int64_t _protect;
		int64_t _type;
		int32_t _partitionId;

	public:
		MemoryRegion(uint64_t baseAddress, uint64_t allocationBase, uint64_t allocationProtect, uint64_t regionSize, int64_t  state, int64_t  protect, int64_t  type, int32_t partitionId);
		
		template<typename T> T GetBaseAddress(){ return (T)_baseAddress; } //ToDo: make this typesafe to all ptrs and integers
		template<typename T> T GetAllocationBase() { return (T)_allocationBase; } //ToDo: make this typesafe to all ptrs and integers
		uint64_t GetAllocationProtect() { return _allocationProtect; }
		uint64_t GetRegionSize() { return _regionSize; }
		int64_t GetState() { return _state; }
		int64_t GetProtect() { return _protect; }
		int64_t GetType() { return _type; }
		int64_t GetPartitionId() { return _partitionId; }

		bool Deallocate();
		bool Extend(uint64_t size);
		bool ChangeAllocationMode(int val);
		bool DumpRegion(std::wstring filePath);
	};
}