// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITYHANDLE
#define SSVOB_CESYSTEM_ENTITYHANDLE

#include "SSVBloodshed/CESystem/Common.h"
#include "SSVBloodshed/CESystem/Entity.h"

namespace ssvces
{
	class Manager;

	class EntityHandle
	{
		private:
			Manager& manager;
			Entity& entity;
			EntityStat stat;

		public:
			inline EntityHandle(Manager& mManager, Entity& mEntity) noexcept : manager(mManager), entity(mEntity), stat(entity.stat) { }

			template<typename T, typename... TArgs> inline void createComponent(TArgs&&... mArgs)	{ assert(isAlive()); entity.createComponent<T>(std::forward<TArgs>(mArgs)...); }
			template<typename T> inline bool hasComponent() const noexcept							{ assert(isAlive()); return entity.hasComponent<T>(); }
			template<typename T> inline T& getComponent()											{ assert(isAlive()); return entity.getComponent<T>(); }

			inline void destroy() noexcept { entity.destroy(); assert(!isAlive()); }

			inline bool isAlive() const noexcept;
			inline Manager& getManager() noexcept	{ return manager; }
			inline Entity& getEntity() noexcept		{ assert(isAlive()); return entity; }

			// Groups
			template<typename... TGroups> inline void addGroups(TGroups... mGroups) noexcept	{ entity.addGroups(mGroups...); }
			template<typename... TGroups> inline void delGroups(TGroups... mGroups) noexcept	{ entity.delGroups(mGroups...); }
			inline bool hasGroup(Group mGroup) const noexcept									{ return entity.hasGroup(mGroup); }
			inline bool hasAnyGroup(const GroupBitset& mGroups) const noexcept					{ return entity.hasAnyGroup(mGroups); }
			inline void clearGroups() noexcept													{ return entity.clearGroups(); }
			inline const GroupBitset& getGroups() const noexcept								{ return entity.getGroups(); }
	};
}

#endif
