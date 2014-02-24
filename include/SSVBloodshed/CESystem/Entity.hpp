// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITY
#define SSVOB_CESYSTEM_ENTITY

#include "SSVBloodshed/CESystem/Common.hpp"

namespace ssvces
{
	class Manager;
	class EntityHandle;
	template<typename, typename, typename> class System;

	class Entity : ssvu::NoCopy
	{
		friend class Manager;
		friend class EntityHandle;
		friend class Internal::SystemBase;
		template<typename, typename, typename> friend class System;

		private:
			Manager& manager;
			std::array<Uptr<Component>, maxComponents> components;
			TypeIdsBitset typeIds;
			bool mustDestroy{false}, mustRematch{true};
			GroupBitset groups;
			EntityStat stat;
			std::size_t componentCount{0};

		public:
			inline Entity(Manager& mManager, const EntityStat& mStat) noexcept : manager(mManager), stat(mStat) { }

			template<typename T, typename... TArgs> inline void createComponent(TArgs&&...);
			template<typename T> inline void removeComponent();
			template<typename T> inline bool hasComponent() const noexcept
			{
				SSVU_ASSERT_STATIC(ssvu::isBaseOf<Component, T>(), "Type must derive from Component");
				return typeIds[Internal::getTypeIdBitIdx<T>()];
			}
			template<typename T> inline T& getComponent() noexcept
			{
				SSVU_ASSERT_STATIC(ssvu::isBaseOf<Component, T>(), "Type must derive from Component");
				SSVU_ASSERT(componentCount > 0 && hasComponent<T>());
				return reinterpret_cast<T&>(*components[Internal::getTypeIdBitIdx<T>()]);
			}

			inline void destroy() noexcept;

			inline Manager& getManager() noexcept { return manager; }

			// Groups
			inline void setGroups(bool mOn, Group mGroup) noexcept;
			inline void addGroups(Group mGroup) noexcept;
			inline void delGroups(Group mGroup) noexcept;
			inline void clearGroups() noexcept;
			template<typename... TGroups> inline void setGroups(bool mOn, Group mGroup, TGroups... mGroups) noexcept { setGroups(mOn, mGroup); setGroups(mOn, mGroups...); }
			template<typename... TGroups> inline void addGroups(Group mGroup, TGroups... mGroups) noexcept { addGroups(mGroup); addGroups(mGroups...); }
			template<typename... TGroups> inline void delGroups(Group mGroup, TGroups... mGroups) noexcept { delGroups(mGroup); delGroups(mGroups...); }
			inline bool hasGroup(Group mGroup) const noexcept					{ return groups[mGroup]; }
			inline bool hasAnyGroup(const GroupBitset& mGroups) const noexcept	{ return (groups & mGroups).any(); }
			inline bool hasAllGroups(const GroupBitset& mGroups) const noexcept	{ return (groups & mGroups).all(); }
			inline const GroupBitset& getGroups() const noexcept				{ return groups; }
	};

	template<typename T> inline constexpr std::tuple<T*> buildComponentsTpl(Entity& mEntity) { return std::tuple<T*>{&mEntity.getComponent<T>()}; }
	template<typename T1, typename T2, typename... TArgs> inline constexpr std::tuple<T1*, T2*, TArgs*...> buildComponentsTpl(Entity& mEntity)
	{
		return std::tuple_cat(buildComponentsTpl<T1>(mEntity), buildComponentsTpl<T2, TArgs...>(mEntity));
	}
}

#endif
