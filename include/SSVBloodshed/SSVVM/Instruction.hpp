// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_INSTRUCTION
#define SSVVM_INSTRUCTION

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/OpCodes.hpp"
#include "SSVBloodshed/SSVVM/Params.hpp"

namespace ssvvm
{
	struct Instruction
	{
		using Idx = int;

		OpCode opCode;
		Params params;
	};
}

#endif
