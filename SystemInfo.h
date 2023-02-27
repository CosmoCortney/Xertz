#pragma once
#include"definitions.h"
#include"ProcessInfo.h"
#include<vector>
#include<tlhelp32.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <windows.h>

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
		bool RefreshProcessInfoList();
		static SystemInfo& GetInstance()
		{
			static SystemInfo Instance;
			return Instance;
		}
		std::wstring WString2Lower(std::wstring& str);
		bool PGetMasterVolume();

		float _masterVolumeScalar = 0.0f;
		float _masterVolumeDecibel = 0.0f;
		IMMDeviceEnumerator* _deviceEnumerator = NULL;
		IMMDevice* _defaultDevice = NULL;
		IAudioEndpointVolume* _endpointVolume = NULL;
		const float _percentageFactor = 100.0f;

	public:
		static PROCESS_INFO GetProcessInfo(int pid);
		static PROCESS_INFO GetProcessInfo(std::wstring processName, int substring, int caseSensitive);
		static PROCESS_INFO_LIST& GetProcessInfoList();
		static bool KillProcess(int pid);
		static bool KillProcess(std::wstring processName, int substring, int caseSensitive);
		static bool GetMasterVolume(float* out, int scalarOrDecibel);
		static bool SetMasterVolume(float in, int scalarOrDecibel);
		static bool SetMasterMute(int option);
	};
}