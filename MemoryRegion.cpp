#include"MemoryRegion.h"

Xertz::MemoryRegion::MemoryRegion(uint64_t baseAddress, uint64_t allocationBase, uint64_t allocationProtect, uint64_t regionSize, int64_t state, int64_t  protect, int64_t type, int32_t partitionId)
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