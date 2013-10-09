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
	class Manager
	{
		friend class Entity;

		private:
			IdPool entityIdPool;
			std::vector<Entity*> entitiesToAdd;
			std::vector<SystemBase*> systems;
			std::vector<ssvu::Uptr<Entity>> entities;

		public:
			inline void refresh()
			{
				for(auto& s : systems) s->refresh();
				ssvu::eraseRemoveIf(entities, [](const ssvu::Uptr<Entity>& mEntity){ return mEntity->mustDestroy; });

				for(auto& e : entitiesToAdd)
				{
					for(auto i(0u); i < systems.size(); ++i)
					{
						auto& s(systems[i]);
						if((e->typeIdBitset & s->typeIdBitset) == s->typeIdBitset) s->registerEntity(*e);
						e->systems.push_back(s);
					}
					entities.emplace_back(e);
				}

				entitiesToAdd.clear();
			}

			inline EntityHandle createEntity()
			{
				auto entity(new Entity{*this});
				entitiesToAdd.push_back(entity);
				return {*this, entityIdPool, *entity, entityIdPool.getAvailableId()};
			}

			template<typename T> void registerSystem(T& mSystem) { systems.push_back(&mSystem);  }
	};

}

#endif
