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
		std::function<bool(dataType&, dataType&, float&)> opColor;

		inline bool operator()(dataType& currentVal, dataType& oldOrKnown) const { return this->opSimple(currentVal, oldOrKnown); }
		static inline bool equal(dataType& currentVal, dataType& oldOrKnown) { return currentVal == oldOrKnown; }
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
		static inline bool equal_precision(dataType currentVal, dataType oldOrKnown, float precision) { return  currentVal >= oldOrKnown * (float)precision && currentVal <= oldOrKnown * (float)(2.0 - precision); } //0.01 = 1% accurate. 0.99 = 99% accurate
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


		inline bool operator()(LitColor& currentVal, LitColor& oldOrKnown, float precision) const { return this->opColor(currentVal, oldOrKnown, precision); }
		static inline bool equal_color(LitColor& currentVal, LitColor& oldOrKnown, float precision)
		{ 
			if (!currentVal.HadValidColorSource())
				return false;
			return  currentVal >= oldOrKnown - (1.0f - precision) && currentVal <= oldOrKnown + (1.0f - precision);
		} //0.01 = 1% accuracy. 0.99 = 99% accuracy

		static inline bool not_equal_color(LitColor& currentVal, LitColor& oldOrKnown, float precision) { return !equal_color(currentVal, oldOrKnown, precision); }

		static inline bool lower_color(LitColor& currentVal, LitColor& oldOrKnown, float precision)
		{
			if (!currentVal.HadValidColorSource())
				return false;
			return currentVal < oldOrKnown + (1.0f - precision);
		}

		static inline bool lower_equal_color(LitColor& currentVal, LitColor& oldOrKnown, float precision) { return !greater_color(currentVal, oldOrKnown, precision); }

		static inline bool greater_color(LitColor& currentVal, LitColor& oldOrKnown, float precision)
		{
			if (!currentVal.HadValidColorSource())
				return false;
			return currentVal > oldOrKnown - (1.0f - precision);
		}
		static inline bool greater_equal_color(LitColor& currentVal, LitColor& oldOrKnown, float precision) { return !lower_color(currentVal, oldOrKnown, precision); }
	};

}