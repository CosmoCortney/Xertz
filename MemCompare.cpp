#include "MemCompare.h"

Xertz::MemCompareResult::MemCompareResult(bool cached, unsigned short resultWidth, unsigned short addressWidth, unsigned long long resultCount, unsigned short iteration, void* results, std::wstring dirPath)
{
	_cached = cached;
	_iteration = iteration;
	_resultCount = resultCount;
	_addressWidth = addressWidth;
	_resultWidth = resultWidth;
	_results = results;
	_filePathAddr = dirPath;
	_filePathVal = dirPath;
	_filePathAddr.append(L"\\").append(std::to_wstring(iteration)).append(L".mcra");
	_filePathVal.append(L"\\").append(std::to_wstring(iteration)).append(L".mcrv");

	*(unsigned short*)_fileHeaderAddr = _magicAddr;
	*((unsigned short*)_fileHeaderAddr + 2) = _iteration;
	*((unsigned short*)_fileHeaderAddr + 3) = _addressWidth;
	*((unsigned long long*)_fileHeaderAddr + 1) = _resultCount;

	*(unsigned short*)_fileHeaderVal = _magicAddr;
	*((unsigned short*)_fileHeaderVal + 2) = _iteration;
	*((unsigned short*)_fileHeaderVal + 3) = _resultWidth;
	*((unsigned long long*)_fileHeaderVal + 1) = _resultCount;

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

Xertz::MemCompareResult::~MemCompareResult()
{
	//free(_results);
	//free(_fileHeader);
}

bool Xertz::MemCompareResult::SaveResults()
{
	if (!Xertz::SaveBinary(_filePathAddr, _fileHeaderAddr, 16))
		return false;

	if (!Xertz::SaveBinary(_filePathAddr, _results, _resultCount * _addressWidth, true))
		return false;

	//if (!Xertz::SaveBinary(_filePathVal, _fileHeaderVal, 16))
	//	return false;

	//if (!Xertz::SaveBinary(_filePathVal, _results, _resultCount * _addressWidth, true))
	//	return false;
	
	return true;
}

bool Xertz::MemCompareResult::LoadResults()
{/*
	if (Xertz::LoadBinary(_filePath, _fileHeader, 16, 0))
	{
		if (*(unsigned int*)_fileHeader == _magicAddr)
		{
			_iteration = *((unsigned short*)_fileHeader + 2);
			_addressWidth = *((unsigned short*)_fileHeader + 3);
			_resultCount = *((unsigned long long*)_fileHeader + 1);
			_results = malloc(_resultCount * _addressWidth);
			return Xertz::LoadBinary(_filePath, _results, _resultCount * _addressWidth, 16);
		}
	}*/
	return false;
}

void* Xertz::MemCompareResult::GetResults()
{
	return _results;
}

unsigned short Xertz::MemCompareResult::GetIteration()
{
	return _iteration;
}

unsigned short Xertz::MemCompareResult::GetAddressWidth()
{
	return _addressWidth;
}

unsigned long long Xertz::MemCompareResult::GetResultsCount()
{
	return _resultCount;
}

bool Xertz::MemCompareResult::IsCached()
{
	return _cached;
}