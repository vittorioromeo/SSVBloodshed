// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_SPAWNER
#define SSVOB_COMPONENTS_SPAWNER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCIdReceiver.hpp"

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
					case 0:		factory.createERunner(cPhys.getPosI(), false);			break;
					case 1:		factory.createERunner(cPhys.getPosI(), true);			break;
					case 2:		factory.createECharger(cPhys.getPosI(), false);			break;
					case 3:		factory.createECharger(cPhys.getPosI(), true);			break;
					case 4:		factory.createECharger(cPhys.getPosI(), true, true);	break;
					case 5:		factory.createEJuggernaut(cPhys.getPosI(), false);		break;
					case 6:		factory.createEJuggernaut(cPhys.getPosI(), true);		break;
					case 7:		factory.createEJuggernaut(cPhys.getPosI(), true, true);	break;
					case 8:		factory.createEGiant(cPhys.getPosI());					break;
					case 9:		factory.createEEnforcer(cPhys.getPosI());				break;
					case 10:	factory.createEBall(cPhys.getPosI(), false);			break;
					case 11:	factory.createEBall(cPhys.getPosI(), true);				break;
				}

				if(--spawnCount <= 0) getEntity().destroy();
			}

		public:
			OBCSpawner(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, int mType, float mDelayStart, float mDelaySpawn, int mSpawnCount) noexcept :
				OBCActorBase{mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), type{mType}, delayStart{mDelayStart}, delaySpawn{mDelaySpawn}, spawnCount{mSpawnCount} { delayStart.setLoop(false); }

			inline void init()
			{
				getEntity().addGroups(OBGroup::GEnemy);
				controlBoolByIdAction(cIdReceiver, active);
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
