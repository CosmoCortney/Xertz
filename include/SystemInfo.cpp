#include"SystemInfo.h"
#include<algorithm>
#include<iostream>
Xertz::SystemInfo::SystemInfo()
{
	CoInitialize(nullptr);
	CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&_deviceEnumerator);

	if (_deviceEnumerator != nullptr)
	{
		_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_defaultDevice);
		_deviceEnumerator->Release();
		_deviceEnumerator = NULL;
		_defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&_endpointVolume);
		_defaultDevice->Release();
		_defaultDevice = NULL;
	}
}

Xertz::SystemInfo::~SystemInfo()
{
	if (_endpointVolume != nullptr)
	{
		_endpointVolume->Release();
		CoUninitialize();
	}
}

bool Xertz::SystemInfo::RefreshProcessInfoList()
{
	s_processInfoList.clear();
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);

	if (Process32FirstW(snap, &entry))
	{
		do
		{
			s_processInfoList.push_back(PROCESS_INFO(entry.th32ProcessID, std::wstring(entry.szExeFile)));
		} while (Process32NextW(snap, &entry));

		return true;
	}

	return false;
}

PROCESS_INFO_LIST& Xertz::SystemInfo::GetProcessInfoList()
{
	GetInstance().RefreshProcessInfoList();
	return GetInstance().s_processInfoList;
}

PROCESS_INFO Xertz::SystemInfo::GetProcessInfo(int pid)
{
	if (GetInstance().RefreshProcessInfoList())
	{
		for (int i = 0; i < GetInstance().s_processInfoList.size(); ++i)
		{
			if (GetInstance().s_processInfoList[i].GetPID() == pid)
			{
				return GetInstance().s_processInfoList[i];
			}
		}
	}

	return ProcessInfo(-1, L"Process not found");
}

PROCESS_INFO Xertz::SystemInfo::GetProcessInfo(std::wstring processName, int substring, int caseSensitive)
{
	if (GetInstance().RefreshProcessInfoList())
	{
		for (int i = 0; i < GetInstance().s_processInfoList.size(); ++i)
		{
			if (caseSensitive == Xertz::CASE_SENSITIVE)
			{
				if ((substring == Xertz::IS_SUBSTRING && GetInstance().s_processInfoList[i].GetProcessName().find(processName) != (uint64_t)-1) ||  /*case sensitive and contains*/
					(!substring == Xertz::IS_SUBSTRING && GetInstance().s_processInfoList[i].GetProcessName().compare(processName) == 0)) /*case sensitive and same*/
				{
					return GetInstance().s_processInfoList[i];
				}
			}
			else
			{
				std::wstring lProcessName = GetInstance().s_processInfoList[i].GetProcessName();
				lProcessName = GetInstance().WString2Lower(lProcessName);
				processName = GetInstance().WString2Lower(processName);

				if ((substring == Xertz::IS_SUBSTRING && lProcessName.find(processName) != (uint64_t)-1) ||  /*case insensitive and contains*/
					(!substring == Xertz::IS_SUBSTRING && lProcessName.compare(processName) == 0)) /*case insensitive and same*/					
				{
					return GetInstance().s_processInfoList[i];
				}
			}
		}
	}

	return ProcessInfo(-1, L"Process not found");
}

bool Xertz::SystemInfo::KillProcess(int pid)
{
	HANDLE h = OpenProcess(PROCESS_TERMINATE, false, pid);
	TerminateProcess(h, 1);
	return CloseHandle(h);
}

bool Xertz::SystemInfo::KillProcess(std::wstring processName, int substring, int caseSensitive)
{
	int pid = GetInstance().GetProcessInfo(processName, substring, caseSensitive).GetPID();
	return GetInstance().KillProcess(pid);
}

std::wstring Xertz::SystemInfo::WString2Lower(std::wstring& str)
{
	std::wstring result = str;
	std::transform(result.begin(), result.end(), result.begin(), tolower);
	return result;
}

bool Xertz::SystemInfo::PGetMasterVolume()
{
	bool success;
	success = (bool)_endpointVolume->GetMasterVolumeLevel(&_masterVolumeDecibel);
	_masterVolumeScalar = _masterVolumeDecibel;
	success = (bool)_endpointVolume->GetMasterVolumeLevelScalar(&_masterVolumeScalar);
	return !success;
}

bool Xertz::SystemInfo::GetMasterVolume(float* out, int scalarOrDecibel)
{
	bool success;
	success = (bool)GetInstance().PGetMasterVolume();
	scalarOrDecibel == Xertz::SCALAR ? *out = GetInstance()._masterVolumeScalar : *out = GetInstance()._masterVolumeDecibel;
	return !success;
}

bool Xertz::SystemInfo::SetMasterVolume(float in, int scalarOrDecibel)
{
	bool success;
	if (scalarOrDecibel == Xertz::SCALAR)
	{
		success = (bool)GetInstance()._endpointVolume->SetMasterVolumeLevelScalar(in, NULL);
	} 
	else
	{
		success = (bool)GetInstance()._endpointVolume->SetMasterVolumeLevel(in, NULL);
	}
	GetInstance().PGetMasterVolume();
	return !success;
}

bool Xertz::SystemInfo::SetMasterMute(int option)
{
	bool success;
	if (option == Xertz::MUTE)
	{
		success = SetMasterVolume(0.0f, Xertz::SCALAR);
		GetInstance().PGetMasterVolume();
	}
	else
	{
		success = SetMasterVolume(GetInstance()._masterVolumeScalar, Xertz::SCALAR);
	}
	return !success;
}