#include"SystemInfo.h"
#include<algorithm>
#include<iostream>
Xertz::SystemInfo::SystemInfo()
{
	CoInitialize(nullptr);
	CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), reinterpret_cast<LPVOID*>(&_deviceEnumerator));

	if (_deviceEnumerator != nullptr)
	{
		_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_defaultDevice);
		_deviceEnumerator->Release();
		_deviceEnumerator = NULL;
		_defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, reinterpret_cast<LPVOID*>(&_endpointVolume));
		_defaultDevice->Release();
		_defaultDevice = NULL;
	}

	_ntdll = GetModuleHandleW(L"ntdll.dll");
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
	GetInstance().s_processInfoList.clear();
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(entry);

	if (Process32FirstW(snap, &entry))
	{
		do
		{
			GetInstance().s_processInfoList.emplace_back(entry.th32ProcessID, std::wstring(entry.szExeFile));
		} while (Process32NextW(snap, &entry));

		return true;
	}

	return false;
}

void Xertz::SystemInfo::obtainHWNDs()
{
	_windows.clear();
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&_windows));
}

WINDOW_HANDLE_LIST& Xertz::SystemInfo::GetWindowHandleList()
{
	GetInstance().obtainHWNDs();
	return GetInstance()._windows;
}

HMODULE Xertz::SystemInfo::GetNtDllHandle()
{
	return GetInstance()._ntdll;
}

bool Xertz::SystemInfo::RefreshApplicationProcessInfoList()
{
	GetInstance().s_applicationProcessInfoList.clear();
	GetInstance().obtainHWNDs();
	DWORD previousPid = 0;
	std::vector<std::wstring> foundApps;

	for (HWND window : GetInstance()._windows)
	{
		DWORD anotherpid;
		DWORD pid = GetWindowThreadProcessId(window, &anotherpid);

		if (anotherpid < 1)
			continue;

		if (anotherpid == previousPid)
			continue;
		
		for (ProcessInfo& process : GetInstance().s_processInfoList)
		{
			bool found = false;
			if (process.GetPID() != anotherpid)
				continue;

			for (std::wstring& processName : foundApps)
				if (process.GetProcessNameW().compare(processName) == 0)
					found = true;
			
			if (found)
				continue;

			GetInstance().s_applicationProcessInfoList.push_back(process);
			foundApps.push_back(process.GetProcessNameW());
			previousPid = anotherpid;
			break;
		}
	}

	return GetInstance().s_applicationProcessInfoList.size();
}

PROCESS_INFO_LIST& Xertz::SystemInfo::GetProcessInfoList()
{
	return GetInstance().s_processInfoList;
}

PROCESS_INFO_LIST& Xertz::SystemInfo::GetApplicationProcessInfoList()
{
	return GetInstance().s_applicationProcessInfoList;
}

PROCESS_INFO Xertz::SystemInfo::GetProcessInfo(const int pid)
{
	for (int i = 0; i < GetInstance().s_processInfoList.size(); ++i)
	{
		if (GetInstance().s_processInfoList[i].GetPID() == pid)
		{
			return GetInstance().s_processInfoList[i];
		}
	}
	
	return ProcessInfo(-1, L"Process not found");
}

PROCESS_INFO Xertz::SystemInfo::GetProcessInfo(std::wstring processName, const bool substring, const bool caseSensitive)
{
	if (GetInstance().RefreshProcessInfoList())
	{
		for (int i = 0; i < GetInstance().s_processInfoList.size(); ++i)
		{
			if (caseSensitive)
			{
				if ((substring && GetInstance().s_processInfoList[i].GetProcessNameW().find(processName) != -1) ||  /*case sensitive and contains*/
					(!substring && GetInstance().s_processInfoList[i].GetProcessNameW().compare(processName) == 0)) /*case sensitive and same*/
				{
					return GetInstance().s_processInfoList[i];
				}
			}
			else
			{
				std::wstring lProcessName = GetInstance().s_processInfoList[i].GetProcessNameW();
				lProcessName = GetInstance().WString2Lower(lProcessName);
				processName = GetInstance().WString2Lower(processName);

				if ((substring && lProcessName.find(processName) != -1) ||  /*case insensitive and contains*/
					(!substring && lProcessName.compare(processName) == 0)) /*case insensitive and same*/					
				{
					return GetInstance().s_processInfoList[i];
				}
			}
		}
	}

	return ProcessInfo(-1, L"Process not found");
}

bool Xertz::SystemInfo::KillProcess(const int pid)
{
	HANDLE h = OpenProcess(PROCESS_TERMINATE, false, pid);
	TerminateProcess(h, 1);
	return CloseHandle(h);
}

bool Xertz::SystemInfo::KillProcess(const std::wstring& processName, const bool substring, const bool caseSensitive)
{
	const int pid = GetInstance().GetProcessInfo(processName, substring, caseSensitive).GetPID();
	return GetInstance().KillProcess(pid);
}

std::wstring Xertz::SystemInfo::WString2Lower(const std::wstring& str) const
{
	std::wstring result = str;
	std::transform(result.begin(), result.end(), result.begin(), tolower);
	return result;
}

bool Xertz::SystemInfo::PGetMasterVolume()
{
	bool success = _endpointVolume->GetMasterVolumeLevel(&_masterVolumeDecibel) != 0;
	_masterVolumeScalar = _masterVolumeDecibel;
	success = _endpointVolume->GetMasterVolumeLevelScalar(&_masterVolumeScalar) != 0;
	return !success;
}

bool Xertz::SystemInfo::GetMasterVolume(float* out, const bool scalar)
{
	const bool success = GetInstance().PGetMasterVolume();
	scalar ? *out = GetInstance()._masterVolumeScalar * GetInstance()._percentageFactor : *out = GetInstance()._masterVolumeDecibel;
	return success;
}

bool Xertz::SystemInfo::SetMasterVolume(const float in, const bool scalar)
{
	bool success;
	if (scalar)
	{
		success = GetInstance()._endpointVolume->SetMasterVolumeLevelScalar(in / GetInstance()._percentageFactor, NULL) != 0;
	} 
	else
	{
		success = GetInstance()._endpointVolume->SetMasterVolumeLevel(in, NULL) != 0;
	}
	GetInstance().PGetMasterVolume();
	return success;
}

bool Xertz::SystemInfo::SetMasterMute(const bool mute)
{
	bool success;
	if (mute)
	{
		GetInstance().PGetMasterVolume();
		const float volScal = GetInstance()._masterVolumeScalar, volDec = GetInstance()._masterVolumeDecibel;
		success = SetMasterVolume(0.0f, true);
		GetInstance()._masterVolumeScalar = volScal;
		GetInstance()._masterVolumeDecibel = volDec;
	}
	else
	{
		success = SetMasterVolume(GetInstance()._masterVolumeScalar * GetInstance()._percentageFactor, true);
	}
	return !success;
}