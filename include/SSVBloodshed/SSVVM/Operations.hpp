// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_OPERATIONS
#define SSVVM_OPERATIONS

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"

namespace ssvvm
{
	class VMOperations
	{
		private:
			template<typename T> inline static bool isValid(const Value& mA, const Value& mB) noexcept { return mA.getType() == getVMVal<T>() && mB.getType() == getVMVal<T>(); }

		public:
			template<typename T> inline static Value getAddition(const Value& mA, const Value& mB) noexcept			{ SSVU_ASSERT(isValid<T>(mA, mB)); return {mA.get<T>() + mB.get<T>()}; }
			template<typename T> inline static Value getSubtraction(const Value& mA, const Value& mB) noexcept		{ SSVU_ASSERT(isValid<T>(mA, mB)); return {mA.get<T>() - mB.get<T>()}; }
			template<typename T> inline static Value getMultiplication(const Value& mA, const Value& mB) noexcept	{ SSVU_ASSERT(isValid<T>(mA, mB)); return {mA.get<T>() * mB.get<T>()}; }
			template<typename T> inline static Value getDivision(const Value& mA, const Value& mB) noexcept			{ SSVU_ASSERT(isValid<T>(mA, mB) && mB.get<T>() != T(0)); return {mA.get<T>() / mB.get<T>()}; }

			inline static Value getIntComparison(const Value& mA, const Value& mB) noexcept	{ return getSubtraction<int>(mA, mB); }
	};
}

#endif
