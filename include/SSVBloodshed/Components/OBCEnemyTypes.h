// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMYTYPES
#define SSVOB_COMPONENTS_ENEMYTYPES

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCFloor.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCEnemy.h"

namespace ob
{
	class OBCEBase : public OBCActorBase
	{
		private:
			bool breakFloor{false};

		protected:
			OBCEnemy& cEnemy;
			OBCHealth& cHealth;

		public:
			OBCEBase(OBCEnemy& mCEnemy) : OBCActorBase{mCEnemy.getCPhys(), mCEnemy.getCDraw()}, cEnemy(mCEnemy), cHealth(cEnemy.getCHealth())
			{
				body.addGroupNoResolve(OBGroup::GFloor);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(breakFloor && mDI.body.hasGroup(OBGroup::GFloor)) static_cast<Entity*>(mDI.body.getUserData())->getComponent<OBCFloor>().smash();
				};
			}

			inline void setBreakFloor(bool mValue) noexcept
			{
				breakFloor = mValue;

				if(mValue) body.addGroupToCheck(OBGroup::GFloor);
				else body.delGroupToCheck(OBGroup::GFloor);
			}
	};

	class OBCECharger : public OBCEBase
	{
		private:
			ssvs::Ticker timerCharge{250.f};
			ssvu::Timeline tlCharge{false};
			float lastDeg{0};

		public:
			OBCECharger(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cEnemy.setWalkSpeed(20.f);
				cEnemy.setTurnSpeed(3.f);
				cEnemy.setGibMult(2);
				cEnemy.setMaxVelocity(40.f);

				tlCharge.append<ssvu::Do>([this]{ body.setVelocity(body.getVelocity() * 0.8f); });
				tlCharge.append<ssvu::Wait>(2.5f);
				tlCharge.append<ssvu::Go>(0, 10);
				tlCharge.append<ssvu::Do>([this]{ setBreakFloor(true); lastDeg = cEnemy.getCurrentDegrees(); body.applyForce(ssvs::getVecFromDegrees(lastDeg, 1250.f)); });
				tlCharge.append<ssvu::Wait>(10.f);
				tlCharge.append<ssvu::Do>([this]{ body.applyForce(ssvs::getVecFromDegrees(lastDeg, -150.f)); });
				tlCharge.append<ssvu::Wait>(9.f);
				tlCharge.append<ssvu::Do>([this]{ setBreakFloor(false); cEnemy.setWalkSpeed(20.f); });
			}
			inline void update(float mFrameTime) override
			{
				tlCharge.update(mFrameTime);
				if(timerCharge.update(mFrameTime)) { tlCharge.reset(); tlCharge.start(); }
			}
	};

	class OBCEJuggernaut : public OBCEBase
	{
		private:
			ssvs::Ticker timerShoot{150.f};
			ssvu::Timeline tlShoot{false};
			float lastDeg{0};

		public:
			OBCEJuggernaut(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cEnemy.setWalkSpeed(10.f);
				cEnemy.setTurnSpeed(2.5f);
				cEnemy.setGibMult(4);

				tlShoot.append<ssvu::Do>([this]{ shoot(ssvu::getRnd(-10, 10)); });
				tlShoot.append<ssvu::Wait>(1.1f);
				tlShoot.append<ssvu::Go>(0, 8);
				tlShoot.append<ssvu::Wait>(15.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-100.f); });
				tlShoot.append<ssvu::Do>([this]{ shoot(lastDeg); lastDeg += 265; });
				tlShoot.append<ssvu::Wait>(0.3f);
				tlShoot.append<ssvu::Go>(5, 45);
				tlShoot.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });
			}
			inline void update(float mFrameTime) override
			{
				tlShoot.update(mFrameTime);
				if(timerShoot.update(mFrameTime)) { tlShoot.reset(); tlShoot.start(); }
			}
			inline void shoot(int mDeg)
			{
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(cEnemy.getCurrentDegrees()) * 100.f)};
				getFactory().createProjectileEnemyBullet(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};

	class OBCEGiant : public OBCEBase
	{
		private:
			ssvs::Ticker timerShoot{185.f};
			ssvu::Timeline tlShoot{false};
			ssvu::Timeline tlSummon{false};
			float lastDeg{0};

		public:
			OBCEGiant(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cEnemy.setWalkSpeed(10.f);
				cEnemy.setTurnSpeed(2.5f);
				cEnemy.setGibMult(8);

				setBreakFloor(true);
				tlShoot.append<ssvu::Do>([this]{ shoot(ssvu::getRnd(-15, 15)); });
				tlShoot.append<ssvu::Wait>(0.4f);
				tlShoot.append<ssvu::Go>(0, 20);
				tlShoot.append<ssvu::Wait>(19.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-50.f); });
				tlShoot.append<ssvu::Do>([this]{ shoot(lastDeg); lastDeg += 235; });
				tlShoot.append<ssvu::Wait>(0.1f);
				tlShoot.append<ssvu::Go>(5, 150);
				tlShoot.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });

				tlSummon.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(0.f); });
				tlSummon.append<ssvu::Do>([this]
				{
					body.setVelocity(body.getVelocity() * 0.8f);
					getFactory().createTestEnemy(body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(lastDeg) * 1000.f));
					lastDeg += 360 / 16;
				});
				tlSummon.append<ssvu::Wait>(4.5f);
				tlSummon.append<ssvu::Go>(1, 16);
				tlSummon.append<ssvu::Wait>(19.f);
				tlSummon.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(10.f); });
			}
			inline void update(float mFrameTime) override
			{
				tlShoot.update(mFrameTime);
				tlSummon.update(mFrameTime);
				if(timerShoot.update(mFrameTime)) { tlShoot.reset(); tlShoot.start(); }
			}
			inline void shoot(int mDeg)
			{
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(cEnemy.getCurrentDegrees()) * 100.f)};
				getFactory().createProjectileEnemyBullet(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};
}

#endif
