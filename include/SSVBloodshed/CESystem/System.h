// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_SYSTEM
#define SSVOB_CESYSTEM_SYSTEM

#include "SSVBloodshed/CESystem/Common.h"
#include "SSVBloodshed/CESystem/Entity.h"
#include "SSVBloodshed/CESystem/SystemBase.h"

namespace ssvces
{
	namespace Internal
	{
		template<typename... TArgs> struct Filter
		{
			static const TypeIdsBitset& getTypeIds() { return getTypeIdsBitsetStorage<TArgs...>(); }
		};

		template<typename TPReq, typename TPArgs> struct ExpHelper;
		template<template<typename...> class TPReq, typename... TP1, template<typename...> class TPArgs, typename... TP2> struct ExpHelper<TPReq<TP1...>, TPArgs<TP2...>>
		{
			template<typename TDerived> inline static void processImpl(TDerived& mDerived, TP1... mArgs1, TP2&&... mArgs2) { mDerived.process(*mArgs1..., std::forward<TP2>(mArgs2)...); }
		};
	}

	template<typename... TArgs> struct Req : public Internal::Filter<TArgs...>
	{
		using TupleType = std::tuple<Entity*, TArgs*...>;
		template<typename T> static inline void emplaceTuple(T& mContainer, Entity& mEntity) { mContainer.emplace_back(std::tuple_cat(std::tuple<Entity*>{&mEntity}, buildComponentsTuple<TArgs...>(mEntity))); }
	};
	template<typename... TArgs> struct Not : public Internal::Filter<TArgs...> { };

	template<typename TDerived, typename TReq, typename TNot = Not<>> class System : public SystemBase
	{
		private:
			using TupleType = typename TReq::TupleType;
			std::vector<TupleType> tuples;

			template<typename... TArgs> inline void processAllImpl(TupleType& mTuple, TArgs&&... mArgs)
			{
				ssvu::explode(&Internal::ExpHelper<typename std::remove_reference<decltype(mTuple)>::type, std::tuple<TArgs...>>::template processImpl<TDerived>,
					std::tuple_cat(std::tuple<TDerived&>(*reinterpret_cast<TDerived*>(this)), mTuple, std::tuple<TArgs...>{std::forward<TArgs>(mArgs)...}));
			}

			inline static constexpr Entity& getEntity(const TupleType& mTuple) noexcept { return *std::get<0>(mTuple); }
			template<unsigned int TIdx> inline static constexpr auto getComponent(const TupleType& mTuple) noexcept -> decltype(*std::get<TIdx + 1>(mTuple)) { return *std::get<TIdx + 1>(mTuple); }
			inline void refresh() override { ssvu::eraseRemoveIf(tuples, [](const TupleType& mTuple){ return getEntity(mTuple).mustDestroy || getEntity(mTuple).mustRematch; }); }
			inline void registerEntity(Entity& mEntity) override { TReq::emplaceTuple(tuples, mEntity); }

		public:
			inline System() noexcept : SystemBase{TReq::getTypeIds(), TNot::getTypeIds()} { }
			template<typename... TArgs> inline void processAll(TArgs&&... mArgs) { for(auto& t : tuples) processAllImpl(t, std::forward<TArgs>(mArgs)...); }
	};
}

#endif
