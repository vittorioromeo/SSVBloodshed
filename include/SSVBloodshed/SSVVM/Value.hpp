// Copyright (c) 2013-2014 Vittorio Romeo
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
			VMVal type{VMVal::Void};
			union { int implInt; float implFloat; };

		public:
			template<typename T> inline static Value create(T mContents) noexcept { return {mContents}; }

			inline Value() = default;
			template<typename T> inline Value(T mContents) noexcept { setType<T>(); set<T>(mContents); }

			template<typename T> inline void setType() noexcept { type = getVMVal<T>(); }
			inline void setType(VMVal mType) noexcept { type = mType; }
			template<typename T> inline void set(T mContents) noexcept;
			template<typename T> inline T get() const noexcept;
			inline VMVal getType() const noexcept { return type; }
	};

	template<> inline void Value::set<int>(int mContents) noexcept		{ SSVU_ASSERT(type == VMVal::Int);	implInt = mContents; }
	template<> inline void Value::set<float>(float mContents) noexcept	{ SSVU_ASSERT(type == VMVal::Float);	implFloat = mContents; }

	template<> inline int Value::get<int>() const noexcept				{ SSVU_ASSERT(type == VMVal::Int);	return implInt; }
	template<> inline float Value::get<float>() const noexcept			{ SSVU_ASSERT(type == VMVal::Float);	return implFloat; }
}

#endif
