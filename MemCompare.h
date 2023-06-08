#pragma once
#include"definitions.h"
#include"MemDump.h"
#include<functional>
#include<iostream>
#include"MemCompareResult.h"
#include"MemCompareOperations.h"
#include <type_traits>

namespace Xertz
{
	template <typename dataType, typename addressType> class MemCompare
	{
	private:
		MemCompare(){}
		MemCompare(MemCompare const&) = delete;
		void operator=(MemCompare const&) = delete;
		static MemCompare& GetInstance()
		{
			static MemCompare Instance;
			return Instance;
		}

		uint32_t _iterationCount = 0;
		std::vector<MemCompareResult<dataType, addressType>*> _results{};
		Xertz::MemDump _currentDump;

		bool _signed = false;
		bool _cached = false;
		bool _rewindable = false;
		bool _zip = false;
		int _alignment = 4;
		uint32_t _pid;
		void* _dumpAddress = nullptr;
		uint64_t _dumpSize = 0;
		uint64_t _resultCount = 0;
		std::wstring _dumpDir = L"";
		bool _swapBytes = false;
		dataType _knownValue;
		dataType _secondaryKnownValue;
		uint32_t _condition = 0;
		CompareOperator<dataType> _comparisonOperator = nullptr;
		float _precision = 0.0f;
		int _valueSizeFactor = 1;
		addressType* _addresses = nullptr;
		dataType* _values = nullptr;
		dataType* _previousValues = nullptr;
		DataAccess<dataType> _byteReader;


		std::wstring GenerateFilePath()
		{
			std::wstringstream stream;
			stream << _dumpDir << "results" << _iterationCount << ".bin";
			return stream.str();
		}

		void InitialUnknown()
		{
			for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
			{
				*(_addresses + _resultCount) = offsetDump;
				*(_values + _resultCount) = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));
				++_resultCount;
			}
		}

		void SuccessiveUnknown()
		{
			if (_condition > OR)
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						addressType addr = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType val = *(dataType*)((uint8_t*)_results[_iterationCount - 1]->GetResultValues() + i * _alignment);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisonOperator(readVal, val, _knownValue))
						{
							*(_addresses + _resultCount) = addr;
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = val;
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						addressType addr = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType val = *(dataType*)((uint8_t*)_results[_iterationCount - 1]->GetResultValues() + i * _alignment);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisonOperator(readVal, val, _precision, false))
						{
							*(_addresses + _resultCount) = addr;
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = val;
							++_resultCount;
						}
					}
				}
			}
			else
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						addressType addr = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType val = *(dataType*)((uint8_t*)_results[_iterationCount - 1]->GetResultValues() + i * _alignment);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisonOperator(readVal, val))
						{
							*(_addresses + _resultCount) = addr;
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = val;
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						addressType addr = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType val = *(dataType*)((uint8_t*)_results[_iterationCount - 1]->GetResultValues() + i * _alignment);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisonOperator(readVal, val, _precision, false))
						{
							*(_addresses + _resultCount) = addr;
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = val;
							++_resultCount;
						}
					}
				}
			}
		}

		void InitialKnown()
		{
			if (_condition > OR)
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisonOperator(readVal, _knownValue, _secondaryKnownValue))
						{
							*(_addresses + _resultCount) = offsetDump;
							*(_values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
				else if constexpr (std::is_floating_point_v<dataType>)
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisonOperator(readVal, _knownValue, _precision, false))
						{
							*(_addresses + _resultCount) = offsetDump;
							*(_values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
			}
			else
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisonOperator(readVal, _knownValue))
						{
							*(_addresses + _resultCount) = offsetDump;
							*(_values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
				else if constexpr (std::is_floating_point_v<dataType>)
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisonOperator(readVal, _knownValue, _precision, false))
						{
							*(_addresses + _resultCount) = offsetDump;
							*(_values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
			}
		}

		void InitialKnownRGBA()
		{
			LitColor readValue;
			readValue.SelectType(_knownValue.GetSelectedType());
			DataAccess<uint32_t> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<uint32_t>::readReversed : DataAccess<uint32_t>::read;

			for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
			{
				readValue = byteReader(*(uint32_t*)(_currentDump.GetDump<uint8_t*>() + offsetDump));
					  
				if (_comparisonOperator(readValue, _knownValue, _precision))
				{
					*(_addresses + _resultCount) = offsetDump;
					*((uint32_t*)_values + _resultCount) = readValue.GetRGBA();
					++_resultCount;
				}
			}
		}

		void SuccessiveKnownRGBA()
		{
			LitColor readValue;
			DataAccess<uint32_t> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<uint32_t>::readReversed : DataAccess<uint32_t>::read;

			for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
			{
				uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
				readValue = byteReader(*(uint32_t*)(_currentDump.GetDump<uint8_t*>() + readOffset));

				if (_comparisonOperator(readValue, _knownValue, _precision))
				{
					*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
					*((uint32_t*)_values + _resultCount) = readValue.GetRGBA();
					*((uint32_t*)_previousValues + _resultCount) = *(((uint32_t*)_results[_iterationCount - 1]->GetResultValues() + i));
					++_resultCount;
				}
			}
		}

		void InitialKnownRGB565()
		{
			LitColor readValue;
			readValue.SelectType(_knownValue.GetSelectedType());
			DataAccess<uint16_t> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<uint16_t>::readReversed : DataAccess<uint16_t>::read;

			for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
			{
				readValue = byteReader(*(uint16_t*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

				if (_comparisonOperator(readValue, _knownValue, _precision))
				{
					*(_addresses + _resultCount) = offsetDump;
					*((uint16_t*)_values + _resultCount) = readValue.GetRGB565();
					++_resultCount;
				}
			}
		}

		void SuccessiveKnownRGB565()
		{
			LitColor readValue;
			DataAccess<uint16_t> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<uint16_t>::readReversed : DataAccess<uint16_t>::read;

			for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
			{
				uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
				readValue = byteReader(*(uint16_t*)(_currentDump.GetDump<uint8_t*>() + readOffset));

				if (_comparisonOperator(readValue, _knownValue, _precision))
				{
					*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
					*((uint16_t*)_values + _resultCount) = readValue.GetRGB565();
					*((uint16_t*)_previousValues + _resultCount) = *(((uint16_t*)_results[_iterationCount - 1]->GetResultValues() + i));
					++_resultCount;
				}
			}
		}

		void InitialKnownText()
		{
			int format = _knownValue.GetPrimaryFormat();
			int stringLength = 0;

			switch (format)
			{
			case MorphText::UTF16LE: case MorphText::UTF16BE: {
				wchar_t* buf = new wchar_t[_valueSizeFactor];
				buf[_valueSizeFactor/2 - 1] = '\0';
				bool isBigEndian = format == MorphText::UTF16BE ? true : false;

				for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
				{
					memcpy(buf, _currentDump.GetDump<char*>() + offsetDump, _valueSizeFactor - 1);
					if (_knownValue.Compare(buf, true, isBigEndian))
					{
						*(_addresses + _resultCount) = offsetDump;
						std::memcpy(((char*)_values) + _resultCount * _valueSizeFactor, buf, _valueSizeFactor);
						++_resultCount;
					}
				}
				delete[] buf;
			} break;
			case MorphText::UTF32LE: case MorphText::UTF32BE: {
				char32_t* buf = new char32_t[_valueSizeFactor];
				buf[_valueSizeFactor / 4 - 1] = '\0';
				bool isBigEndian = format == MorphText::UTF32BE ? true : false;

				for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
				{
					memcpy(buf, _currentDump.GetDump<char*>() + offsetDump, _valueSizeFactor - 1);
					if (_knownValue.Compare(buf, true, isBigEndian))
					{
						*(_addresses + _resultCount) = offsetDump;
						std::memcpy(((char*)_values) + _resultCount * _valueSizeFactor, buf, _valueSizeFactor);
						++_resultCount;
					}
				}
				delete[] buf;
			} break;
			default: //ASCII, Shift-Jis, UTF-8, ISO-8859-X
			{
				char* buf = new char[_valueSizeFactor];
				buf[_valueSizeFactor-1] = '\0';

				for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
				{
					memcpy(buf, _currentDump.GetDump<char*>() + offsetDump, _valueSizeFactor - 1);
					int x = strlen(buf);
					if (_knownValue.Compare(buf, true, format))
					{
						*(_addresses + _resultCount) = offsetDump;
						std::memcpy(((char*)_values) + _resultCount * _valueSizeFactor, buf, _valueSizeFactor);
						++_resultCount;
					}
				}
				delete[] buf;
			}
			}
		}

		void InitialKnownRGBAF()
		{
			LitColor readValue;
			readValue.SelectType(_knownValue.GetSelectedType());
			DataAccess<float> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<float>::readReversed : DataAccess<float>::read;
			int colorValueCount = (_knownValue.UsesAlpha() ? 4 : 3);

			for (uint64_t offsetDump = 0; offsetDump < _dumpSize - 2*sizeof(float); offsetDump += _alignment)
			{
				for(int rgbaSelect = 0; rgbaSelect < colorValueCount; ++rgbaSelect)
					readValue.SetColorValue<float>( byteReader(*(float*)(_currentDump.GetDump<uint8_t*>() + offsetDump + rgbaSelect * sizeof(float))), rgbaSelect);

				if (_comparisonOperator(readValue, _knownValue, _precision))
				{
					*(_addresses + _resultCount) = offsetDump;
					for (int rgbaSelect = 0; rgbaSelect < colorValueCount; ++rgbaSelect)
					{
						*((float*)_values + _resultCount * colorValueCount + rgbaSelect) = readValue.GetColorValue<float>(rgbaSelect);
					}
					++_resultCount;
				}
			}
		}

		void SuccessiveKnownRGBAF()
		{
			LitColor readValue;
			DataAccess<uint32_t> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<uint32_t>::readReversed : DataAccess<uint32_t>::read;

			for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
			{
				uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
				readValue = byteReader(*(uint32_t*)(_currentDump.GetDump<uint8_t*>() + readOffset));

				if (_comparisonOperator(readValue, _knownValue, _precision))
				{
					*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
					*((uint32_t*)_values + _resultCount) = readValue.GetRGBA();
					*((uint32_t*)_previousValues + _resultCount) = *(((uint32_t*)_results[_iterationCount - 1]->GetResultValues() + i));
					++_resultCount;
				}
			}
		}

		template <typename arrayType> void InitialKnownArray()
		{
			uint32_t itemCount = _knownValue.ItemCount();
			dataType readArr(static_cast<arrayType>(0), itemCount);
			DataAccess<arrayType> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<arrayType>::readReversed : DataAccess<arrayType>::read;

			for (uint64_t offsetDump = 0; offsetDump < _dumpSize - (readArr.ItemCount()-1) * sizeof(arrayType); offsetDump += _alignment)
			{
				for (int index = 0; index < itemCount; ++index)
				{
					readArr[index] = byteReader(*(arrayType*)(_currentDump.GetDump<uint8_t*>() + offsetDump + index*sizeof(arrayType)));
				}

				if (_comparisonOperator(_knownValue, readArr))
				{
					*(_addresses + _resultCount) = offsetDump;

					for (int index = 0; index < itemCount; ++index)
					{
						*((arrayType*)_values + _resultCount * itemCount + index) = readArr[index];
						*((arrayType*)_previousValues + _resultCount * itemCount + index) = 0;
					}
					++_resultCount;
				}
			}
		}

		template <typename arrayType> void SuccessiveKnownArray()
		{
			uint32_t itemCount = _knownValue.ItemCount();
			DataAccess<arrayType> byteReader;
			byteReader.reader = _swapBytes ? DataAccess<arrayType>::readReversed : DataAccess<arrayType>::read;
			dataType readArr(static_cast<arrayType>(0), itemCount);

			for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
			{
				uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
				
				for (int index = 0; index < itemCount; ++index)
				{
					readArr[index] = byteReader(*(arrayType*)(_currentDump.GetDump<uint8_t*>() + readOffset + index * sizeof(arrayType)));
				}

				if (_comparisonOperator(_knownValue, readArr))
				{
					*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);

					for (int index = 0; index < itemCount; ++index)
					{
						*((arrayType*)_values + _resultCount * itemCount + index) = readArr[index];
						*((arrayType*)_previousValues + _resultCount * itemCount + index) = *((arrayType*)(_results[_iterationCount - 1]->GetResultValues()) + _resultCount * itemCount + index);
					}
					++_resultCount;
				}
			}
		}

		void SuccessiveKnown()
		{
			if (_condition > OR)
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisonOperator(readVal, _knownValue, _secondaryKnownValue))
						{
							*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisonOperator(readVal, _knownValue, _precision, false))
						{
							*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
			}
			else
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisonOperator(readVal, _knownValue))
						{
							*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = _byteReader(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisonOperator(readVal, _knownValue, _precision, false))
						{
							*(_addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(_values + _resultCount) = readVal;
							*(_previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
			}
		}

		bool SetAndSaveResults()
		{
			_addresses = (addressType*)realloc(_addresses, sizeof(addressType) * _resultCount);
			_values = (dataType*)realloc(_values, _valueSizeFactor * _resultCount);
			_previousValues = (dataType*)realloc(_previousValues, _valueSizeFactor * _resultCount);

			_results.push_back(new MemCompareResult<dataType, addressType>(false, GenerateFilePath(), _resultCount));
			_results[_iterationCount]->SetValueWidth(&_knownValue);
			_results[_iterationCount]->SetResultValues(_values);
			_results[_iterationCount]->SetResultPreviousValues(_previousValues);
			_results[_iterationCount]->SetResultOffsets(_addresses);

			free(_addresses);
			_addresses = nullptr;
			free(_values);
			_values = nullptr;
			free(_previousValues);
			_previousValues = nullptr;

			if (_cached)
				return true;

			bool saved = _results[_iterationCount]->SaveResults(_zip);
			_results[_iterationCount]->FreeData(false);
			return saved;
		}

		void ReserveResultsSpace()
		{
			_currentDump = Xertz::SystemInfo::GetProcessInfo(_pid).DumpMemory(_dumpAddress, _dumpSize);
			_addresses = (addressType*)malloc(_dumpSize * sizeof(addressType));

			if constexpr (is_instantiation_of<dataType, OperativeArray>::value)
			{
				const std::type_info* typeID = _knownValue.UnderlyingTypeID();
				if (*typeID == typeid(uint8_t))
				{
					_valueSizeFactor = 1;
				}
				else if (*typeID == typeid(uint8_t) || *typeID == typeid(int8_t))
					_valueSizeFactor = 1;
				else if (*typeID == typeid(uint16_t) || *typeID == typeid(int16_t))
					_valueSizeFactor = 2;
				else if (*typeID == typeid(uint64_t) || *typeID == typeid(int64_t) || *typeID == typeid(double))
					_valueSizeFactor = 8;
				else //float, int32, uint32, ...
					_valueSizeFactor = 4;

				_valueSizeFactor *= _knownValue.ItemCount();
			}
			else if constexpr (std::is_same_v<dataType, LitColor>)
			{
				switch (_knownValue.GetSelectedType())
				{
				case LitColor::RGBF:
					_valueSizeFactor = 12;
					break;
				case LitColor::RGBAF:
					_valueSizeFactor = 16;
					break;
				case LitColor::RGB565:
					_valueSizeFactor = 2;
					break;
				default: //RGB888, RGBA8888
					_valueSizeFactor = 4;
				}
			}
			else if constexpr (std::is_same_v<dataType, MorphText>)
			{
				switch (_knownValue.GetPrimaryFormat())
				{
				case MorphText::ASCII:
					_valueSizeFactor = strlen(_knownValue.GetASCII())+1;
					break;
				case MorphText::SHIFTJIS:
					_valueSizeFactor = strlen(_knownValue.GetShiftJis())+1;
					break;
				case MorphText::UTF8:
					_valueSizeFactor = strlen(_knownValue.GetUTF8().c_str()) + 1;
					break;
				case MorphText::UTF16LE: case MorphText::UTF16BE:
					_valueSizeFactor = wcslen(_knownValue.GetUTF16(_knownValue.GetPrimaryFormat() == MorphText::UTF16BE ? true : false).c_str())*2 + 2;
					break;
				case MorphText::UTF32LE: case MorphText::UTF32BE:
					_valueSizeFactor = std::char_traits<char32_t>::length(_knownValue.GetUTF32(_knownValue.GetPrimaryFormat() == MorphText::UTF32BE ? true : false).c_str()) * 4 + 4;
					break;
				default: //ISO-8859-X
					_valueSizeFactor = strlen(_knownValue.GetISO8859X(_knownValue.GetPrimaryFormat())) +1;
				}
				
			}
			else if constexpr(std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
			{
				_valueSizeFactor = sizeof(dataType);
				_byteReader.reader = _swapBytes ? DataAccess<dataType>::readReversed : DataAccess<dataType>::read;
			}

			_values = (dataType*)malloc(_dumpSize * _valueSizeFactor);
			_previousValues = (dataType*)calloc(1, _dumpSize * _valueSizeFactor);
		}

		void SetUpComparasionOperator()
		{
			switch (_condition)
			{
			case EQUAL:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opPrecision = CompareOperator<dataType>::equal_precision;
				else if constexpr (std::is_same_v<dataType, LitColor>)
					_comparisonOperator.opColor = CompareOperator<dataType>::equal_color;
				else// if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::equal;
				break;
			case UNEQUAL:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opPrecision = CompareOperator<dataType>::not_equal_precision;
				else if constexpr (std::is_same_v<dataType, LitColor>)
					_comparisonOperator.opColor = CompareOperator<dataType>::not_equal_color;
				else// if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::not_equal;
				break;
			case LOWER:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opPrecision = CompareOperator<dataType>::lower_precision;
				else if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::lower;
				else if constexpr (std::is_same_v<dataType, LitColor>)
					_comparisonOperator.opColor = CompareOperator<dataType>::lower_color;
				else //OperativeArray
					_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case LOWER_EQUAL:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opPrecision = CompareOperator<dataType>::lower_equal_precision;
				else if constexpr (std::is_integral_v<dataType>)
				_comparisonOperator.opSimple = CompareOperator<dataType>::lower_equal;
				else if constexpr (std::is_same_v<dataType, LitColor>)
					_comparisonOperator.opColor = CompareOperator<dataType>::lower_equal_color;
				else //OperativeArray
					_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case GREATER:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opPrecision = CompareOperator<dataType>::greater_precision;
				else if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::greater;
				else if constexpr (std::is_same_v<dataType, LitColor>)
					_comparisonOperator.opColor = CompareOperator<dataType>::greater_color;
				else //OperativeArray
					_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case GREATER_EQUAL:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opPrecision = CompareOperator<dataType>::greater_equal_precision;
				else if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::greater_equal;
				else if constexpr (std::is_same_v<dataType, LitColor>)
					_comparisonOperator.opColor = CompareOperator<dataType>::greater_equal_color;
				else //OperativeArray
					_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case AND:
				if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::And;
				else
					_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case OR:
				if constexpr (std::is_integral_v<dataType>)
					_comparisonOperator.opSimple = CompareOperator<dataType>::Or;
				else
					_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case INCREASED_BY:
				if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opRangePrecision = CompareOperator<dataType>::increased_precision;
				else if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opRange = CompareOperator<dataType>::increased;
				else //OperativeArray
					_comparisonOperator.opRange = CompareOperator<dataType>::dummy_range;
				break;
			case DECREASED_BY:
				if constexpr (std::is_floating_point_v<dataType>)
				_comparisonOperator.opRangePrecision = CompareOperator<dataType>::decreased_precision;
				else if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opRange = CompareOperator<dataType>::decreased;
				else //OperativeArray
					_comparisonOperator.opRange = CompareOperator<dataType>::dummy_range;
				break;
			case BETWEEN:
				if constexpr (std::is_floating_point_v<dataType>)
				_comparisonOperator.opRangePrecision = CompareOperator<dataType>::between_precision;
				else if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opRange = CompareOperator<dataType>::between;
				else //OperativeArray
					_comparisonOperator.opRange = CompareOperator<dataType>::dummy_range;
				break;
			case NOT_BETWEEN:
				if constexpr (std::is_floating_point_v<dataType>)
				_comparisonOperator.opRangePrecision = CompareOperator<dataType>::not_between_precision;
				else if constexpr (std::is_floating_point_v<dataType>)
					_comparisonOperator.opRange = CompareOperator<dataType>::not_between;
				else //OperativeArray
					_comparisonOperator.opRange = CompareOperator<dataType>::dummy_range;
				break;

			default:
				_comparisonOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			}
		}

	public:
		//I'm doing this kind of overloading here because I can't set template parameters optional (multiple definitions). Some parameters also have to be passed by references since they may be more complex structures
		static uint64_t Iterate(void* dumpAddress, uint64_t dumpSize, int32_t condition, bool isKnownValue, float precision)
		{
			dataType dummyVal;
			dataType dummyVal2;
			isKnownValue = false;
			return GetInstance().Iterate(dumpAddress, dumpSize, condition, isKnownValue, precision, dummyVal, dummyVal2);
		}

		static uint64_t Iterate(void* dumpAddress, uint64_t dumpSize, int32_t condition, bool isKnownValue, float precision, const dataType knownValue)
		{
			dataType dummyVal;
			return GetInstance().Iterate(dumpAddress, dumpSize, condition, isKnownValue, precision, knownValue, dummyVal);
		}

		static uint64_t Iterate(void* dumpAddress, uint64_t dumpSize, int32_t condition, bool isKnownValue, float precision, const dataType knownValue, const dataType secondaryKnownValue)
		{
			GetInstance()._resultCount = 0;
			GetInstance()._knownValue = knownValue;
			GetInstance()._dumpAddress = dumpAddress;
			GetInstance()._dumpSize = dumpSize;
			GetInstance()._secondaryKnownValue = secondaryKnownValue;
			GetInstance()._precision = precision;
			GetInstance()._condition = condition;
			if constexpr (!std::is_same_v<dataType, MorphText>)
				GetInstance().SetUpComparasionOperator();
			GetInstance().ReserveResultsSpace();

			if constexpr (is_instantiation_of<dataType, OperativeArray>::value)
			{
				const std::type_info* typeID = GetInstance()._knownValue.UnderlyingTypeID();

				if (GetInstance()._iterationCount == 0)
				{
					if (isKnownValue)
					{
						if (*typeID == typeid(uint8_t))
							GetInstance().InitialKnownArray<uint8_t>();
						else if (*typeID == typeid(int8_t))
							GetInstance().InitialKnownArray<int8_t>();
						else if (*typeID == typeid(uint16_t))
							GetInstance().InitialKnownArray<uint16_t>();
						else if (*typeID == typeid(int16_t))
							GetInstance().InitialKnownArray<int16_t>();
						else if (*typeID == typeid(int32_t))
							GetInstance().InitialKnownArray<int32_t>();
						//else if (*typeID == typeid(float))
						//	GetInstance().InitialKnownArray<float>();
						else if (*typeID == typeid(uint64_t))
							GetInstance().InitialKnownArray<uint64_t>();
						else if (*typeID == typeid(int64_t))
							GetInstance().InitialKnownArray<int64_t>();
						//else if (*typeID == typeid(double))
						//	GetInstance().InitialKnownArray<double>();
						else
							GetInstance().InitialKnownArray<uint32_t>();
					}
				}
				else
				{
					if (isKnownValue)
					{
						if (*typeID == typeid(uint8_t))
							GetInstance().SuccessiveKnownArray<uint8_t>();
						else if (*typeID == typeid(int8_t))
							GetInstance().SuccessiveKnownArray<int8_t>();
						else if (*typeID == typeid(uint16_t))
							GetInstance().SuccessiveKnownArray<uint16_t>();
						else if (*typeID == typeid(int16_t))
							GetInstance().SuccessiveKnownArray<int16_t>();
						else if (*typeID == typeid(int32_t))
							GetInstance().SuccessiveKnownArray<int32_t>();
						//else if (*typeID == typeid(float))
						//	GetInstance().SuccessiveKnownArray<float>();
						else if (*typeID == typeid(uint64_t))
							GetInstance().SuccessiveKnownArray<uint64_t>();
						else if (*typeID == typeid(int64_t))
							GetInstance().SuccessiveKnownArray<int64_t>();
						//else if (*typeID == typeid(double))
						//	GetInstance().SuccessiveKnownArray<double>();
						else
							GetInstance().SuccessiveKnownArray<uint32_t>();
					}
				}
			}
			else if constexpr (std::is_same_v<LitColor, dataType>)
			{
				int colorType = GetInstance()._knownValue.GetSelectedType();
				if (GetInstance()._iterationCount == 0)
				{
					if (isKnownValue)
					{
						switch (colorType)
						{
						case LitColor::RGBAF:
						case LitColor::RGBF:
							GetInstance().InitialKnownRGBAF();
							break;
						case LitColor::RGB565:
								GetInstance().InitialKnownRGB565();
						default: //RGB888, RGBA8888
							GetInstance().InitialKnownRGBA();
						}
					}
				}
				else
				{
					switch (colorType)
					{
					case LitColor::RGBAF:
					case LitColor::RGBF:
						GetInstance().SuccessiveKnownRGBAF();
						break;
					case LitColor::RGB565:
						GetInstance().SuccessiveKnownRGB565();
					default: //RGB888, RGBA8888
						GetInstance().SuccessiveKnownRGBA();
					}
				}
			}
			else if constexpr (std::is_same_v<MorphText, dataType>)
			{
					GetInstance().InitialKnownText();
			}
			else //float, integral
			{
				if (GetInstance()._iterationCount == 0)
				{

					if (isKnownValue)
						GetInstance().InitialKnown();
					else
						GetInstance().InitialUnknown();
				}
				else
				{
					if (isKnownValue)
						GetInstance().SuccessiveKnown();
					else
						GetInstance().SuccessiveUnknown();
				}
			}

			

			GetInstance().SetAndSaveResults();
			++GetInstance()._iterationCount;
			return GetInstance()._resultCount;
		}

		static void Reset()
		{
			GetInstance()._iterationCount = 0;
			GetInstance()._results.clear();
			GetInstance()._currentDump.~MemDump();
			GetInstance()._signed = false;
			GetInstance()._cached = false;
			GetInstance()._rewindable = false;
			GetInstance()._alignment = 4;
			GetInstance()._pid = 0;
			GetInstance()._dumpAddress = nullptr;
			GetInstance()._dumpSize = 0;
			GetInstance()._resultCount = 0;
			GetInstance()._dumpDir = L"";
			GetInstance()._swapBytes = false;
			GetInstance()._knownValue = (dataType)0;
			GetInstance()._secondaryKnownValue = (dataType)0;
			GetInstance()._condition = 0;
			GetInstance()._comparisonOperator = nullptr;
		}

		static void SetUp(int pid, std::wstring& dir, bool cached, bool swapByptes, int alignment)
		{
			GetInstance()._pid = pid;
			GetInstance()._dumpDir = dir;
			GetInstance()._cached = cached;
			GetInstance()._swapBytes = swapByptes;
			GetInstance()._alignment = alignment;
		}

		
		static std::vector<MemCompareResult<dataType, addressType>*>* GetResults()
		{
			return &GetInstance()._results;
		}

		static uint32_t GetValueItemCount()
		{
			if constexpr (std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
				return 1;
			else
				return GetInstance()._knownValue.ItemCount();
		}

		static dataType& GetPrimaryKnownValue()
		{
			return GetInstance()._knownValue;
		}
	};
}