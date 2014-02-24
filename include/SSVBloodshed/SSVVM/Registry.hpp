// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_REGISTRY
#define SSVVM_REGISTRY

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"
#include "SSVBloodshed/SSVVM/Register.hpp"

namespace ssvvm
{
	template<std::size_t TSize> class Registry
	{
		private:
			std::array<Register, TSize> registers;

		public:
			inline Register& get(std::size_t mIdx) noexcept					{ SSVU_ASSERT(mIdx < TSize); return registers[mIdx]; }
			inline const Register& get(std::size_t mIdx) const noexcept		{ SSVU_ASSERT(mIdx < TSize); return registers[mIdx]; }
			inline Value& getValue(std::size_t mIdx) noexcept				{ return get(mIdx).value; }
			inline const Value& getValue(std::size_t mIdx) const noexcept	{ return get(mIdx).value; }
			inline std::size_t getSize() const noexcept						{ return TSize; }
	};
}

#endif
