// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITY
#define SSVOB_CESYSTEM_ENTITY

#include "SSVBloodshed/CESystem/Common.h"

namespace ssvces
{
	class Manager;
	class EntityHandle;
	class SystemBase;
	template<typename...> class System;

	class Entity
	{
		friend class Manager;
		friend class EntityHandle;
		template<typename...> friend class System;

		private:
			Manager& manager;
			std::vector<ssvu::Uptr<Component>> components;
			std::vector<Component*> componentPtrs;
			Bitset typeIdBitset;
			std::vector<SystemBase*> systems;
			bool mustDestroy{false};

		public:
			inline Entity(Manager& mManager) noexcept : manager(mManager), componentPtrs(maxComponentsPerEntity) { }

			template<typename T, typename... TArgs> inline void createComponent(TArgs&&... mArgs)
			{
				assert(!hasComponent<T>());
				auto component(new T(std::forward<TArgs>(mArgs)...));
				componentPtrs[getTypeIdBitIdx<T>()] = component;
				components.emplace_back(component);
				appendTypeIdBit<T>(typeIdBitset);
			}
			template<typename T> inline bool hasComponent() const noexcept	{ return typeIdBitset[getTypeIdBitIdx<T>()]; }
			template<typename T> inline T& getComponent()					{ assert(hasComponent<T>()); return *reinterpret_cast<T*>(componentPtrs[getTypeIdBitIdx<T>()]); }

			inline void destroy() noexcept { mustDestroy = true; }

			inline Manager& getManager() noexcept { return manager; }
	};

	template<typename T> constexpr inline static std::tuple<T*> buildComponentsTuple(Entity& mEntity) { return std::tuple<T*>{&mEntity.getComponent<T>()}; }
	template<typename T1, typename T2, typename... TArgs> constexpr inline static std::tuple<T1*, T2*, TArgs*...> buildComponentsTuple(Entity& mEntity) { return std::tuple_cat(buildComponentsTuple<T1>(mEntity), buildComponentsTuple<T2, TArgs...>(mEntity)); }
}

#endif
