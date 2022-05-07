#pragma once
#include"definitions.h"
#include"MemDump.h"
#include<functional>
#include<iostream>

namespace Xertz
{
	class MemCompareResult
	{
	private:
		void* _results = nullptr;
		std::wstring _filePath;
		void* _fileHeader = malloc(16);
		unsigned short _iteration = 0;
		unsigned short _addressWidth = 4;
		unsigned long long _resultCount = 0;
		bool _cached = false;
		unsigned int _magic = 0x2e6d6372; //.mcr (LE)

		bool SaveResults()
		{
			if (Xertz::SaveBinary(_filePath, _fileHeader, 16))
			{
				return Xertz::SaveBinary(_filePath, _results, _resultCount * _addressWidth, true);
			}
			return false;
		}

		bool LoadResults()
		{
			if (Xertz::LoadBinary(_filePath, _fileHeader, 16, 0))
			{
				if (*(unsigned int*)_fileHeader == _magic)
				{
					_iteration = *((unsigned short*)_fileHeader + 2);
					_addressWidth = *((unsigned short*)_fileHeader + 3);
					_resultCount = *((unsigned long long*)_fileHeader + 1);
					_results = malloc(_resultCount * _addressWidth);
					return Xertz::LoadBinary(_filePath, _results, _resultCount * _addressWidth, 16);
				}
			}
			return false;
		}

	public:
		MemCompareResult(bool cached, unsigned short addressWidth, unsigned long long resultCount, short iteration, void* results = nullptr, std::wstring dirPath = L"")
		{
			_cached = cached;
			_iteration = iteration;
			_resultCount = resultCount;
			_addressWidth = addressWidth;
			_results = results;
			_filePath = dirPath;
			_filePath.append(L"\\").append(std::to_wstring(iteration)).append(L".mcr");

			*(unsigned int*)_fileHeader = _magic;
			*((unsigned short*)_fileHeader + 2) = _iteration;
			*((unsigned short*)_fileHeader + 3) = _addressWidth;
			*((unsigned long long*)_fileHeader + 1) = _resultCount;

			if (cached && results == nullptr)
			{
				LoadResults();
			}
			else if (cached && results != nullptr)
			{
				SaveResults();
			}
			else
			{
				_results = results;
			}
		}

		~MemCompareResult()
		{
			free(_results);
			free(_fileHeader);
		}

		void* GetResults() { return _results; }
		unsigned short GetIteration() { return _iteration; }
		unsigned short GetAddressWidth() { return _addressWidth; }
		unsigned long long GetResultsCount() { return _resultCount; }
		bool IsCached() { return _cached; }
	};

	class MemCompare
	{
	private:
		MemCompare() {}
		MemCompare(MemCompare const&) = delete;
		void operator=(MemCompare const&) = delete;
		static MemCompare& GetInstance()
		{
			static MemCompare Instance;
			return Instance;
		}

		bool _signed = false;
		bool _swapByteorder = false;
		bool _cached = false;
		bool _rewindable = false;
		int _alignment = 4;

		template<typename T> class Operations
		{
		public:
			std::function<bool(T&, T&)> opInt;
			std::function<bool(T&, T&, T&)> opFloat;
			std::function<bool(T&, T&)> opArrInt;
			std::function<bool(T&, T&, T&)> opArrFloat;
			std::function<bool(T&, T&, T&)> opCol;

			inline bool operator()(T m, T p) const { return this->opInt(m, p); }
			static inline bool intEq(T m, T p) { return m == p; }
			static inline bool intNE(T m, T p) { return m != p; }
			static inline bool intLt(T m, T p) { return m < p; }
			static inline bool intLE(T m, T p) { return m <= p; }
			static inline bool intGt(T m, T p) { return m > p; }
			static inline bool intGE(T m, T p) { return m >= p; }
			static inline bool intAnd(T m, T p) { return m & p == p; }
			static inline bool intOr(T m, T p) { return m & p != 0; }
			//static inline bool intByIncr(T m, T p) { return m == p1 + p2; }
			//static inline bool intDecrBy(T m, T p) { return m == p1 - p2; }
			//static inline bool intBetw(T m, T p) { return m >= p1 && m <= p2; }
			//static inline bool intNBetw(T m, T p) { return m <= p1 && m >= p2; }


		};

	public:
		template<typename T> uint64_t InitialUnknown();
		template<typename T> uint64_t InitialKnownFloat(T value, T valueOpt, int op, T precision);
		template<typename T> uint64_t InitialKnownInt(T value, T valueOpt, int op);
		template<typename T> uint64_t InitialKnownText(T value, int op, bool caseSensitive, int codec);
		template<typename T> uint64_t InitialKnownArrayInt(T values[], T valueOpt[], int op);
		template<typename T> uint64_t InitialKnownArrayFloat(T value[], T valueOpt[], int op, T precision);
		template<typename T> uint64_t InitialKnownColorRGB(T value, T valueOpt[], int op, T precision);
		template<typename T> uint64_t InitialKnownColorRGBA(T value, int op, T precision);
		template<typename T> uint64_t InitialKnownColorFloatRGB(T value, int op, T precision);
		template<typename T> uint64_t InitialKnownColorFloatRGBA(T value, int op, T precision);



	};
}


