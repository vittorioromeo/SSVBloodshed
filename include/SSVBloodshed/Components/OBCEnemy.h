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
			float snappedDegrees{0.f};
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

				getEntity().addGroup(OBGroup::Enemy);
				body.addGroup(OBGroup::Solid);
				body.addGroup(OBGroup::Enemy);
				body.addGroup(OBGroup::Organic);
				body.addGroupToCheck(OBGroup::Solid);
				body.setRestitutionX(1.7f);
				body.setRestitutionY(1.7f);
				body.onPreUpdate += [this]{ body.setVelocity(ssvs::getMClamped(body.getVelocity(), -120.f, 120.f)); };
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::Friendly))
					{
						auto& e(*static_cast<Entity*>(mDI.body.getUserData()));
						e.getComponent<OBCHealth>().damage(1);
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				for(const auto& e : game.getManager().getEntities(OBGroup::Player))
				{
					auto& ecPhys(e->getComponent<OBCPhys>());
					float targetDegrees(ssvs::getDegreesTowards(Vec2f(body.getPosition()), Vec2f(ecPhys.getBody().getPosition())));

					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;

					body.applyForce(ssvs::getVecFromDegrees(snappedDegrees, walkSpeed) * 0.05f);
				}
			}

			inline void draw() override
			{
				auto& s0(cDraw[0]);
				//s0.setTextureRect(assets.get<Tileset>("tileset")Enemy[{0, 0}]);
				s0.setRotation(snappedDegrees);
			}

			inline void setWalkSpeed(float mValue) noexcept	{ walkSpeed = mValue; }
			inline void setTurnSpeed(float mValue) noexcept	{ turnSpeed = mValue; }
			inline void setGibMult(int mValue) noexcept		{ gibMult = mValue; }

			inline OBGame& getGame() const noexcept			{ return game; }
			inline OBCPhys& getCPhys() const noexcept		{ return cPhys; }
			inline OBCDraw& getCDraw() const noexcept		{ return cDraw; }
			inline OBCHealth& getCHealth() const noexcept	{ return cHealth; }
			inline float getCurrentDegrees() const noexcept	{ return currentDegrees; }
	};

	class OBCEJuggernaut : public sses::Component
	{
		private:
			OBCEnemy& cEnemy;
			OBGame& game;
			OBCPhys& cPhys;
			OBCDraw& cDraw;
			OBCHealth& cHealth;
			OBAssets& assets;
			ssvsc::Body& body;
			ssvs::Ticker juggernautShootTimer{150.f};
			ssvu::Timeline juggernautShootTimeline;

			float lastDeg{0};

		public:
			OBCEJuggernaut(OBCEnemy& mCEnemy) : cEnemy(mCEnemy), game(cEnemy.getGame()), cPhys(cEnemy.getCPhys()), cDraw(cEnemy.getCDraw()), cHealth(cEnemy.getCHealth()), assets(game.getAssets()), body(cPhys.getBody()) { }

			inline void init() override
			{
				juggernautShootTimeline.append<ssvu::Do>([this]{ shoot(ssvu::getRnd(-10, 10)); });
				juggernautShootTimeline.append<ssvu::Wait>(1.1f);
				juggernautShootTimeline.append<ssvu::Go>(0, 8);
				juggernautShootTimeline.append<ssvu::Wait>(15.f);
				juggernautShootTimeline.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-100.f); });
				juggernautShootTimeline.append<ssvu::Do>([this]{ shoot(lastDeg); lastDeg += 265; });
				juggernautShootTimeline.append<ssvu::Wait>(0.3f);
				juggernautShootTimeline.append<ssvu::Go>(5, 45);
				juggernautShootTimeline.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });
			}
			inline void update(float mFrameTime) override
			{
				juggernautShootTimeline.update(mFrameTime);
				if(juggernautShootTimer.update(mFrameTime)) { juggernautShootTimeline.reset(); juggernautShootTimeline.start(); }
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
