#pragma once
#include"definitions.h"
#include"MemDump.h"
#include<functional>
#include<iostream>

namespace Xertz
{
	template<typename dataType, typename addressType> class MemCompareResult
	{
	private:
		uint8_t* _fileHeader = (uint8_t*)calloc(1, 16);
		enum HeaderOffsets { MAGIC = 0, RESULT_COUNT = 4, ADDRESS_WIDTH = 12, VALUE_WIDTH = 13 };
		const uint32_t _magic = 0x6d636d77; //memcompare magic word "mcmw"
		dataType* _values = nullptr;
		addressType* _offsets = nullptr;
		std::wstring _path;
		uint64_t _resultCount = 0;
		uint8_t _addressWidth = 0;
		uint8_t _valueWidth = 0;
		uint64_t _fileSize = 0;
		bool _cached = false;
		bool _zipped = false;
		bool _fileHeaderAllocatedWithNew = false;
		bool _offsetsAllocatedWithNew = false;
		bool _valuesAllocatedWithNew = false;

		void CreateHeader()
		{
			*(uint32_t*)_fileHeader = _magic;
			*(uint64_t*)((uint8_t*)_fileHeader + RESULT_COUNT) = _resultCount;
			*((uint8_t*)_fileHeader + ADDRESS_WIDTH) = _addressWidth;
			*((uint8_t*)_fileHeader + VALUE_WIDTH) = _valueWidth;
		}
		
		template<typename whatever>void SetAllocationFlags(whatever* ptr, bool& flag)
		{
			if constexpr(std::is_class<whatever>::value)
			{
				flag = (dynamic_cast<void*>(new(std::nothrow) whatever[1]) == dynamic_cast<void*>(ptr));
			}
			else
			{
				void* tmp = malloc(sizeof(whatever));
				flag = (tmp == (void*)ptr);
				free(tmp);
			}
		}

	public:
		MemCompareResult() {};
		MemCompareResult<typename dataType, typename addressType>(bool zipped, std::wstring& path, uint64_t resultCount = 0)
		{
			_zipped = zipped;
			_addressWidth = sizeof(addressType);
			_valueWidth = sizeof(dataType);
			_resultCount = resultCount;
			_path = path;
		}

		~MemCompareResult()
		{
			FreeData(false);
		}

		dataType* GetResultsValues()
		{
			return _values;
		}

		addressType* GetResultOffsets()
		{
			return _offsets;
		}

		void FreeData(bool headerOnly)
		{
			if (_fileHeaderAllocatedWithNew)
			{
				delete[] _fileHeader;
				_fileHeader = nullptr;
			}
			else
				free(_fileHeader);

			if (headerOnly)
				return;

			if (_offsetsAllocatedWithNew)
			{
				delete[] _offsets;
				_offsets = nullptr;
			}
			else
				free(_offsets);

			if (_valuesAllocatedWithNew)
			{
				delete[] _values;
				_values = nullptr;
			}
			else
				free(_values);
		}

		void SetResultValues(dataType* ptr)
		{
			_values = ptr;
			SetAllocationFlags(ptr, _valuesAllocatedWithNew);
		}

		void SetResultOffsets(addressType* ptr)
		{
			_offsets = ptr;
			SetAllocationFlags(ptr, _offsetsAllocatedWithNew);
		}

		void SetResultCount(uint64_t count)
		{
			_resultCount = count;
		}

		bool SaveResults(bool zipped)
		{
			CreateHeader();

			if (!Xertz::SaveBinary(_path, _fileHeader, 16))
				return false;

			if (!Xertz::SaveBinary(_path, _offsets, _resultCount * _addressWidth, true))
				return false;

			if (!Xertz::SaveBinary(_path, _values, _resultCount * _valueWidth, true))
				return false;

			return true;
		}

		bool LoadResults(bool zipped)
		{
			uint64_t readSize = 16;
			FreeData(true);
			if (!Xertz::LoadBinary(_path, (void*&)_fileHeader, readSize, 0))
				return false;

			if (*(uint32_t*)_fileHeader != _magic)
			{
				free(_fileHeader);
				return false;
			}

			if (_addressWidth != *((uint8_t*)_fileHeader + ADDRESS_WIDTH) || _valueWidth != *((uint8_t*)_fileHeader + VALUE_WIDTH))
				return false;

			_resultCount = *(uint64_t*)(_fileHeader + RESULT_COUNT);
			readSize = _resultCount * _addressWidth;
			if (!Xertz::LoadBinary(_path, (void*&)_offsets, readSize, 16))
				return false;

			readSize = _resultCount * _valueWidth;
			if (!Xertz::LoadBinary(_path, (void*&)_values, readSize, 16 + _resultCount * _addressWidth))
				return false;

			return true;
		}

		void operator=(const MemCompareResult<typename dataType, typename addressType>& other)
		{
			_path = other._path;
			_resultCount = other._resultCount;
			_addressWidth = other._addressWidth;
			_valueWidth = other._valueWidth;
			_cached = other._cached;
			_zipped = other._zipped;
			_fileSize = other._fileSize;

			if (_fileHeader)
				memcpy(_fileHeader, other._fileHeader, 16);
			if (_values)
				memcpy(_values, other._values, _fileSize);
			if (_offsets)
				memcpy(_offsets, other._offsets, _fileSize);
		}
	};

	template <typename dataType, typename addressType> class MemCompare
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

		dataType* _values = nullptr;
		addressType* _offsets = nullptr;
		uint32_t _iterationCount = 0;
		std::vector<MemCompareResult<dataType, addressType>> _results{};
		Xertz::MemDump _currentDump;

		bool _signed = false;
		bool _swapByteorder = false;
		bool _cached = false;
		bool _rewindable = false;
		int _alignment = 4;
		uint32_t _pid;
		void* _dumpAddress = nullptr;
		uint64_t _dumpSize = 0;
		uint64_t _resultCount = 0;
		std::wstring dumpDir = L"F:\\test\\file.bin";

		void InitialUnknown()
		{
			_currentDump = Xertz::SystemInfo::GetProcessInfo(_pid).DumpMemory(_dumpAddress, _dumpSize);
			addressType* addresses = (addressType*)malloc(_dumpSize * sizeof(addressType));
			dataType* values = (dataType*)malloc(_dumpSize * sizeof(dataType));

			for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
			{
				*(addresses + _resultCount) = offsetDump;
				*(values + _resultCount) = *(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump);
				++_resultCount;
			}


			_results.push_back(MemCompareResult<dataType, addressType>(false, dumpDir, _resultCount));
			_results[0].SetResultValues(values);
			_results[0].SetResultOffsets(addresses);
			_results[0].SaveResults(false);
		}

	public:

		static void Iterate(uint32_t pid, void* address, uint64_t size, int32_t condition, bool knownValue)
		{
			GetInstance()._pid = pid;
			GetInstance()._dumpAddress = address;
			GetInstance()._dumpSize = size;

			if (GetInstance()._iterationCount == 0)
			{
				if (knownValue)
					;
				else
				GetInstance().InitialUnknown();
			}
		}

		void Reset()
		{

		}

		bool DumpMemory()
		{

		}
	};

		/*template<typename T> class Operations
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


		};*/
		/*
	public:








		template<typename T> uint64_t InitialKnownFloat(T value, T valueOpt, int op, T precision);
		template<typename T> uint64_t InitialKnownInt(T value, T valueOpt, int op);
		template<typename T> uint64_t InitialKnownText(T value, int op, bool caseSensitive, int codec);
		template<typename T> uint64_t InitialKnownArrayInt(T values[], T valueOpt[], int op);
		template<typename T> uint64_t InitialKnownArrayFloat(T value[], T valueOpt[], int op, T precision);
		template<typename T> uint64_t InitialKnownColorRGB(T value, T valueOpt[], int op, T precision);
		template<typename T> uint64_t InitialKnownColorRGBA(T value, int op, T precision);
		template<typename T> uint64_t InitialKnownColorFloatRGB(T value, int op, T precision);
		template<typename T> uint64_t InitialKnownColorFloatRGBA(T value, int op, T precision);


		*/
	
}


