// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_PROGRAM
#define SSVVM_PROGRAM

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Instruction.hpp"

namespace ssvvm
{
	struct Program
	{
		private:
			std::vector<Instruction> instructions;

		public:
			inline Program& operator+=(Instruction mInstruction)					{ instructions.emplace_back(std::move(mInstruction)); return *this; }
			inline const Instruction& operator[](std::size_t mIdx) const noexcept	{ SSVU_ASSERT(mIdx < instructions.size()); return instructions[mIdx]; }
			inline std::size_t getSize() const noexcept								{ return instructions.size(); }
	};
}

#endif
