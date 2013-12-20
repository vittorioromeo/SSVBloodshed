// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_OPERATIONS
#define SSVVM_OPERATIONS

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"

namespace ssvvm
{
	struct VMOperations
	{
		inline static Value getIntAddition(const Value& mA, const Value& mB) noexcept			{ return {mA.get<int>() + mB.get<int>()}; }
		inline static Value getFloatAddition(const Value& mA, const Value& mB) noexcept			{ return {mA.get<float>() + mB.get<float>()}; }

		inline static Value getIntSubtraction(const Value& mA, const Value& mB) noexcept		{ return {mA.get<int>() - mB.get<int>()}; }
		inline static Value getFloatSubtraction(const Value& mA, const Value& mB) noexcept		{ return {mA.get<float>() - mB.get<float>()}; }

		inline static Value getIntMultiplication(const Value& mA, const Value& mB) noexcept		{ return {mA.get<int>() * mB.get<int>()}; }
		inline static Value getFloatMultiplication(const Value& mA, const Value& mB) noexcept	{ return {mA.get<float>() * mB.get<float>()}; }

		inline static Value getIntDivision(const Value& mA, const Value& mB) noexcept			{ return {mA.get<int>() / mB.get<int>()}; }
		inline static Value getFloatDivision(const Value& mA, const Value& mB) noexcept			{ return {mA.get<float>() / mB.get<float>()}; }

		inline static Value getIntComparison(const Value& mA, const Value& mB) noexcept			{ return getIntSubtraction(mA, mB); }
	};
}

#endif
