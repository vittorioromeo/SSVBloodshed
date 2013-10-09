// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_SPAWNER
#define SSVOB_COMPONENTS_SPAWNER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCSpawner : public OBCActorBase
	{
		private:
			int type;
			ssvs::Ticker delayStart, delaySpawn;
			int spawnCount;

			inline void spawn()
			{
				switch(type)
				{
					case 0:		getFactory().createERunner(cPhys.getPosI(), false);			break;
					case 1:		getFactory().createERunner(cPhys.getPosI(), true);			break;
					case 2:		getFactory().createECharger(cPhys.getPosI(), false);		break;
					case 3:		getFactory().createECharger(cPhys.getPosI(), true);			break;
					case 4:		getFactory().createEJuggernaut(cPhys.getPosI(), false);		break;
					case 5:		getFactory().createEJuggernaut(cPhys.getPosI(), true);		break;
					case 6:		getFactory().createEGiant(cPhys.getPosI());					break;
					case 7:		getFactory().createEEnforcer(cPhys.getPosI());				break;
					case 8:		getFactory().createEBall(cPhys.getPosI(), false);			break;
					case 9:		getFactory().createEBall(cPhys.getPosI(), true);			break;
				}

				if(--spawnCount <= 0) getEntity().destroy();
			}

		public:
			OBCSpawner(OBCPhys& mCPhys, OBCDraw& mCDraw, int mType, float mDelayStart, float mDelaySpawn, int mSpawnCount) noexcept :
				OBCActorBase{mCPhys, mCDraw}, type{mType}, delayStart{mDelayStart}, delaySpawn{mDelaySpawn}, spawnCount{mSpawnCount} { delayStart.setLoop(false); }

			inline void init() override { getEntity().addGroup(OBGroup::GEnemy); }

			inline void update(float mFT) override
			{
				delayStart.update(mFT);
				if(delayStart.isStopped())
				{
					cDraw[0].rotate(10.f * mFT);
					if(delaySpawn.update(mFT)) spawn();
				}
			}
	};
}

#endif

// TODO: Lambda instead of type?
