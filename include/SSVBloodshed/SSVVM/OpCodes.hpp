// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVVM_OPCODES
#define SSVVM_OPCODES

#include "SSVBloodshed/SSVVM/Common.hpp"

namespace ssvu
{
	#define SSVU_FAT_ENUM_IMPL_MK_ELEM_VALS(mIdx, mData, mArg)				SSVPP_TPL_ELEM(mArg, 0) = SSVPP_TPL_ELEM(mArg, 1) SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_ELEM_DEF(mIdx, mData, mArg)				mArg SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_ELEM_DISPATCH(mDispatch)					SSVPP_CAT(SSVU_FAT_ENUM_IMPL_MK_ELEM_, mDispatch)

	#define SSVU_FAT_ENUM_IMPL_MK_BIMAP_ENTRY_VALS(mIdx, mData, mArg)		{ SSVPP_TOSTR(SSVPP_TPL_ELEM(mArg, 0)) , mData :: SSVPP_TPL_ELEM(mArg, 0) } SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_BIMAP_ENTRY_DEF(mIdx, mData, mArg)		{ SSVPP_TOSTR(mArg) , mData :: mArg } SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_BIMAP_ENTRY(mDispatch)					SSVPP_CAT(SSVU_FAT_ENUM_IMPL_MK_BIMAP_ENTRY_, mDispatch)

	#define SSVU_FAT_ENUM_IMPL_MK_ARRAY_ENTRY_VALS(mIdx, mData, mArg)		mData :: SSVPP_TPL_ELEM(mArg, 0) SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_ARRAY_ENTRY_DEF(mIdx, mData, mArg)		mData :: mArg SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_ARRAY_ENTRY(mDispatch)					SSVPP_CAT(SSVU_FAT_ENUM_IMPL_MK_ARRAY_ENTRY_, mDispatch)

	#define SSVU_FAT_ENUM_IMPL_MK_ARRAY_EN_ENTRY_VALS(mIdx, mData, mArg)	SSVPP_TOSTR(SSVPP_TPL_ELEM(mArg, 0)) SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_ARRAY_EN_ENTRY_DEF(mIdx, mData, mArg)		SSVPP_TOSTR(mArg) SSVPP_COMMA_IF(mIdx)
	#define SSVU_FAT_ENUM_IMPL_MK_ARRAY_EN_ENTRY(mDispatch)					SSVPP_CAT(SSVU_FAT_ENUM_IMPL_MK_ARRAY_EN_ENTRY_, mDispatch)

	#define SSVU_FAT_ENUM_IMPL_MK_GETASSTRING(mMgr, mEnum, mX)				template<> inline const std::string& mMgr < mEnum > :: getAsStringImpl < mEnum :: mX >() noexcept { static std::string s{SSVPP_TOSTR(mX)}; return s; }
	#define SSVU_FAT_ENUM_IMPL_MK_GETASSTRING_VALS(mIdx, mData, mArg)		SSVU_FAT_ENUM_IMPL_MK_GETASSTRING(SSVPP_TPL_ELEM(mData, 0), SSVPP_TPL_ELEM(mData, 1), SSVPP_TPL_ELEM(mArg, 0))
	#define SSVU_FAT_ENUM_IMPL_MK_GETASSTRING_DEF(mIdx, mData, mArg)		SSVU_FAT_ENUM_IMPL_MK_GETASSTRING(SSVPP_TPL_ELEM(mData, 0), SSVPP_TPL_ELEM(mData, 1), mArg)
	#define SSVU_FAT_ENUM_IMPL_MK_GETASSTRING_DISPATCH(mDispatch)			SSVPP_CAT(SSVU_FAT_ENUM_IMPL_MK_GETASSTRING_, mDispatch)


	#define SSVU_FAT_ENUM_MGR(mMgr) \
		template<typename> class mMgr \
		{ \
			\
		}

	namespace Internal
	{
		template<std::size_t, typename> struct FatEnumMgrImpl;

		template<std::size_t TS, template<typename> class T, typename TEnum> struct FatEnumMgrImpl<TS, T<TEnum>>
		{
			inline static std::size_t getSize() noexcept									{ return TS; }
			template<TEnum TVal> inline static const std::string& getAsString() noexcept	{ return T<TEnum>::template getAsStringImpl<TVal>(); }
			inline static const std::string& getAsString(TEnum mValue) noexcept				{ SSVU_ASSERT(T<TEnum>::getBimap().has(mValue)); return T<TEnum>::getBimap().at(mValue); }
			inline static TEnum getFromString(const std::string& mValue) noexcept			{ SSVU_ASSERT(T<TEnum>::getBimap().has(mValue)); return T<TEnum>::getBimap().at(mValue); }
		};
	}

	#define SSVU_FAT_ENUM_IMPL(mMgr, mName, mUnderlying, mDispatch, ...) \
		enum class mName : mUnderlying \
		{ \
			SSVPP_FOREACH(SSVU_FAT_ENUM_IMPL_MK_ELEM_DISPATCH(mDispatch), SSVPP_EMPTY(), __VA_ARGS__) \
		}; \
		template<> struct mMgr<mName> : public ssvu::Internal::FatEnumMgrImpl<SSVPP_ARGCOUNT(__VA_ARGS__), mMgr<mName>> \
		{ \
			template<mName TVal> inline static const std::string& getAsStringImpl() noexcept; \
			inline static const ssvu::Bimap<std::string, mName>& getBimap() noexcept \
			{ \
				static ssvu::Bimap<std::string, mName> result \
				{ \
					SSVPP_FOREACH(SSVU_FAT_ENUM_IMPL_MK_BIMAP_ENTRY(mDispatch), mName, __VA_ARGS__) \
				}; \
				return result; \
			} \
			inline static const std::array<mName, SSVPP_ARGCOUNT(__VA_ARGS__)>& getValues() noexcept \
			{ \
				static std::array<mName, SSVPP_ARGCOUNT(__VA_ARGS__)> result \
				{{ \
					SSVPP_FOREACH(SSVU_FAT_ENUM_IMPL_MK_ARRAY_ENTRY(mDispatch), mName, __VA_ARGS__) \
				}}; \
				return result; \
			} \
			inline static const std::array<std::string, SSVPP_ARGCOUNT(__VA_ARGS__)>& getElementNames() noexcept \
			{ \
				static std::array<std::string, SSVPP_ARGCOUNT(__VA_ARGS__)> result \
				{{ \
					SSVPP_FOREACH(SSVU_FAT_ENUM_IMPL_MK_ARRAY_EN_ENTRY(mDispatch), mName, __VA_ARGS__) \
				}}; \
				return result; \
			} \
		}; \
		SSVPP_FOREACH(SSVU_FAT_ENUM_IMPL_MK_GETASSTRING_DISPATCH(mDispatch), SSVPP_TPL_MAKE(mMgr, mName), __VA_ARGS__) \
		SSVU_DEFINE_DUMMY_STRUCT(mMgr, mName, mDispatch)

	#define SSVU_FAT_ENUM_VALS(mMgr, mName, mUnderlying, ...)	SSVU_FAT_ENUM_IMPL(mMgr, mName, mUnderlying, VALS, __VA_ARGS__)
	#define SSVU_FAT_ENUM_DEF(mMgr, mName, mUnderlying, ...)	SSVU_FAT_ENUM_IMPL(mMgr, mName, mUnderlying, DEF, __VA_ARGS__)




	// TODO: cleanup and move to ssvu

	SSVU_FAT_ENUM_MGR(TestManager);
	SSVU_FAT_ENUM_VALS(TestManager, TestEnum, int, (A, 5), (B, 4), (C, -3));
	SSVU_FAT_ENUM_DEF(TestManager, Colors, int, Red, Green, Blue);

	SSVUT_TEST(EnumTestsTemp)
	{
		SSVUT_EXPECT(int(TestEnum::A) == 5);
		SSVUT_EXPECT(int(TestEnum::B) == 4);
		SSVUT_EXPECT(int(TestEnum::C) == -3);

		SSVUT_EXPECT(TestManager<TestEnum>::getAsString<TestEnum::A>() == "A");
		SSVUT_EXPECT(TestManager<TestEnum>::getAsString<TestEnum::B>() == "B");
		SSVUT_EXPECT(TestManager<TestEnum>::getAsString<TestEnum::C>() == "C");

		SSVUT_EXPECT(TestManager<TestEnum>::getAsString(TestEnum::A) == "A");
		SSVUT_EXPECT(TestManager<TestEnum>::getAsString(TestEnum::B) == "B");
		SSVUT_EXPECT(TestManager<TestEnum>::getAsString(TestEnum::C) == "C");

		SSVUT_EXPECT(TestManager<TestEnum>::getFromString("A") == TestEnum::A);
		SSVUT_EXPECT(TestManager<TestEnum>::getFromString("B") == TestEnum::B);
		SSVUT_EXPECT(TestManager<TestEnum>::getFromString("C") == TestEnum::C);

		SSVUT_EXPECT(TestManager<TestEnum>::getSize() == 3);

		for(const auto& v : TestManager<TestEnum>::getValues()) ssvu::lo() << int(v) << std::endl;
		for(const auto& v : TestManager<TestEnum>::getElementNames()) ssvu::lo() << v << std::endl;


		SSVUT_EXPECT(int(Colors::Red) == 0);
		SSVUT_EXPECT(int(Colors::Green) == 1);
		SSVUT_EXPECT(int(Colors::Blue) == 2);

		SSVUT_EXPECT(TestManager<Colors>::getAsString<Colors::Red>() == "Red");
		SSVUT_EXPECT(TestManager<Colors>::getAsString<Colors::Green>() == "Green");
		SSVUT_EXPECT(TestManager<Colors>::getAsString<Colors::Blue>() == "Blue");

		SSVUT_EXPECT(TestManager<Colors>::getAsString(Colors::Red) == "Red");
		SSVUT_EXPECT(TestManager<Colors>::getAsString(Colors::Green) == "Green");
		SSVUT_EXPECT(TestManager<Colors>::getAsString(Colors::Blue) == "Blue");

		SSVUT_EXPECT(TestManager<Colors>::getFromString("Red") == Colors::Red);
		SSVUT_EXPECT(TestManager<Colors>::getFromString("Green") == Colors::Green);
		SSVUT_EXPECT(TestManager<Colors>::getFromString("Blue") == Colors::Blue);

		SSVUT_EXPECT(TestManager<Colors>::getSize() == 3);

		for(const auto& v : TestManager<Colors>::getValues()) ssvu::lo() << int(v) << std::endl;
		for(const auto& v : TestManager<Colors>::getElementNames()) ssvu::lo() << v << std::endl;
	}
}

namespace ssvvm
{
	SSVU_FAT_ENUM_MGR(ReflectedEnum);

	#define SSVVM_CREATE_MFPTR(mIdx, mData, mArg) & SSVPP_EXPAND(T) :: SSVPP_EXPAND(mArg) SSVPP_COMMA_IF(mIdx)

	#define SSVVM_CREATE_OPCODE_DATABASE(...)	\
		SSVU_FAT_ENUM_DEF(ReflectedEnum, OpCode, std::size_t, __VA_ARGS__); \
		template<typename T> inline VMFnPtr<T> getVMFnPtr(OpCode mOpCode) noexcept \
		{ \
			static VMFnPtr<T> fnPtrs[] \
			{ \
				SSVPP_FOREACH(SSVVM_CREATE_MFPTR, SSVPP_EMPTY(), __VA_ARGS__) \
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

	inline const std::string& getOpCodeStr(OpCode mOpCode) noexcept { return ReflectedEnum<OpCode>::getAsString(mOpCode); }
}

#endif
