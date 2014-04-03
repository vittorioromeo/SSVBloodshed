// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_COMMON
#define SSVOB_CESYSTEM_COMMON

#include <vector>
#include <tuple>
#include <numeric>
#include <cstdint>
#include <array>
#include <cassert>
#include <unordered_map>
#include <bitset>
#include <SSVUtils/SSVUtils.hpp>

namespace ssvces
{
	struct Component { inline virtual ~Component() noexcept { } };

	static constexpr std::size_t maxEntities{1000000};
	static constexpr std::size_t maxComponents{32};
	static constexpr std::size_t maxGroups{32};

	using EntityId = int;
	using EntityIdCtr = int;
	struct EntityStat { EntityId id; EntityIdCtr ctr; };

	using TypeIdIdx = std::size_t;
	using TypeIdsBitset = std::bitset<maxComponents>;

	using Group = std::size_t;
	using GroupBitset = std::bitset<maxGroups>;

	template<typename T, typename TDeleter = std::default_delete<T>> using Uptr = ssvu::Uptr<T, TDeleter>;

	namespace Internal
	{
		class SystemBase;

		// Returns the next unique bit index for a type
		inline TypeIdIdx getNextTypeIdBitIdx() noexcept { static TypeIdIdx lastIdx{0}; return lastIdx++; }

		// Shortcut to get the bit index of a Component type
		template<typename T> inline const TypeIdIdx& getTypeIdBitIdx() noexcept
		{
			SSVU_ASSERT_STATIC(ssvu::isBaseOf<Component, T>(), "Type must derive from Component");
			static TypeIdIdx idx{getNextTypeIdBitIdx()}; return idx;
		}

		// These functions use variadic template recursion to "build" a bitset for a set of Component types
		template<typename T> inline void buildBitsetHelper(TypeIdsBitset& mBitset) noexcept { mBitset[getTypeIdBitIdx<T>()] = true; }
		template<typename T1, typename T2, typename... TArgs> inline void buildBitsetHelper(TypeIdsBitset& mBitset) noexcept { buildBitsetHelper<T1>(mBitset); buildBitsetHelper<T2, TArgs...>(mBitset); }
		template<typename... TArgs> inline TypeIdsBitset getBuildBitset() noexcept { TypeIdsBitset result; buildBitsetHelper<TArgs...>(result); return result; }
		template<> inline TypeIdsBitset getBuildBitset<>() noexcept { static TypeIdsBitset nullBitset; return nullBitset; }

		SSVU_DEFINE_MEMFN_CALLER(callAdded, added, void(TArgs...));		// `callAdded(...)` only calls `T::added` if it exists
		SSVU_DEFINE_MEMFN_CALLER(callRemoved, removed, void(TArgs...));	// `callRemoved(...)` only calls `T::removed` if it exists

		// Shortcut to get the static Bitset of a pack of Component types
		template<typename... TArgs> inline const TypeIdsBitset& getTypeIdsBitset() noexcept { static TypeIdsBitset bitset{Internal::getBuildBitset<TArgs...>()}; return bitset; }

		// Returns whether the first bitset contains all the value of the second one
		inline bool containsAll(const TypeIdsBitset& mA, const TypeIdsBitset& mB) noexcept { return (mA & mB) == mB; }

		// Returns whether a type id bitset matches a system's type id bitset
		inline bool matchesSystem(const TypeIdsBitset& mTypeIds, const SystemBase& mSystem) noexcept;
	}
}

#endif
