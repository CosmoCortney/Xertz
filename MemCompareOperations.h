#pragma once
#include"definitions.h"
#include"MemDump.h"
#include<functional>
#include<iostream>
#include"MemCompareResult.h"
#include <type_traits>

namespace Xertz
{
	template <typename dataType> class DataAccess
	{
	public:
		std::function<dataType(dataType)> reader;

		inline dataType operator()(dataType ptr) const { return this->reader(ptr); }

		static inline dataType read(dataType ptr) { return ptr; }
		static inline dataType readReversed(dataType ptr) { return SwapBytes<dataType>(ptr); }
	};

	template<typename dataType> class CompareOperator
	{
	public:
		std::function<bool(dataType&, dataType&)> opSimple;
		std::function<bool(dataType&, dataType&, dataType&)> opRange;
		std::function<bool(dataType&, dataType&, float&)> opPrecision;
		std::function<bool(dataType&, dataType&, dataType&, float&)> opRangePrecision;

		inline bool operator()(dataType currentVal, dataType oldOrKnown) const { return this->opSimple(currentVal, oldOrKnown); }
		static inline bool equal(dataType currentVal, dataType oldOrKnown) { return currentVal == oldOrKnown; }
		static inline bool not_equal(dataType currentVal, dataType oldOrKnown) { return currentVal != oldOrKnown; }
		static inline bool lower(dataType currentVal, dataType oldOrKnown) { return currentVal < oldOrKnown; }
		static inline bool lower_equal(dataType currentVal, dataType oldOrKnown) { return currentVal <= oldOrKnown; }
		static inline bool greater(dataType currentVal, dataType oldOrKnown) { return currentVal > oldOrKnown; }
		static inline bool greater_equal(dataType currentVal, dataType oldOrKnown) { return currentVal >= oldOrKnown; }
		static inline bool And(dataType currentVal, dataType oldOrKnown) { return currentVal & oldOrKnown == oldOrKnown; }
		static inline bool Or(dataType currentVal, dataType oldOrKnown) { return currentVal & oldOrKnown != 0; }
		static inline bool dummy(dataType x, dataType y) { return false; }

		inline bool operator()(dataType currentVal, dataType oldOrRangeMin, dataType offsetOrRangeMax) const { return this->opRange(currentVal, oldOrRangeMin, offsetOrRangeMax); }
		static inline bool increased(dataType currentVal, dataType old, dataType offset) { return currentVal == old + offset; }
		static inline bool decreased(dataType currentVal, dataType old, dataType offset) { return currentVal == old - offset; }
		static inline bool between(dataType currentVal, dataType rangeMin, dataType RangeMax) { return currentVal >= rangeMin && currentVal <= RangeMax; }
		static inline bool not_between(dataType currentVal, dataType rangeMin, dataType RangeMax) { return currentVal < rangeMin&& currentVal > RangeMax; }
		static inline bool dummy_range(dataType x, dataType y, dataType z) { return false; }

		inline bool operator()(dataType currentVal, dataType oldOrKnown, float precision, bool whatever) const { return this->opPrecision(currentVal, oldOrKnown, precision); } //bool is needed to make overload unique if 3rd parameter of opRange is a float
		static inline bool equal_precision(dataType currentVal, dataType oldOrKnown, float precision) { return  currentVal >= oldOrKnown * precision && currentVal <= oldOrKnown * (2.0 - precision); } //0.01 = 1% accurate. 0.99 = 99% accurate
		static inline bool not_equal_precision(dataType currentVal, dataType oldOrKnown, float precision) { return !equal_precision(currentVal, oldOrKnown, precision); }
		static inline bool lower_precision(dataType currentVal, dataType oldOrKnown, float precision) { return currentVal < oldOrKnown * precision; }
		static inline bool lower_equal_precision(dataType currentVal, dataType oldOrKnown, float precision) { return !greater_precision(currentVal, oldOrKnown, precision); }
		static inline bool greater_precision(dataType currentVal, dataType oldOrKnown, float precision) { return currentVal > oldOrKnown * (2.0 - precision); }
		static inline bool greater_equal_precision(dataType currentVal, dataType oldOrKnown, float precision) { return !lower_precision(currentVal, oldOrKnown, precision); }
		static inline bool dummy_precision(dataType x, dataType y, float z) { return false; }

		inline bool operator()(dataType currentVal, dataType oldOrRangeMin, dataType offsetOrRangeMax, float precision) const { return this->opRangePrecision(currentVal, oldOrRangeMin, offsetOrRangeMax, precision); }
		static inline bool increased_precision(dataType currentVal, dataType old, dataType offset, float precision) { return equal_precision(currentVal, old + offset, precision); }
		static inline bool decreased_precision(dataType currentVal, dataType old, dataType offset, float precision) { return equal_precision(currentVal, old - offset, precision); }
		static inline bool between_precision(dataType currentVal, dataType rangeMin, dataType RangeMax, float precision) { return greater_equal_precision(currentVal, rangeMin, precision) && lower_equal_precision(currentVal, RangeMax, precision); }
		static inline bool not_between_precision(dataType currentVal, dataType rangeMin, dataType RangeMax, float precision) { return lower_precision(currentVal, rangeMin, precision) && greater_precision(currentVal, RangeMax, precision); }
		static inline bool dummy_range_precision(dataType x, dataType y, dataType z, float w) { return false; }

		template<typename datatype> static CompareOperator<dataType>* GetCompareOperator(uint32_t condition, float precision = 1.0f)
		{
			CompareOperator<dataType> fx;

			if constexpr (std::is_floating_point<datatype>)
			{
				if (precision > 0.99f)
				{
					switch (condition)
					{
					case Xertz::EQUAL: fx.op = CompareOperator<dataType>::equal; break;
					case Xertz::UNEQUAL: fx.op = CompareOperator<dataType>::not_equal; break;
					case Xertz::GREATER: fx.op = CompareOperator<dataType>::greater; break;
					case Xertz::GREATER_EQUAL: fx.op = CompareOperator<dataType>::greater_equal; break;
					case Xertz::LOWER: fx.op = CompareOperator<dataType>::lower; break;
					case Xertz::LOWER_EQUAL: fx.op = CompareOperator<dataType>::lower_equal; break;
					default: fx.op = functor<dataType>::dummy; break;
					}
					return fx;
				}

				switch (condition)
				{
				case Xertz::EQUAL: fx.opPrecision = CompareOperator<dataType>::equal_precision; break;
				case Xertz::UNEQUAL: fx.opPrecision = CompareOperator<dataType>::not_equal_precision; break;
				case Xertz::GREATER: fx.opPrecision = CompareOperator<dataType>::greater_precision; break;
				case Xertz::GREATER_EQUAL: fx.opPrecision = CompareOperator<dataType>::greater_equal_precision; break;
				case Xertz::LOWER: fx.opPrecision = CompareOperator<dataType>::lower_precision; break;
				case Xertz::LOWER_EQUAL: fx.opPrecision = CompareOperator<dataType>::lower_equal_precision; break;
				default: fx.opPrecision = functor<dataType>::dummy_precision; break;
				}
				return fx;
			}

			if constexpr (std::is_integral_v<datatype>)
			{
				switch (condition)
				{
				case Xertz::EQUAL: fx.op = CompareOperator<dataType>::equal; break;
				case Xertz::UNEQUAL: fx.op = CompareOperator<dataType>::not_equal; break;
				case Xertz::GREATER: fx.op = CompareOperator<dataType>::greater; break;
				case Xertz::GREATER_EQUAL: fx.op = CompareOperator<dataType>::greater_equal; break;
				case Xertz::LOWER: fx.op = CompareOperator<dataType>::lower; break;
				case Xertz::LOWER_EQUAL: fx.op = CompareOperator<dataType>::lower_equal; break;
				case Xertz::AND: fx.op = CompareOperator<dataType>::And; break;
				case Xertz::OR: fx.op = CompareOperator<dataType>::Or; break;
				}
				return fx;
			}

			/*
			INCREASED_BY = 8,
			DECREASED_BY = 9,
			BETWEEN = 10,
			NOT_BETWEEN = 11*/
		}
	};

}