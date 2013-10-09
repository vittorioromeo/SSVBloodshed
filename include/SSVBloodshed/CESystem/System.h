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
	template<typename... TArgs> class System : public SystemBase
	{
		private:
			using TupleType = std::tuple<Entity*, TArgs*...>;
			std::vector<TupleType> tuples;

		public:
			inline System() noexcept : SystemBase{getTypeIdPackBitset<TArgs...>()} { }

			inline void registerEntity(Entity& mEntity) override { tuples.push_back(std::tuple_cat(std::tuple<Entity*>{&mEntity}, buildComponentsTuple<TArgs...>(mEntity))); }
			inline void refresh() override { ssvu::eraseRemoveIf(tuples, [](const TupleType& mTuple){ return std::get<0>(mTuple)->mustDestroy; }); }

			inline constexpr Entity& getEntity(const TupleType& mTuple) const noexcept { return *std::get<0>(mTuple); }
			template<unsigned int TIdx> inline constexpr auto getComponent(const TupleType& mTuple) const noexcept -> decltype(*std::get<TIdx + 1>(mTuple)) { return *std::get<TIdx + 1>(mTuple); }
			inline const decltype(tuples)& getTuples() const { return tuples; }
	};
}

#endif
