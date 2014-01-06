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

		template<typename> struct ReflectedEnumImpl;

		template<template<typename> class T, typename TEnum> struct ReflectedEnumImpl<T<TEnum>>
		{
			using EnumType = T<TEnum>;
			inline static const std::vector<std::string>& getElementsAsStrings() noexcept
			{
				static std::vector<std::string> result(getSplittedEnumVarArgs(EnumType::getEnumString()));
				return result;
			}
			inline static std::size_t getElementCount() noexcept
			{
				return getElementsAsStrings().size();
			}
			inline static const std::string& getElementAsString(TEnum mElement) noexcept
			{
				assert(!contains(EnumType::getEnumString(), '='));
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


	#define SSVU_REFLECTED_ENUM_CUSTOM_CREATE_ELEMENT(mIdx, mData, mArg)	SSVU_PP_TPL_ELEM(0, mArg) = SSVU_PP_TPL_ELEM(1, mArg) SSVU_PP_COMMA_IF(mIdx)
	#define SSVU_REFLECTED_ENUM_CUSTOM_CREATE_TOSTR(mIdx, mData, mArg)		template<> inline const std::string& SSVU_PP_TPL_ELEM(0, mData) < SSVU_PP_TPL_ELEM(1, mData) > :: getAsString < SSVU_PP_TPL_ELEM(1, mData) :: SSVU_PP_TPL_ELEM(0, mArg) >() noexcept \
																			{ \
																				static std::string result{SSVU_PP_STRINGIFY(SSVU_PP_TPL_ELEM(0, mArg))}; return result; \
																			}

	#define SSVU_REFLECTED_ENUM_CUSTOM_SPECIALIZATIONS(mManagerName, mName, ...) SSVU_PP_FOREACH_IDX(SSVU_REFLECTED_ENUM_CUSTOM_CREATE_TOSTR, SSVU_PP_TPL(mManagerName, mName), __VA_ARGS__)

	#define CREATE_MAPENTRY(mIdx, mData, mArg) { mData :: SSVU_PP_TPL_ELEM(0, mArg) , SSVU_PP_STRINGIFY(SSVU_PP_TPL_ELEM(0, mArg)) } SSVU_PP_COMMA_IF(mIdx)

	#define SSVU_REFLECTED_ENUM_CUSTOM_DEFINE_MANAGER(mName) template<typename> class mName

	#define SSVU_REFLECTED_ENUM_CUSTOM(mManagerName, mName, mUnderlying, ...) \
		enum class mName : mUnderlying \
		{ \
			SSVU_PP_FOREACH_IDX(SSVU_REFLECTED_ENUM_CUSTOM_CREATE_ELEMENT, SSVU_PP_EMPTY(), __VA_ARGS__) \
		}; \
		template<> struct mManagerName<mName> \
		{ \
			template<mName TVal> inline static const std::string& getAsString() noexcept; \
			inline static const std::string& getAsString(mName mValue) \
			{ \
				static std::map<mName, std::string> map \
				{ \
					SSVU_PP_FOREACH_IDX(CREATE_MAPENTRY, mName, __VA_ARGS__) \
				}; \
				return map[mValue]; \
			} \
		}; \
		SSVU_REFLECTED_ENUM_CUSTOM_SPECIALIZATIONS(mManagerName, mName, __VA_ARGS__)

	//




	// TODO: cleanup and move to ssvu

	SSVU_REFLECTED_ENUM_DEFINE_MANAGER(TestManager);
	SSVU_REFLECTED_ENUM_CUSTOM(TestManager, TestEnum, int, (A, 5), (B, 4), (C, -3));

	SSVU_TEST("TESTAGLAOP")
	{
		EXPECT(int(TestEnum::A) == 5);
		EXPECT(int(TestEnum::B) == 4);
		EXPECT(int(TestEnum::C) == -3);

		EXPECT(TestManager<TestEnum>::getAsString<TestEnum::A>() == "A");
		EXPECT(TestManager<TestEnum>::getAsString<TestEnum::B>() == "B");
		EXPECT(TestManager<TestEnum>::getAsString<TestEnum::C>() == "C");

		EXPECT(TestManager<TestEnum>::getAsString(TestEnum::A) == "A");
		EXPECT(TestManager<TestEnum>::getAsString(TestEnum::B) == "B");
		EXPECT(TestManager<TestEnum>::getAsString(TestEnum::C) == "C");
	}
	SSVU_TEST_END();

}

namespace ssvvm
{
	#define SSVVM_CREATE_MFPTR(mIdx, mData, mArg) & SSVU_PP_EXPAND(T) :: SSVU_PP_EXPAND(mArg) SSVU_PP_COMMA_IF(mIdx)

	#define SSVVM_CREATE_OPCODE_DATABASE(...)	\
		SSVU_REFLECTED_ENUM_DEFINE_MANAGER(ReflectedEnum); \
		SSVU_REFLECTED_ENUM(ReflectedEnum, OpCode, std::size_t, __VA_ARGS__); \
		template<typename T> inline VMFnPtr<T> getVMFnPtr(OpCode mOpCode) noexcept \
		{ \
			static VMFnPtr<T> fnPtrs[] \
			{ \
				SSVU_PP_FOREACH_IDX(SSVVM_CREATE_MFPTR, SSVU_PP_EMPTY(), __VA_ARGS__) \
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
