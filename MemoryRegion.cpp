#include"MemoryRegion.h"

Xertz::MemoryRegion::MemoryRegion(const uint64_t baseAddress, const uint64_t allocationBase, const uint64_t allocationProtect, const uint64_t regionSize, const int64_t state, const int64_t  protect, const int64_t type, const int32_t partitionId)
{
	_baseAddress = baseAddress;
	_allocationBase = allocationBase;
	_allocationProtect = allocationProtect;
	_regionSize = regionSize;
	_state = state;
	_protect = protect;
	_type = type;
	_partitionId = partitionId;
}

bool Xertz::MemoryRegion::SetAllocationProtect(const DWORD protect) const
{
	DWORD oldPRotect;
	return VirtualProtect(reinterpret_cast<void*>(_baseAddress), _regionSize, protect, &oldPRotect);
}

bool Xertz::MemoryRegion::SetProtect(const HANDLE handle, const DWORD protect) const
{
	DWORD oldPRotect;
	return VirtualProtectEx(handle, reinterpret_cast<void*>(_baseAddress), _regionSize, protect, &oldPRotect);
}