// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_IDPOOL
#define SSVOB_CESYSTEM_IDPOOL

#include "SSVBloodshed/CESystem/Common.h"

namespace ssvces
{
	class IdPool
	{
		// IdPool stores available Entity ids and is used to check Entity validity

		private:
			std::vector<EntityId> available;
			std::array<EntityIdCtr, maxEntities> counters;

		public:
			inline IdPool() : available(maxEntities)
			{
				// Fill the available ids vector from 0 to maxEntities
				std::iota(std::begin(available), std::end(available), 0);
			}

			// Returns the first available IdCtrPair
			inline EntityIdCtrPair getAvailableIdCtrPair() noexcept { EntityId id(available.back()); available.pop_back(); return {id, counters[id]}; }

			// Used on Entity death, reclaims the Entity's id so that it can be reused
			inline void reclaim(const EntityIdCtrPair& mIdCtrPair) noexcept { if(mIdCtrPair.second != counters[mIdCtrPair.first]) return; ++counters[mIdCtrPair.first]; available.emplace_back(mIdCtrPair.first); }

			// Checks if an Entity is currently alive
			inline bool isAlive(const EntityIdCtrPair& mIdCtrPair) const noexcept { return counters[mIdCtrPair.first] == mIdCtrPair.second; }
	};
}

#endif
