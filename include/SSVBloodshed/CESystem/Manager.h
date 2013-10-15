// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_MANAGER
#define SSVOB_CESYSTEM_MANAGER

#include "SSVBloodshed/CESystem/Common.h"
#include "SSVBloodshed/CESystem/Entity.h"
#include "SSVBloodshed/CESystem/EntityHandle.h"
#include "SSVBloodshed/CESystem/IdPool.h"
#include "SSVBloodshed/CESystem/System.h"

namespace ssvces
{
	class EntityStorage
	{
		friend class Manager;

		private:
			std::vector<Entity*> toAdd;
			std::vector<Uptr<Entity>> alive;
			std::array<std::vector<Entity*>, maxGroups> grouped;

			inline Entity* create(Manager& mManager, IdPool& mIdPool) { auto entity(new Entity{mManager, mIdPool.getAvailable()}); toAdd.push_back(entity); return entity; }
	};

	class Manager
	{
		friend class Entity;
		friend class EntityHandle;

		private:
			IdPool entityIdPool;
			std::vector<SystemBase*> systems;
			EntityStorage entities;

			inline void addToGroup(Entity* mEntity, Group mGroup) { assert(mGroup <= maxGroups); entities.grouped[mGroup].push_back(mEntity); }

		public:
			inline void refresh()
			{
				for(auto& s : systems) s->refresh();

				for(auto i(0u); i < maxGroups; ++i) ssvu::eraseRemoveIf(entities.grouped[i], [i](const Entity* mEntity){ return mEntity->mustDestroy || !mEntity->hasGroup(i); });

				auto first(std::begin(entities.alive)), last(std::end(entities.alive)), result(first);
				for(; first != last; ++first)
				{
					auto& e(**first);
					if(e.mustDestroy) continue;

					if(e.mustRematch)
					{
						for(auto& s : systems) if(matchesSystem(e.typeIds, *s)) s->registerEntity(e);
						e.mustRematch = false;
					}

					*result++ = std::move(*first);
				}
				entities.alive.erase(result, last);

				for(auto& e : entities.toAdd)
				{
					for(auto& s : systems) if(matchesSystem(e->typeIds, *s)) s->registerEntity(*e);
					e->mustRematch = false;
					entities.alive.emplace_back(e);
				}

				entities.toAdd.clear();
			}

			inline EntityHandle createEntity() { return {*entities.create(*this, entityIdPool)}; }
			template<typename T> inline void registerSystem(T& mSystem)
			{
				static_assert(std::is_base_of<SystemBase, T>::value, "Type must derive from SystemBase");
				systems.push_back(&mSystem);
			}

			inline const decltype(entities.alive)& getEntities() const noexcept			{ return entities.alive; }
			inline decltype(entities.alive)& getEntities() noexcept						{ return entities.alive; }
			inline const std::vector<Entity*>& getEntities(Group mGroup) const noexcept { assert(mGroup <= maxGroups); return entities.grouped[mGroup]; }
			inline std::vector<Entity*>& getEntities(Group mGroup) noexcept				{ assert(mGroup <= maxGroups); return entities.grouped[mGroup]; }
			inline std::vector<EntityHandle> getEntityHandles(Group mGroup) noexcept
			{
				std::vector<EntityHandle> result;
				for(const auto& e : getEntities(mGroup)) result.emplace_back(*e);
				return result;
			}

			inline bool hasEntity(Group mGroup) const noexcept				{ return !getEntities(mGroup).empty(); }
			inline std::size_t getEntityCount() const noexcept				{ return entities.alive.size(); }
			inline std::size_t getEntityCount(Group mGroup) const noexcept	{ return getEntities(mGroup).size(); }

			// Have a manager.componentCount too?
			inline std::size_t getComponentCount() const noexcept			{ std::size_t result{0}; for(auto& e : getEntities()) result += e->componentCount; return result; }
	};

	// to .inl
	template<typename T, typename... TArgs> inline void Entity::createComponent(TArgs&&... mArgs)
	{
		static_assert(std::is_base_of<Component, T>::value, "Type must derive from Component");
		assert(!hasComponent<T>() && componentCount <= maxComponents);

		components[getTypeIdBitIdx<T>()] = std::make_unique<T>(std::forward<TArgs>(mArgs)...);
		typeIds[getTypeIdBitIdx<T>()] = true;
		++componentCount;

		mustRematch = true;
	}
	template<typename T> inline void Entity::removeComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "Type must derive from Component");
		assert(hasComponent<T>() && componentCount > 0);

		components[getTypeIdBitIdx<T>()].reset();
		typeIds[getTypeIdBitIdx<T>()] = false;
		--componentCount;

		mustRematch = true;
	}
	inline void Entity::destroy() noexcept					{ mustDestroy = true; manager.entityIdPool.reclaim(stat); }
	inline void Entity::addGroups(Group mGroup) noexcept	{ groups[mGroup] = true; manager.addToGroup(this, mGroup); }
	inline void Entity::delGroups(Group mGroup) noexcept	{ groups[mGroup] = false; }
	inline void Entity::clearGroups() noexcept				{ groups.reset(); }
	inline bool EntityHandle::isAlive() const noexcept		{ return manager.entityIdPool.isAlive(stat); }
	inline bool matchesSystem(const TypeIdsBitset& mTypeIds, const SystemBase& mSystem) noexcept
	{
		return (mTypeIds & mSystem.typeIdsNot).none() && containsAll(mTypeIds, mSystem.typeIdsReq);
	}
}

#endif
