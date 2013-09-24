// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_TARGETER
#define SSVOB_COMPONENTS_TARGETER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCFloor.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCKillable.h"

namespace ob
{
	class OBCTargeter : public OBCActorNoDrawBase
	{
		private:
			OBCPhys* target{nullptr};
			OBGroup targetGroup;
			sses::EntityStat targetStat;

		public:
			OBCTargeter(OBCPhys& mCPhys, OBGroup mTargetGroup) : OBCActorNoDrawBase{mCPhys}, targetGroup(mTargetGroup) { }

			inline void update(float) override
			{
				if(game.getManager().hasEntity(targetGroup))
				{
					targetStat = game.getManager().getEntities(targetGroup).front()->getStat();
					target = &game.getManager().getEntities(targetGroup).front()->getComponent<OBCPhys>();
				}

				if(!game.getManager().isAlive(targetStat)) target = nullptr;
			}

			inline bool hasTarget() const noexcept { return target != nullptr && game.getManager().isAlive(targetStat); }
			inline OBCPhys& getTarget() const noexcept { return *target; }
	};
}

#endif

