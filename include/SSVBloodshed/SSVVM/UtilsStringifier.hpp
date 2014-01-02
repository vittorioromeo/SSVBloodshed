// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_UTILS_STRINGIFIER
#define SSVVM_UTILS_STRINGIFIER

#include "SSVBloodshed/SSVVM/Common.hpp"
#include "SSVBloodshed/SSVVM/Value.hpp"

namespace ssvu
{
	template<> struct Stringifier<ssvvm::Value>
	{
		template<bool TFmt> inline static void impl(std::ostream& mStream, const ssvvm::Value& mValue)
		{
			if(mValue.getType() == ssvvm::VMVal::Void)
			{
				Internal::printBold<TFmt>(mStream, "VOID[");
			}
			else if(mValue.getType() == ssvvm::VMVal::Int)
			{
				Internal::printBold<TFmt>(mStream, "INT[");
				Internal::callStringifyImpl<TFmt>(mStream, mValue.get<int>());
			}
			else if(mValue.getType() == ssvvm::VMVal::Float)
			{
				Internal::printBold<TFmt>(mStream, "FLOAT[");
				Internal::callStringifyImpl<TFmt>(mStream, mValue.get<float>());
			}

			Internal::printBold<TFmt>(mStream, "]");
		}
	};
}

#endif
