// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITYHANDLE
#define SSVOB_CESYSTEM_ENTITYHANDLE

#include "SSVBloodshed/CESystem/Common.hpp"
#include "SSVBloodshed/CESystem/Entity.hpp"

namespace ssvces
{
	class Manager;

	class EntityHandle
	{
		private:
			Entity& entity;
			Manager& manager;
			EntityStat stat;

		public:
			inline EntityHandle(Entity& mEntity) noexcept : entity(mEntity), manager(entity.getManager()), stat(entity.stat) { }

			template<typename T, typename... TArgs> inline void createComponent(TArgs&&... mArgs)	{ SSVU_ASSERT(isAlive()); entity.createComponent<T>(std::forward<TArgs>(mArgs)...); }
			template<typename T> inline bool hasComponent() const noexcept							{ SSVU_ASSERT(isAlive()); return entity.hasComponent<T>(); }
			template<typename T> inline T& getComponent()											{ SSVU_ASSERT(isAlive()); return entity.getComponent<T>(); }

			inline void destroy() noexcept { if(isAlive()) entity.destroy(); }

			inline bool isAlive() const noexcept;
			inline Manager& getManager() noexcept	{ return manager; }
			inline Entity& getEntity() noexcept		{ SSVU_ASSERT(isAlive()); return entity; }

			// Groups
			template<typename... TGroups> inline void addGroups(TGroups... mGroups) noexcept	{ SSVU_ASSERT(isAlive()); entity.addGroups(mGroups...); }
			template<typename... TGroups> inline void delGroups(TGroups... mGroups) noexcept	{ SSVU_ASSERT(isAlive()); entity.delGroups(mGroups...); }
			inline bool hasGroup(Group mGroup) const noexcept									{ SSVU_ASSERT(isAlive()); return entity.hasGroup(mGroup); }
			inline bool hasAnyGroup(const GroupBitset& mGroups) const noexcept					{ SSVU_ASSERT(isAlive()); return entity.hasAnyGroup(mGroups); }
			inline void clearGroups() noexcept													{ SSVU_ASSERT(isAlive()); return entity.clearGroups(); }
			inline const GroupBitset& getGroups() const noexcept								{ SSVU_ASSERT(isAlive()); return entity.getGroups(); }
	};
}

#endif
