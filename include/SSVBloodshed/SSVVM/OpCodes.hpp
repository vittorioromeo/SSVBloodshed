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
		Halt = 0,

		// Register instructions
		LoadIntCVToR,
		LoadFloatCVToR,
		MoveRVToR,

		// Register-stack instructions
		PushRVToS,
		PopSVToR,
		MoveSBOVToR,

		// Stack instructions
		PushIntCVToS,
		PushFloatCVToS,
		PushSVToS,
		PopSV,

		// Program logic
		GoToPI,
		GoToPIIfIntRV,
		GoToPIIfCompareRVGreater,
		GoToPIIfCompareRVSmaller,
		GoToPIIfCompareRVEqual,
		CallPI,
		ReturnPI,

		// Register basic arithmetic
		IncrementIntRV,
		DecrementIntRV,

		// Stack basic arithmetic
		AddInt2SVs,
		AddFloat2SVs,
		SubtractInt2SVs,
		SubtractFloat2SVs,
		MultiplyInt2SVs,
		MultiplyFloat2SVs,
		DivideInt2SVs,
		DivideFloat2SVs,

		// Comparisons
		CompareIntRVIntRVToR,
		CompareIntRVIntSVToR,
		CompareIntSVIntSVToR,
		CompareIntRVIntCVToR,
		CompareIntSVIntCVToR
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
}

#endif
