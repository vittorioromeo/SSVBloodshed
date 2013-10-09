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
			IdPool& idPool;
			Entity& entity;
			Id id;
			Ctr counter;

		public:
			inline EntityHandle(Manager& mManager, IdPool& mIdPool, Entity& mEntity, const Id& mId) noexcept : manager(mManager), idPool(mIdPool), entity(mEntity), id{mId} { }

			template<typename T, typename... TArgs> inline void createComponent(TArgs&&... mArgs)	{ assert(isAlive()); entity.createComponent<T>(std::forward<TArgs>(mArgs)...); }
			template<typename T> inline bool hasComponent() const noexcept							{ assert(isAlive()); return entity.hasComponent<T>(); }
			template<typename T> inline T& getComponent()											{ assert(isAlive()); return entity.getComponent<T>(); }

			inline void destroy() noexcept
			{
				entity.destroy();
				idPool.reclaim(id, counter);
				assert(!isAlive());
			}

			inline bool isAlive() const noexcept		{ return idPool.isAlive(id, counter); }
			inline Manager& getManager() noexcept		{ return manager; }
			inline Entity& getEntity() noexcept			{ assert(isAlive()); return entity; }
			inline const Id& getId() const noexcept		{ return id; }
			inline const Ctr& getCtr() const noexcept	{ return counter; }
	};
}

#endif
