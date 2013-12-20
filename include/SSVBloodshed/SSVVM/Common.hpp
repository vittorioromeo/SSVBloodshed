// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_COMMON
#define SSVVM_COMMON

#include <cassert>
#include <array>
#include <vector>
#include <memory>
#include <tuple>
#include <SSVUtils/SSVUtils.hpp>

namespace ssvvm
{
	template<typename T> using VMFnPtr = void(T::*)();

	// Built-in value types and conversions
	enum class ValueType
	{
		Void,
		Int,
		Float
	};

	template<typename T> inline constexpr ValueType getValueType() noexcept;
	template<> inline constexpr ValueType getValueType<void>() noexcept			{ return ValueType::Void; }
	template<> inline constexpr ValueType getValueType<int>() noexcept			{ return ValueType::Int; }
	template<> inline constexpr ValueType getValueType<float>() noexcept		{ return ValueType::Float; }
}

#endif
