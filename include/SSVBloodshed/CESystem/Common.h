// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_COMMON
#define SSVOB_CESYSTEM_COMMON

#include <stack>
#include <vector>
#include <tuple>
#include <numeric>
#include <cstdint>
#include <array>
#include <cassert>
#include <unordered_map>
#include <bitset>
#include <SSVUtils/SSVUtils.h>

namespace ssvces
{
	struct Component { inline virtual ~Component() noexcept { } };

	static constexpr std::size_t maxEntities{1000000};
	static constexpr std::size_t maxComponents{32};
	static constexpr std::size_t maxGroups{32};

	using EntityId = std::size_t;
	using EntityIdCtr = std::uint8_t;
	struct EntityStat { EntityId id; EntityIdCtr ctr; };

	using TypeId = std::size_t;
	using TypeIdsBitset = std::bitset<maxComponents>;

	using Group = unsigned int;
	using GroupBitset = std::bitset<maxGroups>;

	template<typename T, typename TDeleter = std::default_delete<T>> using Uptr = ssvu::Uptr<T, TDeleter>;

	namespace Internal
	{
		inline std::size_t getNextTypeId() { static std::size_t lastTypeIdBitIdx{0}; return lastTypeIdBitIdx++; }

		template<typename T> struct TypeIdStorage
		{
			// TypeIdStorage statically stores the TypeId and bit index of a Component type
			static_assert(std::is_base_of<Component, T>::value, "TypeIdStorage only works with types that derive from Component");
			static const std::size_t bitIdx;
		};
		template<typename T> const std::size_t TypeIdStorage<T>::bitIdx{getNextTypeId()};

		// These functions use variadic template recursion to "build" a bitset for a set of Component types
		template<typename T> inline void buildBitsetHelper(TypeIdsBitset& mBitset) noexcept { mBitset[Internal::TypeIdStorage<T>::bitIdx] = true; }
		template<typename T1, typename T2, typename... TArgs> inline void buildBitsetHelper(TypeIdsBitset& mBitset) noexcept { buildBitsetHelper<T1>(mBitset); buildBitsetHelper<T2, TArgs...>(mBitset); }
		template<typename... TArgs> inline TypeIdsBitset getBuildBitset() noexcept { TypeIdsBitset result; buildBitsetHelper<TArgs...>(result); return result; }

		template<typename... TArgs> struct TypeIdsBitsetStorage{ static const TypeIdsBitset bitset; };
		template<typename... TArgs> const TypeIdsBitset TypeIdsBitsetStorage<TArgs...>::bitset{getBuildBitset<TArgs...>()};
		template<> const TypeIdsBitset TypeIdsBitsetStorage<>::bitset{};

		SSVU_DEFINE_HAS_MEMBER_CHECKER(HasAdded, added);
		SSVU_DEFINE_HAS_MEMBER_CHECKER(HasRemoved, removed);

		SSVU_DEFINE_HAS_MEMBER_INVOKER(callAdded, added, (HasAdded<T, void(TArgs...)>::Value));
		SSVU_DEFINE_HAS_MEMBER_INVOKER(callRemoved, removed, (HasRemoved<T, void(TArgs...)>::Value));
	}

	// Shortcut to get the bit index of a Component type
	template<typename T> inline constexpr const std::size_t& getTypeIdBitIdx() noexcept { return Internal::TypeIdStorage<T>::bitIdx; }

	// Shortcut to get the static Bitset of a pack of Component types
	// TODO: BUG: g++ 4.8.2 fails if vvvvvvvvvvvv this constexpr is preserved!!
	template<typename... TArgs> inline constexpr const TypeIdsBitset& getTypeIdsBitsetStorage() noexcept { return Internal::TypeIdsBitsetStorage<TArgs...>::bitset; }

	// Returns whether the first bitset contains all the value of the second one
	inline bool containsAll(const TypeIdsBitset& mA, const TypeIdsBitset& mB) noexcept { return (mA & mB) == mB; }

	class SystemBase;
	inline bool matchesSystem(const TypeIdsBitset& mTypeIds, const SystemBase& mSystem) noexcept;
}

#endif
