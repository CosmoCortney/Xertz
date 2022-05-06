#include"include/Xertz.h"
#include<iostream>

int main()
{

	std::wcout << Xertz::SystemInfo::TEST(5);
	/*for (int i = 0; i < Xertz::SystemInfo::GetInstance().GetProcessInfoList().size(); ++i)
	{
		std::wcout << "process " << i << ": " << Xertz::SystemInfo::GetInstance().GetProcessInfoList()[i].GetProcessName() << std::endl;

	}*/

	//PROCESS_INFO_LIST list = Xertz::SystemInfo::GetProcessInfoList();
	//for (int i = 0; i < list.size(); ++i)
	//{
	//	std::wcout << list[i].GetProcessName() << ", x64: " << list[i].IsX64() << std::endl;
	//}

	/*

	PROCESS_INFO process = Xertz::SystemInfo::GetProcessInfo(L"snipping", Xertz::IS_SUBSTRING, Xertz::CASE_INSENSITIVE);
	REGION_LIST regions = process.GetRegionList();
	Xertz::MemDump dump = process.DumpMemory(regions[1].GetBaseAddress<uint64_t>(), regions[1].GetBaseAddress<uint64_t>(), regions[1].GetRegionSize());
	std::wstring file;
	std::wcin >> file;
	Xertz::MemDump dump2(file, 0x3d430000, 0x3d430000);
	std::cout << std::hex << *dump2.GetDump<uint32_t*>() << std::endl;

*/

	//std::cout << dump.SaveDump(file);
	//MODULE_LIST moduleList = process.GetModuleList();
	//REGION_LIST regionList = process.GetRegionList();

	//for (int i = 0; i < moduleList.size(); ++i)
	//{
	//	std::wcout << moduleList[i].first << L" at " << process.GetProcessName() << L"/0x" << std::hex << moduleList[i].second << std::endl;
	//}

	//for (int i = 0; i < regionList.size(); ++i)
	//{
	//	std::wcout << "Address: " << std::hex << regionList[i].GetBaseAddress<uint64_t>() << ", Size: 0x" << std::hex << regionList[i].GetRegionSize() << " - " << regionList[i].GetProtect() << std::endl;
	//}



}