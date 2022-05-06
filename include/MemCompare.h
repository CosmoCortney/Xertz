#pragma once
#include"definitions.h"
#include"MemDump.h"
#include<functional>

namespace Xertz
{
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



		struct Result
		{
			bool _cached = false;
			void* _results = nullptr;
			std::wstring _filePath;
		};
	};
}


