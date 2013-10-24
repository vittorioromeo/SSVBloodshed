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
			template<typename TDerived> inline static void processImpl(TDerived& mDerived, TP1... mArgs1, TP2&&... mArgs2)	{ mDerived.process(*mArgs1..., std::forward<TP2>(mArgs2)...); }
			template<typename TDerived> inline static void addedImpl(TDerived& mDerived, TP1... mArgs1, TP2&&... mArgs2)	{ Internal::callAdded(mDerived, *mArgs1..., std::forward<TP2>(mArgs2)...); }
			template<typename TDerived> inline static void removedImpl(TDerived& mDerived, TP1... mArgs1, TP2&&... mArgs2)	{ Internal::callRemoved(mDerived, *mArgs1..., std::forward<TP2>(mArgs2)...); }
		};
	}

	template<typename... TArgs> struct Req : public Internal::Filter<TArgs...>
	{
		using TplType = std::tuple<Entity*, TArgs*...>;
		template<typename T, typename TSystem> static inline void emplaceTuple(TSystem& mSystem, T& mContainer, Entity& mEntity)
		{
			auto tpl(std::tuple_cat(std::tuple<Entity*>{&mEntity}, buildComponentsTpl<TArgs...>(mEntity)));
			onAdded(mSystem, tpl);
			mContainer.emplace_back(tpl);
		}
		template<typename TSystem, typename TTpl, typename TTpl2> static inline void onProcess(TSystem& mSystem, TTpl mTpl, TTpl2 mExtra)
		{
			ssvu::explode(&Internal::ExpHelper<TTpl, TTpl2>::template processImpl<TSystem>, std::tuple_cat(std::tuple<TSystem&>(mSystem), mTpl, mExtra));
		}
		template<typename TSystem, typename TTpl> static inline void onAdded(TSystem& mSystem, TTpl mTpl)
		{
			ssvu::explode(&Internal::ExpHelper<TTpl, std::tuple<>>::template addedImpl<TSystem>, std::tuple_cat(std::tuple<TSystem&>(mSystem), mTpl));
		}
		template<typename TSystem, typename TTpl> static inline void onRemoved(TSystem& mSystem, TTpl mTpl)
		{
			ssvu::explode(&Internal::ExpHelper<TTpl, std::tuple<>>::template removedImpl<TSystem>, std::tuple_cat(std::tuple<TSystem&>(mSystem), mTpl));
		}
	};
	template<typename... TArgs> struct Not : public Internal::Filter<TArgs...> { };

	template<typename TDerived, typename TReq, typename TNot = Not<>> class System : public SystemBase
	{
		private:
			using TplType = typename TReq::TplType;
			std::vector<TplType> tuples;

			inline static constexpr Entity& getEntity(const TplType& mTpl) noexcept { return *std::get<0>(mTpl); }
			template<unsigned int TIdx> inline static constexpr auto getComponent(const TplType& mTpl) noexcept -> decltype(*std::get<TIdx + 1>(mTpl)) { return *std::get<TIdx + 1>(mTpl); }
			inline void refresh() override
			{
				ssvu::eraseRemoveIf(tuples, [this](const TplType& mTpl)
				{
					if(getEntity(mTpl).mustDestroy || getEntity(mTpl).mustRematch)
					{
						TReq::onRemoved(*reinterpret_cast<TDerived*>(this), mTpl);
						return true;
					}

					return false;
				});
			}
			inline void registerEntity(Entity& mEntity) override { TReq::emplaceTuple(*reinterpret_cast<TDerived*>(this), tuples, mEntity); }

		public:
			inline System() noexcept : SystemBase{TReq::getTypeIds(), TNot::getTypeIds()} { }
			template<typename... TArgs> inline void processAll(TArgs&&... mArgs)
			{
				for(auto& t : tuples) TReq::onProcess(*reinterpret_cast<TDerived*>(this), t, std::tuple<TArgs...>{std::forward<TArgs>(mArgs)...});
			}
	};
}

#endif
