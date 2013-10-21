// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_SPAWNER
#define SSVOB_COMPONENTS_SPAWNER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCIdReceiver.h"

namespace ob
{
	class OBCSpawner : public OBCActorBase
	{
		private:
			OBCIdReceiver& cIdReceiver;
			int type;
			ssvs::Ticker delayStart, delaySpawn;
			int spawnCount;
			bool active{true};

			inline void spawn()
			{
				switch(type)
				{
					case 0:	factory.createERunner(cPhys.getPosI(), false);		break;
					case 1:	factory.createERunner(cPhys.getPosI(), true);		break;
					case 2:	factory.createECharger(cPhys.getPosI(), false);		break;
					case 3:	factory.createECharger(cPhys.getPosI(), true);		break;
					case 4:	factory.createEJuggernaut(cPhys.getPosI(), false);	break;
					case 5:	factory.createEJuggernaut(cPhys.getPosI(), true);	break;
					case 6:	factory.createEGiant(cPhys.getPosI());				break;
					case 7:	factory.createEEnforcer(cPhys.getPosI());			break;
					case 8:	factory.createEBall(cPhys.getPosI(), false);		break;
					case 9:	factory.createEBall(cPhys.getPosI(), true);			break;
				}

				if(--spawnCount <= 0) getEntity().destroy();
			}

		public:
			OBCSpawner(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, int mType, float mDelayStart, float mDelaySpawn, int mSpawnCount) noexcept :
				OBCActorBase{mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), type{mType}, delayStart{mDelayStart}, delaySpawn{mDelaySpawn}, spawnCount{mSpawnCount} { delayStart.setLoop(false); }

			inline void init()
			{
				getEntity().addGroups(OBGroup::GEnemy);
				cIdReceiver.onActivate += [this](IdAction mIdAction)
				{
					switch(mIdAction)
					{
						case IdAction::Toggle: active = !active; break;
						case IdAction::Open: active = true; break;
						case IdAction::Close: active = false; break;
					}
				};
			}

			inline void update(float mFT) override
			{
				if(!active) return;

				delayStart.update(mFT);
				if(!delayStart.isRunning())
				{
					cDraw[0].rotate(10.f * mFT);
					if(delaySpawn.update(mFT)) spawn();
				}
			}

			inline void setActive(bool mActive) noexcept { active = mActive; }
	};
}

#endif

// TODO: Lambda instead of type?
