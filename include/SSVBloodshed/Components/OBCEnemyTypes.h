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
#include "SSVBloodshed/Components/OBCFloorBreaker.h"
#include "SSVBloodshed/Components/OBCKillable.h"

namespace ob
{
	template<typename T> class PingPongValue
	{
		private:
			T value, min, max, speed, dir{1};

		public:
			inline PingPongValue(T mMin, T mMax, T mSpeed) : value(mMin), min(mMin), max(mMax), speed(mSpeed) { }

			inline void update(float mFrameTime)
			{
				value += speed * mFrameTime * dir;
				if(value > max) { value = max; dir = -1; }
				else if(value < min) { value = min; dir = 1; }
			}

			inline operator T() const noexcept { return value; }
	};

	class OBCEBase : public OBCActorBase
	{
		protected:
			OBCEnemy& cEnemy;
			OBCKillable& cKillable;

		public:
			OBCEBase(OBCEnemy& mCEnemy) : OBCActorBase{mCEnemy.getCPhys(), mCEnemy.getCDraw()}, cEnemy(mCEnemy), cKillable(cEnemy.getCKillable()) { }
	};

	class OBCEBall : public OBCEBase
	{
		private:
			OBCFloorBreaker& cFloorBreaker;
			float deg{0};
			PingPongValue<float> ts{0.1f, 15.f, 0.07f};

		public:
			OBCEBall(OBCEnemy& mCEnemy, OBCFloorBreaker& mCFloorBreaker) : OBCEBase{mCEnemy}, cFloorBreaker(mCFloorBreaker) { }

			inline void init() override
			{
				cKillable.setType(OBCKillable::Type::Robotic);

				//cKillable.onDeath += [this]{ for(int i = 0; i < 4; ++i) game.getFactory().createTestEnemyBallSmall(cPhys.getPos<int>()); };
				cFloorBreaker.setActive(true);

				cEnemy.setFaceDirection(false);
				cEnemy.setWalkSpeed(200.f);
				cEnemy.setTurnSpeed(3.f);

				cEnemy.setMaxVelocity(400.f);
			}
			inline void update(float mFrameTime) override
			{
				deg += mFrameTime * 15.f;
				ts.update(mFrameTime);
			//	cEnemy.setWalkSpeed(ts * 50.f);

			}
			inline void draw() override { cDraw.setRotation(deg); }
	};

	class OBCECharger : public OBCEBase
	{
		private:
			OBCFloorBreaker& cFloorBreaker;
			ssvs::Ticker timerCharge{250.f};
			ssvu::Timeline tlCharge{false};
			float lastDeg{0};

		public:
			OBCECharger(OBCEnemy& mCEnemy, OBCFloorBreaker& mCFloorBreaker) : OBCEBase{mCEnemy}, cFloorBreaker(mCFloorBreaker) { }

			inline void init() override
			{
				cKillable.setParticleMult(2);

				cEnemy.setWalkSpeed(20.f);
				cEnemy.setTurnSpeed(3.f);
				cEnemy.setMaxVelocity(40.f);

				tlCharge.append<ssvu::Do>([this]{ body.setVelocity(body.getVelocity() * 0.8f); });
				tlCharge.append<ssvu::Wait>(2.5f);
				tlCharge.append<ssvu::Go>(0, 10);
				tlCharge.append<ssvu::Do>([this]{ cFloorBreaker.setActive(true); lastDeg = cEnemy.getCurrentDegrees(); body.applyForce(ssvs::getVecFromDegrees(lastDeg, 1250.f)); });
				tlCharge.append<ssvu::Wait>(10.f);
				tlCharge.append<ssvu::Do>([this]{ body.applyForce(ssvs::getVecFromDegrees(lastDeg, -150.f)); });
				tlCharge.append<ssvu::Wait>(9.f);
				tlCharge.append<ssvu::Do>([this]{ cFloorBreaker.setActive(false); cEnemy.setWalkSpeed(20.f); });
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
				cKillable.setParticleMult(4);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };

				cEnemy.setWalkSpeed(10.f);
				cEnemy.setTurnSpeed(2.5f);

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
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(cEnemy.getCurrentDegrees()) * 100.f)};
				getFactory().createProjectileEnemyBullet(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};

	class OBCEGiant : public OBCEBase
	{
		private:
			OBCFloorBreaker& cFloorBreaker;
			ssvs::Ticker timerShoot{185.f};
			ssvu::Timeline tlShoot{false};
			ssvu::Timeline tlSummon{false};
			float lastDeg{0};

		public:
			OBCEGiant(OBCEnemy& mCEnemy, OBCFloorBreaker& mCFloorBreaker) : OBCEBase{mCEnemy}, cFloorBreaker(mCFloorBreaker) { }

			inline void init() override
			{
				cKillable.setParticleMult(8);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };

				cEnemy.setWalkSpeed(10.f);
				cEnemy.setTurnSpeed(2.5f);

				cFloorBreaker.setActive(true);

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
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(cEnemy.getCurrentDegrees()) * 100.f)};
				getFactory().createProjectileEnemyBullet(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};
}

#endif
