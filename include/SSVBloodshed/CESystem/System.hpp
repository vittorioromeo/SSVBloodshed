// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_SYSTEM
#define SSVOB_CESYSTEM_SYSTEM

#include "SSVBloodshed/CESystem/Common.hpp"
#include "SSVBloodshed/CESystem/Entity.hpp"
#include "SSVBloodshed/CESystem/SystemBase.hpp"

namespace ssvces
{
	namespace Internal
	{
		template<typename... TArgs> struct Filter
		{
			static constexpr const TypeIdsBitset& getTypeIds() noexcept { return Internal::getTypeIdsBitset<TArgs...>(); }
		};

		template<typename TPReq, typename TPArgs> struct ExpHelper;
		template<template<typename...> class TPReq, typename... TP1, template<typename...> class TPArgs, typename... TP2> struct ExpHelper<TPReq<TP1...>, TPArgs<TP2...>>
		{
			template<typename TS> inline static void processImpl(TS& mSystem, TP1... mArgs1, TP2&&... mArgs2)	{ mSystem.process(*mArgs1..., std::forward<TP2>(mArgs2)...); }
			template<typename TS> inline static void addedImpl(TS& mSystem, TP1... mArgs1, TP2&&... mArgs2)		{ Internal::callAdded(mSystem, *mArgs1..., std::forward<TP2>(mArgs2)...); }
			template<typename TS> inline static void removedImpl(TS& mSystem, TP1... mArgs1, TP2&&... mArgs2)	{ Internal::callRemoved(mSystem, *mArgs1..., std::forward<TP2>(mArgs2)...); }
		};
	}

	template<typename... TArgs> struct Req : public Internal::Filter<TArgs...>
	{
		using TplType = std::tuple<Entity*, TArgs*...>;
		static inline TplType createTuple(Entity& mEntity) { return std::tuple_cat(std::make_tuple(&mEntity), buildComponentsTpl<TArgs...>(mEntity)); }
		template<typename TS, typename TT1, typename TT2> static inline void onProcess(TS& mSystem, TT1 mTpl1, TT2 mTpl2)
		{
			ssvu::explode(&Internal::ExpHelper<TT1, TT2>::template processImpl<TS>, std::tuple_cat(std::tuple<TS&>(mSystem), mTpl1, mTpl2));
		}
		template<typename TS, typename TT> static inline void onAdded(TS& mSystem, TT mTpl)
		{
			ssvu::explode(&Internal::ExpHelper<TT, std::tuple<>>::template addedImpl<TS>, std::tuple_cat(std::tuple<TS&>(mSystem), mTpl));
		}
		template<typename TS, typename TT> static inline void onRemoved(TS& mSystem, TT mTpl)
		{
			ssvu::explode(&Internal::ExpHelper<TT, std::tuple<>>::template removedImpl<TS>, std::tuple_cat(std::tuple<TS&>(mSystem), mTpl));
		}
	};
	template<typename... TArgs> struct Not : public Internal::Filter<TArgs...> { };

	template<typename TDerived, typename TReq, typename TNot = Not<>> class System : public Internal::SystemBase
	{
		private:
			using Tpl = typename TReq::TplType;
			std::vector<Tpl> tuples;

			inline static constexpr Entity& getEntity(const Tpl& mTpl) noexcept { return *std::get<Entity*>(mTpl); }
			inline TDerived& getThisDerived() noexcept { return *reinterpret_cast<TDerived*>(this); }

			inline void refresh() override
			{
				ssvu::eraseRemoveIf(tuples, [this](const Tpl& mTpl)
				{
					if(getEntity(mTpl).mustDestroy || getEntity(mTpl).mustRematch) { TReq::onRemoved(getThisDerived(), mTpl); return true; }
					return false;
				});
			}
			inline void registerEntity(Entity& mEntity) override
			{
				auto tpl(TReq::createTuple(mEntity)); tuples.emplace_back(tpl);
				TReq::onAdded(getThisDerived(), tpl);
			}

		public:
			inline System() noexcept : SystemBase{TReq::getTypeIds(), TNot::getTypeIds()} { }
			template<typename... TArgs> inline void processAll(TArgs&&... mArgs)
			{
				for(auto& t : tuples) TReq::onProcess(getThisDerived(), t, std::make_tuple(std::forward<TArgs>(mArgs)...));
			}
	};
}

#endif
