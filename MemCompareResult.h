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
		uint8_t* _fileHeader = nullptr;
		enum HeaderOffsets { MAGIC = 0, RESULT_COUNT = 4, ADDRESS_WIDTH = 12, VALUE_WIDTH = 13 };
		const uint32_t _magic = 0x6d636d77; //memcompare magic word "mcmw"
		dataType* _values = nullptr;
		dataType* _previousValues = nullptr;
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
		bool _previousValuesAllocatedWithNew = false;
		bool _hasAddresses = false;
		bool _hasValues = false;
		bool _hasPreviousValues = false;

		void CreateHeader()
		{
			_fileHeader = (uint8_t*)calloc(1, 16);
			*(uint32_t*)_fileHeader = _magic;
			*(uint64_t*)((uint8_t*)_fileHeader + RESULT_COUNT) = _resultCount;
			*((uint8_t*)_fileHeader + ADDRESS_WIDTH) = _addressWidth;
			*((uint8_t*)_fileHeader + VALUE_WIDTH) = _valueWidth;
		}

		template<typename whatever>void SetAllocationFlags(whatever* ptr, bool& flag)
		{
			if constexpr (std::is_class<whatever>::value)
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
		MemCompareResult<typename dataType, typename addressType>(bool zipped, std::wstring path, uint64_t resultCount = 0)
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

		dataType* GetResultValues()
		{
			if(_hasValues)
				return _values;
			return nullptr;
		}

		dataType* GetResultPreviousValues()
		{
			if (_hasPreviousValues)
				return _previousValues;
			return nullptr;
		}

		addressType* GetResultOffsets()
		{
			if (_offsets)
				return _offsets;
			return nullptr;
		}

		uint64_t GetResultCount()
		{
			return _resultCount;
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

			if (_previousValuesAllocatedWithNew)
			{
				delete[] _previousValues;
				_previousValues = nullptr;
			}
			else
				free(_previousValues);

			_hasValues = false;
			_hasPreviousValues = false;
			_hasAddresses = false;
		}

		void SetResultValues(dataType* ptr)
		{
			_values = ptr;
			SetAllocationFlags(ptr, _valuesAllocatedWithNew);
			_hasValues = true;
		}

		void SetResultPreviousValues(dataType* ptr)
		{
			_previousValues = ptr;
			SetAllocationFlags(ptr, _previousValuesAllocatedWithNew);
			_hasPreviousValues = true;
		}

		void SetResultOffsets(addressType* ptr)
		{
			_offsets = ptr;
			SetAllocationFlags(ptr, _offsetsAllocatedWithNew);
			_hasAddresses = true;
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

			if (!Xertz::SaveBinary(_path, _previousValues, _resultCount * _valueWidth, true))
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
				FreeData(true);
				return false;
			}

			if (_addressWidth != *((uint8_t*)_fileHeader + ADDRESS_WIDTH) || _valueWidth != *((uint8_t*)_fileHeader + VALUE_WIDTH))
				return false;

			_resultCount = *(uint64_t*)(_fileHeader + RESULT_COUNT);
			readSize = _resultCount * _addressWidth;
			if (!Xertz::LoadBinary(_path, (void*&)_offsets, readSize, 16))
				return false;
			_hasAddresses = true;

			readSize = _resultCount * _valueWidth;
			if (!Xertz::LoadBinary(_path, (void*&)_values, readSize, 16 + _resultCount * _addressWidth))
				return false;
			_hasValues = true;

			if (!Xertz::LoadBinary(_path, (void*&)_previousValues, readSize, 16 + _resultCount * _addressWidth + _resultCount * _valueWidth))
				return false;
			_hasPreviousValues = true;

			return true;
		}

		bool HasResults()
		{
			return _hasValues && _hasAddresses && _hasPreviousValues;
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
			_hasValues = other._hasValues;
			_hasAddresses = other._hasAddresses;
			_hasPreviousValues = other._hasPreviousValues;

			if (_fileHeader)
				memcpy(_fileHeader, other._fileHeader, 16);
			if (_values)
				memcpy(_values, other._values, sizeof(dataType) * other._resultCount);
			if (_previousValues)
				memcpy(_previousValues, other._previousValues, sizeof(dataType) * other._resultCount);
			if (_offsets)
				memcpy(_offsets, other._offsets, sizeof(addressType) * other._resultCount);
		}
	};
}