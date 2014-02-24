// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITY_INL
#define SSVOB_CESYSTEM_ENTITY_INL

namespace ssvces
{
	template<typename T, typename... TArgs> inline void Entity::createComponent(TArgs&&... mArgs)
	{
		SSVU_ASSERT_STATIC(ssvu::isBaseOf<Component, T>(), "Type must derive from Component");
		SSVU_ASSERT(!hasComponent<T>() && componentCount <= maxComponents);

		components[Internal::getTypeIdBitIdx<T>()] = std::make_unique<T>(std::forward<TArgs>(mArgs)...);
		typeIds[Internal::getTypeIdBitIdx<T>()] = true;
		++componentCount;

		mustRematch = true;
	}
	template<typename T> inline void Entity::removeComponent()
	{
		SSVU_ASSERT_STATIC(ssvu::isBaseOf<Component, T>(), "Type must derive from Component");
		SSVU_ASSERT(hasComponent<T>() && componentCount > 0);

		components[Internal::getTypeIdBitIdx<T>()].reset();
		typeIds[Internal::getTypeIdBitIdx<T>()] = false;
		--componentCount;

		mustRematch = true;
	}
	inline void Entity::destroy() noexcept							{ mustDestroy = true; manager.entityIdPool.reclaim(stat); }
	inline void Entity::setGroups(bool mOn, Group mGroup) noexcept	{ groups[mGroup] = mOn; if(mOn) manager.addToGroup(this, mGroup); }
	inline void Entity::addGroups(Group mGroup) noexcept			{ groups[mGroup] = true; manager.addToGroup(this, mGroup); }
	inline void Entity::delGroups(Group mGroup) noexcept			{ groups[mGroup] = false; }
	inline void Entity::clearGroups() noexcept						{ groups.reset(); }
}

#endif
