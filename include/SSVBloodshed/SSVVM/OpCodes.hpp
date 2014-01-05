// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_OPCODES
#define SSVVM_OPCODES

#include "SSVBloodshed/SSVVM/Common.hpp"

namespace ssvu
{
	namespace Internal
	{
		inline std::vector<std::string> getSplittedEnumVarArgs(const std::string& mEnumVarArgs)
		{
			// Splits on comma, and removes everything after '='

			std::vector<std::string> result;
			for(const auto& s : getSplit(mEnumVarArgs, ',')) result.emplace_back(getTrimmedStrLR(std::string(std::begin(s), find(s, '='))));
			return result;
		}

		template<typename T> struct ReflectedEnumImpl;

		template<template<typename> class T, typename TEnum> struct ReflectedEnumImpl<T<TEnum>>
		{
			inline static const std::vector<std::string>& getElementsAsStrings() noexcept
			{
				static std::vector<std::string> result(ssvu::Internal::getSplittedEnumVarArgs(T<TEnum>::getEnumString()));
				return result;
			}
			inline static std::size_t getElementCount() noexcept
			{
				return getElementsAsStrings().size();
			}
			inline static const std::string& getElementAsString(TEnum mElement) noexcept
			{
				assert(!contains(T<TEnum>::getEnumString(), '='));
				return getElementsAsStrings()[std::size_t(mElement)];
			}
		};
	}

	#define SSVU_REFLECTED_ENUM_DEFINE_MANAGER(mName) template<typename> class mName

	#define SSVU_REFLECTED_ENUM(mManagerName, mName, mUnderlying, ...) enum class mName : mUnderlying { __VA_ARGS__ }; \
		template<> class mManagerName<mName> : public ssvu::Internal::ReflectedEnumImpl<mManagerName<mName>> \
		{ \
			friend ssvu::Internal::ReflectedEnumImpl<mManagerName<mName>>; \
			inline static const std::string& getEnumString(){ static std::string result{#__VA_ARGS__}; return result; } \
		}

	// TODO: cleanup and move to ssvu
}

namespace ssvvm
{
	#define SSVVM_CREATE_MFPTR(mX) & SSVU_PP_EXPAND(T) :: SSVU_PP_EXPAND(mX) ,

	// Trailing comma is allowed in arrays
	#define SSVVM_CREATE_OPCODE_DATABASE(...)	\
		SSVU_REFLECTED_ENUM_DEFINE_MANAGER(ReflectedEnum); \
		SSVU_REFLECTED_ENUM(ReflectedEnum, OpCode, std::size_t, __VA_ARGS__); \
		template<typename T> inline VMFnPtr<T> getVMFnPtr(OpCode mOpCode) noexcept \
		{ \
			static VMFnPtr<T> fnPtrs[] \
			{ \
				SSVU_PP_FOREACH(SSVVM_CREATE_MFPTR, __VA_ARGS__) \
			}; \
			return fnPtrs[std::size_t(mOpCode)]; \
		}

	SSVVM_CREATE_OPCODE_DATABASE
	(
		// Virtual machine control
		halt,

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
	)

	inline const std::string& getOpCodeStr(OpCode mOpCode) noexcept { return ReflectedEnum<OpCode>::getElementAsString(mOpCode); }
}

#endif
