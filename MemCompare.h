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
		dataType _knownValue = (dataType)0;
		dataType _secondaryKnownValue = (dataType)0;
		uint32_t _condition = 0;
		CompareOperator<dataType> _comparisionOperator = nullptr;
		float _precision = 0.0f;

		std::wstring GenerateFilePath()
		{
			std::wstringstream stream;
			stream << _dumpDir << "results" << _iterationCount << ".bin";
			return stream.str();
		}

		void InitialUnknown()
		{
			_currentDump = Xertz::SystemInfo::GetProcessInfo(_pid).DumpMemory(_dumpAddress, _dumpSize);
			addressType* addresses = (addressType*)malloc(_dumpSize * sizeof(addressType));
			dataType* values = (dataType*)malloc(_dumpSize * sizeof(dataType));
			dataType* previousValues = (dataType*)calloc(1, _dumpSize * sizeof(dataType));

			DataAccess<dataType> accs;
			accs.reader = _swapBytes ? DataAccess<dataType>::readReversed : DataAccess<dataType>::read;

			for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
			{
				*(addresses + _resultCount) = offsetDump;
				*(values + _resultCount) = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));
				++_resultCount;
			}

			SetAndSaveResults(addresses, values, previousValues);
		}

		void SuccessiveUnknown()
		{
			_currentDump = Xertz::SystemInfo::GetProcessInfo(_pid).DumpMemory(_dumpAddress, _dumpSize);
			addressType* addresses = (addressType*)malloc(_results[_iterationCount - 1]->GetResultCount() * sizeof(addressType));
			dataType* values = (dataType*)malloc(_results[_iterationCount - 1]->GetResultCount() * sizeof(dataType));
			dataType* previousValues = (dataType*)malloc(_dumpSize * sizeof(dataType));

			DataAccess<dataType> accs;
			accs.reader = _swapBytes ? DataAccess<dataType>::readReversed : DataAccess<dataType>::read;
			SetUpComparasionOperator();

			if (_condition > OR)
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						addressType addr = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType val = *(dataType*)((uint8_t*)_results[_iterationCount - 1]->GetResultValues() + i * _alignment);
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisionOperator(readVal, val, _knownValue))
						{
							*(addresses + _resultCount) = addr;
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = val;
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
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisionOperator(readVal, val, _precision, false))
						{
							*(addresses + _resultCount) = addr;
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = val;
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
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisionOperator(readVal, val))
						{
							*(addresses + _resultCount) = addr;
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = val;
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
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + addr));

						if (_comparisionOperator(readVal, val, _precision, false))
						{
							*(addresses + _resultCount) = addr;
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = val;
							++_resultCount;
						}
					}
				}
			}

			


			SetAndSaveResults(addresses, values, previousValues);
		}

		void InitialKnown()
		{
			_currentDump = Xertz::SystemInfo::GetProcessInfo(_pid).DumpMemory(_dumpAddress, _dumpSize);
			addressType* addresses = (addressType*)malloc(_dumpSize * sizeof(addressType));
			dataType* values = (dataType*)malloc(_dumpSize * sizeof(dataType));
			dataType* previousValues = (dataType*)calloc(1, _dumpSize * sizeof(dataType));

			DataAccess<dataType> accs;
			accs.reader = _swapBytes ? DataAccess<dataType>::readReversed : DataAccess<dataType>::read;
			
			if (_condition > OR)
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisionOperator(readVal, _knownValue, _secondaryKnownValue))
						{
							*(addresses + _resultCount) = offsetDump;
							*(values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisionOperator(readVal, _knownValue, _precision, false))
						{
							*(addresses + _resultCount) = offsetDump;
							*(values + _resultCount) = readVal;
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
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisionOperator(readVal, _knownValue))
						{
							*(addresses + _resultCount) = offsetDump;
							*(values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t offsetDump = 0; offsetDump < _dumpSize; offsetDump += _alignment)
					{
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + offsetDump));

						if (_comparisionOperator(readVal, _knownValue, _precision, false))
						{
							*(addresses + _resultCount) = offsetDump;
							*(values + _resultCount) = readVal;
							++_resultCount;
						}
					}
				}
			}

			



			SetAndSaveResults(addresses, values, previousValues);
		}

		void SuccessiveKnown()
		{
			_currentDump = Xertz::SystemInfo::GetProcessInfo(_pid).DumpMemory(_dumpAddress, _dumpSize);
			addressType* addresses = (addressType*)malloc(_results[_iterationCount-1]->GetResultCount() * sizeof(addressType));
			dataType* values = (dataType*)malloc(_results[_iterationCount - 1]->GetResultCount() * sizeof(dataType));
			dataType* previousValues = (dataType*)malloc(_results[_iterationCount - 1]->GetResultCount() * sizeof(dataType));

			DataAccess<dataType> accs;
			accs.reader = _swapBytes ? DataAccess<dataType>::readReversed : DataAccess<dataType>::read;
			SetUpComparasionOperator();

			if (_condition > OR)
			{
				if constexpr (std::is_integral_v<dataType>)
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisionOperator(readVal, _knownValue, _secondaryKnownValue))
						{
							*(addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisionOperator(readVal, _knownValue, _precision, false))
						{
							*(addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
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
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisionOperator(readVal, _knownValue))
						{
							*(addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
				else
				{
					for (uint64_t i = 0; i < _results[_iterationCount - 1]->GetResultCount(); ++i)
					{
						uint64_t readOffset = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
						dataType readVal = accs(*(dataType*)(_currentDump.GetDump<uint8_t*>() + readOffset));

						if (_comparisionOperator(readVal, _knownValue, _precision, false))
						{
							*(addresses + _resultCount) = *(_results[_iterationCount - 1]->GetResultOffsets() + i);
							*(values + _resultCount) = readVal;
							*(previousValues + _resultCount) = *(_results[_iterationCount - 1]->GetResultValues() + i);
							++_resultCount;
						}
					}
				}
			}
			
			SetAndSaveResults(addresses, values, previousValues);
		}

		bool SetAndSaveResults(addressType* addresses, dataType* values, dataType* previousValues)
		{
			if (_iterationCount > 0)
			{
				addresses = (addressType*)realloc(addresses, sizeof(addressType) * _resultCount);
				values = (dataType*)realloc(values, sizeof(dataType) * _resultCount);
				previousValues = (dataType*)realloc(previousValues, sizeof(dataType) * _resultCount);
			}

			_results.push_back(new MemCompareResult<dataType, addressType>(false, GenerateFilePath(), _resultCount));
			_results[_iterationCount]->SetResultValues(values);
			_results[_iterationCount]->SetResultPreviousValues(previousValues);
			_results[_iterationCount]->SetResultOffsets(addresses);

			if (_cached)
				return true;

			bool saved = _results[_iterationCount]->SaveResults(_zip);
			_results[_iterationCount]->FreeData(false);
			return saved;
		}

		void SetUpComparasionOperator()
		{
			switch (_condition)
			{
			case EQUAL:
				if constexpr (std::is_floating_point<dataType>::value)
					_comparisionOperator.opPrecision = CompareOperator<dataType>::equal_precision;
				else
					_comparisionOperator.opSimple = CompareOperator<dataType>::equal;
				break;
			case UNEQUAL:
				if constexpr (std::is_floating_point<dataType>::value)
					_comparisionOperator.opPrecision = CompareOperator<dataType>::not_equal_precision;
				else
				_comparisionOperator.opSimple = CompareOperator<dataType>::not_equal;
				break;
			case LOWER:
				if constexpr (std::is_floating_point<dataType>::value)
					_comparisionOperator.opPrecision = CompareOperator<dataType>::lower_precision;
				else
				_comparisionOperator.opSimple = CompareOperator<dataType>::lower;
				break;
			case LOWER_EQUAL:
				if constexpr (std::is_floating_point<dataType>::value)
					_comparisionOperator.opPrecision = CompareOperator<dataType>::lower_equal_precision;
				else
				_comparisionOperator.opSimple = CompareOperator<dataType>::lower_equal;
				break;
			case GREATER:
				if constexpr (std::is_floating_point<dataType>::value)
					_comparisionOperator.opPrecision = CompareOperator<dataType>::greater_precision;
				else
				_comparisionOperator.opSimple = CompareOperator<dataType>::greater;
				break;
			case GREATER_EQUAL:
				if constexpr (std::is_floating_point<dataType>::value)
					_comparisionOperator.opPrecision = CompareOperator<dataType>::greater_equal_precision;
				else
				_comparisionOperator.opSimple = CompareOperator<dataType>::greater_equal;
				break;
			case AND:
				if constexpr (std::is_integral_v<dataType>)
					_comparisionOperator.opSimple = CompareOperator<dataType>::And;
				else
					_comparisionOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case OR:
				if constexpr (std::is_integral_v<dataType>)
					_comparisionOperator.opSimple = CompareOperator<dataType>::Or;
				else
					_comparisionOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			case INCREASED_BY:
				if constexpr (std::is_integral_v<dataType>)
					_comparisionOperator.opRange = CompareOperator<dataType>::increased;
				else
					_comparisionOperator.opRangePrecision = CompareOperator<dataType>::increased_precision;
				break;
			case DECREASED_BY:
				if constexpr (std::is_integral_v<dataType>)
					_comparisionOperator.opRange = CompareOperator<dataType>::decreased;
				else
					_comparisionOperator.opRangePrecision = CompareOperator<dataType>::decreased_precision;
				break;
			case BETWEEN:
				if constexpr (std::is_integral_v<dataType>)
					_comparisionOperator.opRange = CompareOperator<dataType>::between;
				else
					_comparisionOperator.opRangePrecision = CompareOperator<dataType>::between_precision;
				break;
			case NOT_BETWEEN:
				if constexpr (std::is_integral_v<dataType>)
					_comparisionOperator.opRange = CompareOperator<dataType>::not_between;
				else
					_comparisionOperator.opRangePrecision = CompareOperator<dataType>::not_between_precision;
				break;

			default:
				_comparisionOperator.opSimple = CompareOperator<dataType>::dummy;
				break;
			}
		}

	public:
		//I'm doing this kind of overloading here because I can't set template parameters optional (multiple definitions). Some also have to be references since they may pass arrays
		static uint64_t Iterate(void* dumpAddress, uint64_t dumpSize, int32_t condition, bool isKnownValue, float precision)
		{
			dataType dummyVal;
			dataType dummyVal2;
			isKnownValue = false;
			return GetInstance().Iterate(dumpAddress, dumpSize, condition, isKnownValue, precision, dummyVal, dummyVal2);
		}

		static uint64_t Iterate(void* dumpAddress, uint64_t dumpSize, int32_t condition, bool isKnownValue, float precision, dataType& knownValue)
		{
			dataType dummyVal;
			return GetInstance().Iterate(dumpAddress, dumpSize, condition, isKnownValue, precision, knownValue, dummyVal);
		}

		static uint64_t Iterate(void* dumpAddress, uint64_t dumpSize, int32_t condition, bool isKnownValue, float precision, dataType& knownValue, dataType& secondaryKnownValue)
		{
			GetInstance()._resultCount = 0;
			GetInstance()._knownValue = knownValue;
			GetInstance()._dumpAddress = dumpAddress;
			GetInstance()._dumpSize = dumpSize;
			GetInstance()._secondaryKnownValue = secondaryKnownValue;
			GetInstance()._precision = precision;
			GetInstance()._condition = condition;
			GetInstance().SetUpComparasionOperator();

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
			GetInstance()._comparisionOperator = nullptr;
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
	};
}