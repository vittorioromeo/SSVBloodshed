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
			std::vector<Id> available;
			std::array<Ctr, maxEntities> counters;

		public:
			inline IdPool() : available(maxEntities)
			{
				// Fill the available ids vector from 0 to maxEntities
				std::iota(std::begin(available), std::end(available), 0);
			}

			// Returns the first unused id
			inline Id getAvailableId() noexcept	{ Id result(available.back()); available.pop_back(); return result; }

			// Used on Entity death, reclaims the Entity's id so that it can be reused
			inline void reclaim(const Id& mId, const Ctr& mCtr) noexcept { if(mCtr != counters[mId]) return; ++counters[mId]; available.emplace_back(mId); }

			// Checks if an Entity is currently alive
			inline bool isAlive(const Id& mId, const Ctr& mCtr) const noexcept { return counters[mId] == mCtr; }
	};
}

#endif
