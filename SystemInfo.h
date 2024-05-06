#pragma once
#include"definitions.h"
#include"ProcessInfo.h" //also includes Windows.h and MemoryRegions.h
#include<vector>
#include<tlhelp32.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

namespace Xertz
{
	class SystemInfo
	{
	private:
		SystemInfo();
		~SystemInfo();
		SystemInfo(SystemInfo const&) = delete;
		void operator=(SystemInfo const&) = delete;
		PROCESS_INFO_LIST s_processInfoList;
		PROCESS_INFO_LIST s_applicationProcessInfoList;
		std::vector<HWND> _windows{};
		static SystemInfo& GetInstance()
		{
			static SystemInfo Instance;
			return Instance;
		}
		std::wstring WString2Lower(const std::wstring& str) const;
		bool PGetMasterVolume();

		float _masterVolumeScalar = 0.0f;
		float _masterVolumeDecibel = 0.0f;
		IMMDeviceEnumerator* _deviceEnumerator = NULL;
		IMMDevice* _defaultDevice = NULL;
		IAudioEndpointVolume* _endpointVolume = NULL;
		const float _percentageFactor = 100.0f;
		HMODULE _ntdll = 0;

		void obtainHWNDs();
		static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
		{
			std::vector<HWND>* hwndList = reinterpret_cast<std::vector<HWND>*>(lParam);
			hwndList->push_back(hwnd);
			return TRUE;
		}

	public:
		static PROCESS_INFO GetProcessInfo(const int pid);
		static PROCESS_INFO GetProcessInfo(std::wstring processName, const bool substring, const bool caseSensitive);
		static PROCESS_INFO_LIST& GetProcessInfoList();
		static PROCESS_INFO_LIST& GetApplicationProcessInfoList();
		static bool KillProcess(const int pid);
		static bool KillProcess(const std::wstring& processName, const bool substring, const bool caseSensitive);
		static bool GetMasterVolume(float* out, const bool scalar = true);
		static bool SetMasterVolume(const float in, const bool scalar = true);
		static bool SetMasterMute(const bool mute = true);
		static bool RefreshProcessInfoList();
		static bool RefreshApplicationProcessInfoList();
		static WINDOW_HANDLE_LIST& GetWindowHandleList();
		static HMODULE GetNtDllHandle();
        
		template<typename T> static HWND GetWindowHandle(const T windowTitle)
		{
			 if constexpr (std::is_same_v<T, std::wstring>)
			 	return FindWindowW(NULL, windowTitle.c_str());
    		
			return FindWindowA(NULL, windowTitle.c_str());
		}
	};
}