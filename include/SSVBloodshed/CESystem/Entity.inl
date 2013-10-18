// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITY_INL
#define SSVOB_CESYSTEM_ENTITY_INL

namespace ssvces
{
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
}

#endif
