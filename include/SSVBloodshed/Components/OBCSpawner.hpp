// Copyright (c) 2013-2014 Vittorio Romeo
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
			SpawnerItem type;
			ssvs::Ticker delayStart, delaySpawn;
			int spawnCount;
			bool active{true};

			inline void spawn()
			{
				static void(*fnPtrs[])(OBFactory&, const Vec2i&)
				{
					[](OBFactory& mF, const Vec2i& mPos){ mF.createERunner(mPos, RunnerType::Unarmed); },					// 0
					[](OBFactory& mF, const Vec2i& mPos){ mF.createERunner(mPos, RunnerType::PlasmaBolter); },				// 1
					[](OBFactory& mF, const Vec2i& mPos){ mF.createECharger(mPos, ChargerType::Unarmed); },					// 2
					[](OBFactory& mF, const Vec2i& mPos){ mF.createECharger(mPos, ChargerType::PlasmaBolter); },			// 3
					[](OBFactory& mF, const Vec2i& mPos){ mF.createECharger(mPos, ChargerType::GrenadeLauncher); },			// 4
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEJuggernaut(mPos, JuggernautType::Unarmed); },			// 5
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEJuggernaut(mPos, JuggernautType::PlasmaBolter); },		// 6
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEJuggernaut(mPos, JuggernautType::RocketLauncher); },	// 7
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEGiant(mPos); },											// 8
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEEnforcer(mPos); },										// 9
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEBall(mPos, BallType::Normal, false); },					// 10
					[](OBFactory& mF, const Vec2i& mPos){ mF.createEBall(mPos, BallType::Flying, false); }					// 11
				};

				fnPtrs[int(type)](factory, cPhys.getPosI());
				if(--spawnCount <= 0) getEntity().destroy();
			}

		public:
			OBCSpawner(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, SpawnerItem mType, float mDelayStart, float mDelaySpawn, int mSpawnCount) noexcept :
				OBCActorBase{mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), type{mType}, delayStart{mDelayStart}, delaySpawn{mDelaySpawn}, spawnCount{mSpawnCount} { delayStart.setLoop(false); }

			inline void init()
			{
				getEntity().addGroups(OBGroup::GEnemy);
				controlBoolByIdAction(cIdReceiver, active);
			}

			inline void update(FT mFT) override
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
