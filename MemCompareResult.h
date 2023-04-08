#pragma once
#include"definitions.h"
#include"MemDump.h"
#include<functional>
#include<iostream>
#include"OperativeArray.h"

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

		void Deallocate(void* ptr)
		{
			uint64_t size = _addressWidth * _resultCount;

			if (ptr != nullptr)
			{
				free(ptr);
				ptr = nullptr;
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
			free(_fileHeader);
			_fileHeader = nullptr;

			if (headerOnly)
				return;

			free(_offsets);
			_offsets = nullptr;
			free(_values);
			_values = nullptr;
			free(_previousValues);
			_previousValues = nullptr;
			_hasValues = false;
			_hasPreviousValues = false;
			_hasAddresses = false;
		}

		void SetResultValues(dataType* ptr)
		{
			uint64_t size = _addressWidth * _resultCount;
			Deallocate((void*)_values);
			_values = (dataType*)malloc(size);
			memcpy(_values, ptr, size);
			_hasValues = true;
		}

		void SetResultPreviousValues(dataType* ptr)
		{
			uint64_t size = _addressWidth * _resultCount;
			Deallocate((void*)_previousValues);
			_previousValues = (dataType*)malloc(size);
			memcpy(_previousValues, ptr, size);
			_hasPreviousValues = true;
		}

		void SetResultOffsets(addressType* ptr)
		{
			uint64_t size = _addressWidth * _resultCount;
			Deallocate((void*)_offsets);
			_offsets = (addressType*)malloc(size);
			memcpy(_offsets, ptr, size);
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