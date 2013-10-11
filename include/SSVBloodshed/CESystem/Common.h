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
	class Component { };

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
		// Last used bit index
		static unsigned int lastTypeIdBitIdx{0};

		template<typename T> struct TypeIdStorage
		{
			// TypeIdStorage statically stores the TypeId and bit index of a Component type

			static_assert(std::is_base_of<Component, T>::value, "TypeIdStorage only works with types that derive from Component");

			static const TypeId typeId;
			static const std::size_t bitIdx;
		};
		template<typename T> const TypeId TypeIdStorage<T>::typeId{typeid(T).hash_code()};
		template<typename T> const std::size_t TypeIdStorage<T>::bitIdx{lastTypeIdBitIdx++};

		// These functions use variadic template recursion to "build" a bitset for a set of Component types
		template<typename T> inline static void buildBitsetHelper(TypeIdsBitset& mBitset) noexcept { mBitset[Internal::TypeIdStorage<T>::bitIdx] = true; }
		template<typename T1, typename T2, typename... TArgs> inline static void buildBitsetHelper(TypeIdsBitset& mBitset) noexcept { buildBitsetHelper<T1>(mBitset); buildBitsetHelper<T2, TArgs...>(mBitset); }
		template<typename... TArgs> inline static TypeIdsBitset getBuildBitset() noexcept { TypeIdsBitset result; buildBitsetHelper<TArgs...>(result); return result; }

		template<typename... TArgs> struct TypeIdsBitsetStorage{ static const TypeIdsBitset bitset; };
		template<typename... TArgs> const TypeIdsBitset TypeIdsBitsetStorage<TArgs...>::bitset{getBuildBitset<TArgs...>()};
		template<> const TypeIdsBitset TypeIdsBitsetStorage<>::bitset{};
	}

	// Shortcut to get the static TypeId of a Component type from TypeIdStorage
	template<typename T> inline constexpr const TypeId& getTypeId() noexcept { return Internal::TypeIdStorage<T>::typeId; }

	// Shortcut to get the bit index of a Component type
	template<typename T> inline constexpr static const std::size_t& getTypeIdBitIdx() noexcept { return Internal::TypeIdStorage<T>::bitIdx; }

	// Shortcut to get the static Bitset of a pack of Component types
	template<typename... TArgs> inline constexpr static const TypeIdsBitset& getTypeIdsBitsetStorage() noexcept { return Internal::TypeIdsBitsetStorage<TArgs...>::bitset; }

	// Returns whether the first bitset contains all the value of the second one
	inline static bool containsAll(const TypeIdsBitset& mA, const TypeIdsBitset& mB) noexcept { return (mA & mB) == mB; }

	class SystemBase;
	inline static bool matchesSystem(const TypeIdsBitset& mTypeIds, const SystemBase& mSystem) noexcept;
}

#endif
