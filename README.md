# Xertz
A wrapper around the Windows API dedicated to manipulating games and other processes

## Definitions and datatypes
See Classes for more information<br />
MODULE = std::pair<std::wstring, unsigned long long><br />
MODULE_LIST = std::vector<std::pair<std::wstring, unsigned long long>><br />
REGION_LIST = std::vector<Xertz::MemoryRegion><br />
PROCESS_INFO = Xertz::ProcessInfo<br />
PROCESS_INFO_LIST = std::vector<Xertz::ProcessInfo><br />
NO_SUBSTRING = 0<br />
IS_SUBSTRING = 1<br />
CASE_SENSITIVE = 2<br />
CASE_INSENSITIVE = 3<br />
SCALAR = 0,<br />
DECIBEL = 1,<br />
MUTE = 2,<br />
UNMUTE = 3<br />


## Classes
### Xertz::SystemInfo
A singleton class that provides information and functions related to you system.<br />

#### PROCESS_INFO  Xertz::SystemInfo::GetProcessInfo(int pid)
Returns a PROCESS_INFO (Xertz::ProcessInfo) that possesses the process ID specified by pid.<br />

#### PROCESS_INFO  Xertz::SystemInfo::GetProcessInfo(std::wstring processName, int substring, int caseSensitive)
Returns a PROCESS_INFO (Xertz::ProcessInfo) which name matches processName considering the criteria of substring and caseSensitive.<br />

#### PROCESS_INFO_LIST& Xertz::SystemInfo::GetProcessInfoList()
Returns a PROCESS_INFO_LIST& (std::vector<Xertz::ProcessInfo>&) containing information about all processes.<br />

#### bool Xertz::SystemInfo::KillProcess(int pid)
Kills the process specified by its process ID (pid). If successful return value is true.<br />

#### bool Xertz::SystemInfo::KillProcess(std::wstring processName, int substring, int caseSensitive)
Kills the process specified by its name (processName) considering the criteria of substring and caseSensitive. If successful return value is true.<br />

#### bool Xertz::SystemInfo::GetMasterVolume(float* out, int scalarOrDecibel)
Writes the master volume to *out. scalarOrDecibel defines the interpretation of the audio level. If successful return value is true.<br />

#### bool Xertz::SystemInfo::SetMasterVolume(float in, int scalarOrDecibel)
Sets the master volume with the value given in in. scalarOrDecibel defines the interpretation of the audio level. If successful return value is true.<br />

#### bool Xertz::SystemInfo::SetMasterMute(int option)
Mutes or unmutes the Master Volume according the option. If successful return value is true.<br />


### Xertz::ProcessInfo
A class that delivers useful information of any running process and features various functions<br />

#### int Xertz::ProcessInfo::GetPID()
Returns the process ID<br />

#### std::wstring& Xertz::ProcessInfo::GetProcessName()
Returns the process name.<br />

#### uint64_t Xertz::ProcessInfo::GetModuleAddress(std::wstring moduleName)
Returns the module address of moduleName. If an error occurred return value is -1.<br />

#### MODULE_LIST& Xertz::ProcessInfo::GetModuleList()
Returns a MODULE_LIST& (std::vector<std::pair<std::wstring, unsigned long long>>&) containing addresses and names of all modules.<br />

#### REGION_LIST& Xertz::ProcessInfo::GetRegionList()
Returns a REGION_LIST& (std::vector<Xertz::MemoryRegion>&) containing all memory regions.<br />

#### HANDLE Xertz::ProcessInfo::GetHandle(int64_t accessMode, bool inheritHandle)
Returns the HANDLE.<br />

#### bool Xertz::ProcessInfo::IsX64()
Returns true if 64-bit or false if 32-bit.<br />

#### bool Xertz::ProcessInfo::IsRunning()
Returns true if currently running. False if suspended.<br />

#### std::wstring& Xertz::ProcessInfo::GetFilePath()
Returns the exe filepath.<br />

#### Xertz::MemDump Xertz::ProcessInfo::DumpMemory(uint64_t baseAddress, uint64_t startingAddress, uint64_t size)
Dumps the memory to an object of Xertz::MemDump region of size from baseAddress starting at startingAddress.<br />

#### bool Xertz::ProcessInfo::FillProcessMemory<T>(uint64_t start, uint64_t writeCount, T val, uint64_t valSize)
Fills the value of val with the specified size of size writeCount times starting at start. If successful return value is true.<br />

#### T Xertz::ProcessInfo::AllocateRegion<T>(uint64_t size, uint64_t allocationType, uint64_t protection, uint64_t address = NULL)
Allocates a new memory region of size size with the allocation type of allocationType, the protection type of protection. Address can be specified (optional). Return value is the address of the new memory range. <br />


### Xertz::MemoryRegion
A class that provides all useful information about a memory region.<br />

#### T Xertz::MemoryRegion::GetBaseAddress<T>()
Returns the Base Address of the Memory Region of type T<br />

#### T Xertz::MemoryRegion::GetAllocationBase<T>()
Returns the Allocation Base Address of the memory region of type T<br />

#### uint64_t Xertz::MemoryRegion::GetAllocationProtect()
Returns the Allocation Protection type<br />

#### uint64_t Xertz::MemoryRegion::GetRegionSize()
Returns the Region size<br />

#### int64_t Xertz::MemoryRegion::GetState()
Returns the Mapping state<br />

#### int64_t Xertz::MemoryRegion::GetProtect()
Returns the access protection code<br />

#### int64_t Xertz::MemoryRegion::GetType()
Returns the Region Type<br />

#### int64_t Xertz::MemoryRegion::GetPartitionId()
Returns the Partition ID<br />


### Xertz::MemDump
A classes that delivers useful information of any running process and features various functions<br />

#### MemDump(HANDLE handle, uint64_t baseAddress, uint64_t startingAddress, uint64_t memSize)
Creates an instance of the MemDump type of the handles' Process. baseAddress is the base address of where the desired memory area is located. startingAddress is the location of the desired memory area. memSize is the size of the memory area you want to dump.<br />

#### MemDump(std::wstring& path, uint64_t baseAddress, uint64_t startingAddress, uint64_t size (optional), uint64_t pos (optional))
Creates an instance of the MemDump type by the file expressed by path. baseAddress is the base address of the dumped memory. startingAddress is the beging of the memory area. size (optional) defines how many bytes to read from the file. pos (optional) defines where to start reading from the file (if not 0 startingAddress must be considered respectively)<br />

#### ~MemDump()
Frees the memory dump<br />

#### bool SaveDump(std::wstring& filePath)
Saves dump to file. If successful return value is true.<br />

#### T GetBaseAddress()
Returns the baseaddress as type T.<br />

#### T GetStartingAddress()
Returns the starting address as type T.<br />

#### T GetDump()
Returns the dump location's address as type T<br />
