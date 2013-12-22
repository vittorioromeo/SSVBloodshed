// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_OPCODES
#define SSVVM_OPCODES

#include "SSVBloodshed/SSVVM/Common.hpp"

namespace ssvvm
{
	enum class OpCode : std::size_t
	{
		// Virtual machine control
		halt = 0,

		// Register instructions
		loadIntCVToR,
		loadFloatCVToR,
		moveRVToR,

		// Register-stack instructions
		pushRVToS,
		popSVToR,
		moveSBOVToR,

		// Stack instructions
		pushIntCVToS,
		pushFloatCVToS,
		pushSVToS,
		popSV,

		// Program logic
		goToPI,
		goToPIIfIntRV,
		goToPIIfCompareRVGreater,
		goToPIIfCompareRVSmaller,
		goToPIIfCompareRVEqual,
		callPI,
		returnPI,

		// Register basic arithmetic
		incrementIntRV,
		decrementIntRV,

		// Stack basic arithmetic
		addInt2SVs,
		addFloat2SVs,
		subtractInt2SVs,
		subtractFloat2SVs,
		multiplyInt2SVs,
		multiplyFloat2SVs,
		divideInt2SVs,
		divideFloat2SVs,

		// Comparisons
		compareIntRVIntRVToR,
		compareIntRVIntSVToR,
		compareIntSVIntSVToR,
		compareIntRVIntCVToR,
		compareIntSVIntCVToR
	};

	template<typename T> inline VMFnPtr<T> getVMFnPtr(OpCode mOpCode) noexcept
	{
		static VMFnPtr<T> fnPtrs[]
		{
			// Virtual machine control
			&T::halt,

			// Register instructions
			&T::loadIntCVToR,
			&T::loadFloatCVToR,
			&T::moveRVToR,

			// Register-stack instructions
			&T::pushRVToS,
			&T::popSVToR,
			&T::moveSBOVToR,

			// Stack instructions
			&T::pushIntCVToS,
			&T::pushFloatCVToS,
			&T::pushSVToS,
			&T::popSV,

			// Program logic
			&T::goToPI,
			&T::goToPIIfIntRV,
			&T::goToPIIfCompareRVGreater,
			&T::goToPIIfCompareRVSmaller,
			&T::goToPIIfCompareRVEqual,
			&T::callPI,
			&T::returnPI,

			// Register basic arithmetic
			&T::incrementIntRV,
			&T::decrementIntRV,

			// Stack basic arithmetic
			&T::addInt2SVs,
			&T::addFloat2SVs,
			&T::subtractInt2SVs,
			&T::subtractFloat2SVs,
			&T::multiplyInt2SVs,
			&T::multiplyFloat2SVs,
			&T::divideInt2SVs,
			&T::divideFloat2SVs,

			// Comparisons
			&T::compareIntRVIntRVToR,
			&T::compareIntRVIntSVToR,
			&T::compareIntSVIntSVToR,
			&T::compareIntRVIntCVToR,
			&T::compareIntSVIntCVToR
		};

		return fnPtrs[std::size_t(mOpCode)];
	}

	constexpr const char* strs[]
	{
		// Virtual machine control
		"halt",

		// Register instructions
		"loadIntCVToR",
		"loadFloatCVToR",
		"moveRVToR",

		// Register-stack instructions
		"pushRVToS",
		"popSVToR",
		"moveSBOVToR",

		// Stack instructions
		"pushIntCVToS",
		"pushFloatCVToS",
		"pushSVToS",
		"popSV",

		// Program logic
		"goToPI",
		"goToPIIfIntRV",
		"goToPIIfCompareRVGreater",
		"goToPIIfCompareRVSmaller",
		"goToPIIfCompareRVEqual",
		"callPI",
		"returnPI",

		// Register basic arithmetic
		"incrementIntRV",
		"decrementIntRV",

		// Stack basic arithmetic
		"addInt2SVs",
		"addFloat2SVs",
		"subtractInt2SVs",
		"subtractFloat2SVs",
		"multiplyInt2SVs",
		"multiplyFloat2SVs",
		"divideInt2SVs",
		"divideFloat2SVs",

		// Comparisons
		"compareIntRVIntRVToR",
		"compareIntRVIntSVToR",
		"compareIntSVIntSVToR",
		"compareIntRVIntCVToR",
		"compareIntSVIntCVToR"
	};

	inline constexpr const char* getOpCodeStr(OpCode mOpCode) noexcept { return strs[std::size_t(mOpCode)]; }
}

#endif
