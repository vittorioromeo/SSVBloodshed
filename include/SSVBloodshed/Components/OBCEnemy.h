// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMY
#define SSVOB_COMPONENTS_ENEMY

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCFloor.h"

namespace ob
{
	class OBCEnemy : public sses::Component
	{
		private:
			OBGame& game;
			OBCPhys& cPhys;
			OBCDraw& cDraw;
			OBCHealth& cHealth;
			OBAssets& assets;
			ssvsc::Body& body;
			float walkSpeed{100.f}, currentDegrees{0.f}, turnSpeed{7.5f};
			float snappedDegrees{0.f}, maxVelocity{120.f};
			int gibMult{1};

		public:
			OBCEnemy(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCHealth& mCHealth) : game(mCDraw.getGame()), cPhys(mCPhys), cDraw(mCDraw), cHealth(mCHealth), assets(game.getAssets()), body(cPhys.getBody()) { }

			inline void init() override
			{
				cHealth.onDamage += [this]
				{
					game.createPBlood(6 * gibMult, toPixels(body.getPosition()));
					if(cHealth.isDead())
					{
						game.createPBlood(20 * gibMult * gibMult, toPixels(body.getPosition()), gibMult);
						game.createPGib(35 * gibMult * gibMult, toPixels(body.getPosition()));
						getEntity().destroy();
					}
				};

				getEntity().addGroup(OBGroup::OBGEnemy);
				body.addGroup(OBGroup::OBGSolid);
				body.addGroup(OBGroup::OBGEnemy);
				body.addGroup(OBGroup::OBGOrganic);
				body.addGroupToCheck(OBGroup::OBGSolid);
				body.setRestitutionX(0.9f);
				body.setRestitutionY(0.9f);
				//body.onPreUpdate += [this]{ body.setVelocity(ssvs::getMClamped(body.getVelocity(), -120.f, 120.f)); };
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::OBGFriendly))
					{
						auto& e(*static_cast<Entity*>(mDI.body.getUserData()));
						e.getComponent<OBCHealth>().damage(1);
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				for(const auto& e : game.getManager().getEntities(OBGroup::OBGPlayer))
				{
					auto& ecPhys(e->getComponent<OBCPhys>());
					float targetDegrees(ssvs::getDegreesTowards(Vec2f(body.getPosition()), Vec2f(ecPhys.getBody().getPosition())));

					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;
				}

				if(ssvs::getMagnitude(body.getVelocity()) < maxVelocity) body.applyForce(ssvs::getVecFromDegrees(snappedDegrees, walkSpeed) * 0.05f);
				else body.setVelocity(body.getVelocity() * 0.99f);
			}

			inline void draw() override
			{
				auto& s0(cDraw[0]);
				//s0.setTextureRect(assets.get<Tileset>("tileset")OBGEnemy[{0, 0}]);
				s0.setRotation(snappedDegrees);
			}

			inline void setWalkSpeed(float mValue) noexcept	{ walkSpeed = mValue; }
			inline void setTurnSpeed(float mValue) noexcept	{ turnSpeed = mValue; }
			inline void setGibMult(int mValue) noexcept		{ gibMult = mValue; }
			inline void setMaxVelocity(float mMax) noexcept	{ maxVelocity = mMax; }

			inline OBGame& getGame() const noexcept			{ return game; }
			inline OBCPhys& getCPhys() const noexcept		{ return cPhys; }
			inline OBCDraw& getCDraw() const noexcept		{ return cDraw; }
			inline OBCHealth& getCHealth() const noexcept	{ return cHealth; }
			inline float getCurrentDegrees() const noexcept	{ return currentDegrees; }
	};

	class OBCEBase
	{
		private:
			bool breakFloor{false};

		protected:
			OBCEnemy& cEnemy;
			OBGame& game;
			OBCPhys& cPhys;
			OBCDraw& cDraw;
			OBCHealth& cHealth;
			OBAssets& assets;
			ssvsc::Body& body;

		public:
			OBCEBase(OBCEnemy& mCEnemy) : cEnemy(mCEnemy), game(cEnemy.getGame()), cPhys(cEnemy.getCPhys()), cDraw(cEnemy.getCDraw()), cHealth(cEnemy.getCHealth()), assets(game.getAssets()), body(cPhys.getBody())
			{
				body.addGroupNoResolve(OBGroup::OBGFloor);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(breakFloor && mDI.body.hasGroup(OBGroup::OBGFloor))
						static_cast<Entity*>(mDI.body.getUserData())->getComponent<OBCFloor>().smash();
				};
			}

			inline void setBreakFloor(bool mValue) noexcept
			{
				breakFloor = mValue;

				if(mValue) body.addGroupToCheck(OBGroup::OBGFloor);
				else body.delGroupToCheck(OBGroup::OBGFloor);
			}
	};

	class OBCECharger : public OBCEBase, public sses::Component
	{
		private:
			ssvs::Ticker timerCharge{250.f};
			ssvu::Timeline tlCharge{false};
			float lastDeg{0};

		public:
			OBCECharger(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
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

	class OBCEJuggernaut : public OBCEBase, public sses::Component
	{
		private:
			ssvs::Ticker timerShoot{150.f};
			ssvu::Timeline tlShoot{false};
			float lastDeg{0};

		public:
			OBCEJuggernaut(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
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
				game.getFactory().createProjectileEnemyBullet(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};

	class OBCEGiant : public OBCEBase, public sses::Component
	{
		private:
			ssvs::Ticker timerShoot{185.f};
			ssvu::Timeline tlShoot{false};
			float lastDeg{0};

		public:
			OBCEGiant(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
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
			}
			inline void update(float mFrameTime) override
			{
				tlShoot.update(mFrameTime);
				if(timerShoot.update(mFrameTime)) { tlShoot.reset(); tlShoot.start(); }
			}
			inline void shoot(int mDeg)
			{
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(cEnemy.getCurrentDegrees()) * 100.f)};
				game.getFactory().createProjectileEnemyBullet(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};
}

#endif
