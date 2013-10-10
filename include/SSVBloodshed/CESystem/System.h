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
			static const TypeIdsBitset& getTypeIdsBitset() { return getTypeIdsBitsetStorage<TArgs...>(); }
		};
	}

	template<typename... TArgs> struct Req : public Internal::Filter<TArgs...>
	{
		using TupleType = std::tuple<SystemBase*, Entity*, TArgs*...>;
		static TupleType getTuple(SystemBase& mSystemBase, Entity& mEntity) { return std::tuple_cat(std::tuple<SystemBase*, Entity*>{&mSystemBase, &mEntity}, buildComponentsTuple<TArgs...>(mEntity)); }
	};
	template<typename... TArgs> struct Not : public Internal::Filter<TArgs...> { };

	template<typename TReq, typename TNot = Not<>> class System : public SystemBase
	{
		private:
			using TupleType = typename TReq::TupleType;
			std::vector<TupleType> tuples;

		public:
			inline System() noexcept : SystemBase{TReq::getTypeIdsBitset(), TNot::getTypeIdsBitset()} { }

			inline void registerEntity(Entity& mEntity) override { tuples.push_back(TReq::getTuple(*this, mEntity)); }
			inline void refresh() override { ssvu::eraseRemoveIf(tuples, [](const TupleType& mTuple){ return getEntity(mTuple).mustDestroy || getEntity(mTuple).mustRematch; }); }

			inline static constexpr Entity& getEntity(const TupleType& mTuple) noexcept { return *std::get<1>(mTuple); }
			template<unsigned int TIdx> inline static constexpr auto getComponent(const TupleType& mTuple) noexcept -> decltype(*std::get<TIdx + 2>(mTuple)) { return *std::get<TIdx + 2>(mTuple); }
			inline const decltype(tuples)& getTuples() const noexcept { return tuples; }
	};
}

#endif
