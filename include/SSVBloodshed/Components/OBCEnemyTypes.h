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
#include "SSVBloodshed/Components/OBCFloorSmasher.h"
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
			OBCBoid& cBoid;
			OBCTargeter& cTargeter;

		public:
			OBCEBase(OBCEnemy& mCEnemy) : OBCActorBase{mCEnemy.getCPhys(), mCEnemy.getCDraw()}, cEnemy(mCEnemy), cKillable(cEnemy.getCKillable()), cBoid(cEnemy.getCBoid()), cTargeter(cEnemy.getCTargeter()) { }
	};

	class OBCERunner : public OBCEBase
	{
		public:
			OBCERunner(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cKillable.setType(OBCKillable::Type::Organic);
				cEnemy.setWalkSpeed(150.f);
				cEnemy.setTurnSpeed(4.5f);
				cEnemy.setMaxVelocity(200.f);
			}
			inline void update(float) override { if(cTargeter.hasTarget()) cBoid.pursuit(cTargeter.getTarget()); }
	};

	class OBCEGunner : public OBCEBase
	{
		private:
			ssvs::Ticker timerShoot{150.f};
			Direction direction{Direction::E};
			bool inRange{false};

		public:
			OBCEGunner(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cKillable.setType(OBCKillable::Type::Organic);
				cEnemy.setWalkSpeed(100.f);
				cEnemy.setTurnSpeed(3.5f);
				cEnemy.setMaxVelocity(110.f);
			}
			inline void update(float mFrameTime) override
			{
				if(cTargeter.hasTarget())
				{
					const auto& tp(cTargeter.getPosF());
					const auto& p(cPhys.getPosF());

					float distance{ssvs::getDistEuclidean(tp, p)};

					inRange = distance < 10000;

					if(distance > 9000) cBoid.pursuit(cTargeter.getTarget());
					else body.applyForce((cPhys.getPosF() - ssvs::getOrbitFromDegrees(cPhys.getPosF(), getDegreesFromDirection(direction), 3000.f)) * 0.0005f);

					if(inRange && timerShoot.update(mFrameTime)) shoot(0);
					direction = getDirectionFromDegrees(cEnemy.getCurrentDegrees());
				}
				else inRange = false;
			}
			inline void draw() override
			{
				// TODO: new component and merge with player
				const auto& intDir(static_cast<int>(direction));
				cDraw.setRotation(45 * intDir);

				if(inRange)
				{
					cDraw.getOffsets()[1] = ssvs::getVecFromDegrees(getDegreesFromDirection(direction)) * 10.f;
					cDraw[0].setTextureRect(assets.e2Shoot);
					cDraw[1].setColor({255, 255, 255, 255});
					cDraw[1].setTextureRect(assets.e2Gun);
				}
				else
				{
					cDraw[0].setTextureRect(assets.e2Stand);
					cDraw[1].setColor({255, 255, 255, 0});
				}
			}

			inline void shoot(int mDeg)
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(getDegreesFromDirection(direction)) * 100.f)};
				getFactory().createProjectileEnemyBullet(shootPos, getDegreesFromDirection(direction) + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};

	class OBCEBall : public OBCEBase
	{
		private:
			OBCFloorSmasher& cFloorSmasher;
			float spin{0};

		public:
			OBCEBall(OBCEnemy& mCEnemy, OBCFloorSmasher& mCFloorSmasher) : OBCEBase{mCEnemy}, cFloorSmasher(mCFloorSmasher) { }

			inline void init() override
			{
				cKillable.setType(OBCKillable::Type::Robotic);
				//cKillable.onDeath += [this]{ for(int i = 0; i < 4; ++i) game.getFactory().createTestEnemyBallSmall(cPhys.getPosI()); };

				cFloorSmasher.setActive(true);

				cEnemy.setFaceDirection(false);
				cEnemy.setWalkSpeed(200.f);
				cEnemy.setTurnSpeed(3.f);

				cEnemy.setMaxVelocity(400.f);
			}
			inline void update(float mFrameTime) override
			{
				if(cTargeter.hasTarget()) cBoid.pursuit(cTargeter.getTarget());
				spin += mFrameTime * 15.f;
			}
			inline void draw() override { cDraw.setRotation(spin); }
	};

	class OBCECharger : public OBCEBase
	{
		private:
			OBCFloorSmasher& cFloorSmasher;
			ssvs::Ticker timerCharge{250.f};
			ssvu::Timeline tlCharge{false};
			float lastDeg{0};

		public:
			OBCECharger(OBCEnemy& mCEnemy, OBCFloorSmasher& mCFloorSmasher) : OBCEBase{mCEnemy}, cFloorSmasher(mCFloorSmasher) { }

			inline void init() override
			{
				cKillable.setParticleMult(2);

				cEnemy.setWalkSpeed(20.f);
				cEnemy.setTurnSpeed(3.f);
				cEnemy.setMaxVelocity(40.f);

				tlCharge.append<ssvu::Do>([this]{ body.setVelocity(body.getVelocity() * 0.8f); });
				tlCharge.append<ssvu::Wait>(2.5f);
				tlCharge.append<ssvu::Go>(0, 10);
				tlCharge.append<ssvu::Do>([this]{ cFloorSmasher.setActive(true); lastDeg = cEnemy.getCurrentDegrees(); body.applyForce(ssvs::getVecFromDegrees(lastDeg, 1250.f)); });
				tlCharge.append<ssvu::Wait>(10.f);
				tlCharge.append<ssvu::Do>([this]{ body.applyForce(ssvs::getVecFromDegrees(lastDeg, -150.f)); });
				tlCharge.append<ssvu::Wait>(9.f);
				tlCharge.append<ssvu::Do>([this]{ cFloorSmasher.setActive(false); cEnemy.setWalkSpeed(20.f); });
			}
			inline void update(float mFrameTime) override
			{
				if(cTargeter.hasTarget())
				{
					cBoid.pursuit(cTargeter.getTarget());
					tlCharge.update(mFrameTime);
					if(timerCharge.update(mFrameTime)) { tlCharge.reset(); tlCharge.start(); }
				}
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
				if(cTargeter.hasTarget())
				{
					if(ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF()) > 10000) cBoid.pursuit(cTargeter.getTarget());
					else cBoid.evade(cTargeter.getTarget());

					tlShoot.update(mFrameTime);
					if(timerShoot.update(mFrameTime)) { tlShoot.reset(); tlShoot.start(); }
				}
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
			OBCFloorSmasher& cFloorSmasher;
			ssvs::Ticker timerShoot{185.f};
			ssvu::Timeline tlShoot{false};
			ssvu::Timeline tlSummon{false};
			float lastDeg{0};

		public:
			OBCEGiant(OBCEnemy& mCEnemy, OBCFloorSmasher& mCFloorSmasher) : OBCEBase{mCEnemy}, cFloorSmasher(mCFloorSmasher) { }

			inline void init() override
			{
				cKillable.setParticleMult(8);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };

				cEnemy.setWalkSpeed(10.f);
				cEnemy.setTurnSpeed(2.5f);

				cFloorSmasher.setActive(true);

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
				if(cTargeter.hasTarget())
				{
					if(ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF()) > 10000) cBoid.pursuit(cTargeter.getTarget());
					else cBoid.evade(cTargeter.getTarget());

					tlShoot.update(mFrameTime);
					tlSummon.update(mFrameTime);
					if(timerShoot.update(mFrameTime))
					{
						if(ssvu::getRnd(0, 2) > 0)
						{
							tlShoot.reset(); tlShoot.start();
						}
						else
						{
							tlSummon.reset(); tlSummon.start();
						}
					}
				}
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

