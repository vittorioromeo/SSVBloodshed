// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_VALUE
#define SSVVM_VALUE

#include "SSVBloodshed/SSVVM/Common.hpp"

namespace ssvvm
{
	class Value
	{
		private:
			ValueType type{ValueType::Void};
			union { int implInt; float implFloat; };

		public:
			template<typename T> inline static Value create(T mContents) noexcept { return {mContents}; }

			inline Value() = default;
			template<typename T> inline Value(T mContents) noexcept { setType<T>(); set<T>(mContents); }

			template<typename T> inline void setType() noexcept { type = getValueType<T>(); }
			inline void setType(ValueType mType) noexcept { type = mType; }
			template<typename T> inline void set(T mContents) noexcept;
			template<typename T> inline T get() const noexcept;
			inline ValueType getType() const noexcept { return type; }
	};

	template<> inline void Value::set<int>(int mContents) noexcept		{ assert(type == ValueType::Int);	implInt = mContents; }
	template<> inline void Value::set<float>(float mContents) noexcept	{ assert(type == ValueType::Float);	implFloat = mContents; }

	template<> inline int Value::get<int>() const noexcept				{ assert(type == ValueType::Int);	return implInt; }
	template<> inline float Value::get<float>() const noexcept			{ assert(type == ValueType::Float);	return implFloat; }
}

#endif
