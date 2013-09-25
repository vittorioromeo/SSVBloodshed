// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMYTYPES
#define SSVOB_COMPONENTS_ENEMYTYPES

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCFloor.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCEnemy.h"
#include "SSVBloodshed/Components/OBCFloorSmasher.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCWielder.h"

namespace ob
{
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

	class OBCEArmedBase : public OBCEBase
	{
		protected:
			OBCWielder& cWielder;
			OBCDirection8& cDirection8;
			bool armed{false};
			int weaponHealth{1};

		public:
			OBCEArmedBase(OBCEnemy& mCEnemy, OBCWielder& mCWielder, bool mArmed, int mWeaponHealth)
				: OBCEBase{mCEnemy}, cWielder(mCWielder), cDirection8(mCWielder.getCDirection()), armed{mArmed}, weaponHealth(mWeaponHealth)
			{
				cKillable.getCHealth().onDamage += [this]{ if(armed && weaponHealth-- <= 0) { armed = false; game.createPElectric(10, toPixels(cPhys.getPosF())); } };
			}

			inline void pursuitOrAlign(float mDist, float mPursuitDist)
			{
				if(mDist > mPursuitDist) cBoid.pursuit(cTargeter.getTarget());
				else cBoid.seek(ssvs::getOrbitFromDegrees(cTargeter.getPosF(), cDirection8.getDegrees() + 180, mPursuitDist), 0.018f, 1250.f);
			}
			inline void faceShootingDirection() { cDirection8 = getDirection8FromDegrees(cEnemy.getCurrentDegrees()); }
			inline void recalculateTile(const sf::IntRect& mUnarmedStand, const sf::IntRect& mArmedStand, const sf::IntRect& mArmedShoot)
			{
				if(armed)
				{
					cDraw[0].setRotation(cDirection8.getDegrees());
					cDraw[0].setTextureRect(cWielder.isShooting() ? mArmedShoot : mArmedStand);
				}
				else cDraw[0].setTextureRect(mUnarmedStand);
			}
	};

	class OBCERunner : public OBCEArmedBase
	{
		private:
			ssvs::Ticker timerShoot{150.f};

		public:
			OBCERunner(OBCEnemy& mCEnemy, OBCWielder& mCWielder, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mArmed, 1} { }

			inline void init() override
			{
				cEnemy.setWalkSpeed(150.f); cEnemy.setTurnSpeed(4.5f); cEnemy.setMaxVelocity(200.f);
				cKillable.setType(OBCKillable::Type::Organic);
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && distance < 10000);

					if(armed)
					{
						pursuitOrAlign(distance, 9000.f); faceShootingDirection();
						if(cWielder.isShooting() && timerShoot.update(mFT)) shootGun();
					}
					else cBoid.pursuit(cTargeter.getTarget());
				}
				else cWielder.setShooting(false);
			}
			inline void draw() override { recalculateTile(assets.e1Stand, assets.e2Stand, assets.e2Shoot); }

			inline void shootGun()
			{
				assets.playSound("Sounds/spark.wav"); game.createPMuzzle(20, cPhys.getPosPixels());
				getFactory().createPJEnemyStar(cWielder.getShootingPos(), cDirection8.getDegrees());
			}
	};

	class OBCECharger : public OBCEArmedBase
	{
		private:
			OBCFloorSmasher& cFloorSmasher;
			ssvs::Ticker timerGunShoot{35.f}, timerCharge{250.f};
			ssvu::Timeline tlCharge{false};
			float lastDeg{0};

		public:
			OBCECharger(OBCEnemy& mCEnemy, OBCFloorSmasher& mCFloorSmasher, OBCWielder& mCWielder, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mArmed, 9}, cFloorSmasher(mCFloorSmasher) { }

			inline void init() override
			{
				cEnemy.setWalkSpeed(25.f); cEnemy.setTurnSpeed(3.2f); cEnemy.setMaxVelocity(50.f);
				cWielder.setWieldDistance(2000.f);
				cKillable.setType(OBCKillable::Type::Organic);
				cKillable.setParticleMult(2);

				repeat(tlCharge, [this]{ body.setVelocity(body.getVelocity() * 0.8f); }, 10, 2.5f);
				tlCharge.append<ssvu::Do>([this]{ cFloorSmasher.setActive(true); lastDeg = cEnemy.getCurrentDegrees(); body.applyForce(ssvs::getVecFromDegrees(lastDeg, 1250.f)); });
				tlCharge.append<ssvu::Wait>(10.f);
				tlCharge.append<ssvu::Do>([this]{ body.applyForce(ssvs::getVecFromDegrees(lastDeg, -150.f)); });
				tlCharge.append<ssvu::Wait>(9.f);
				tlCharge.append<ssvu::Do>([this]{ cFloorSmasher.setActive(false); cEnemy.setWalkSpeed(20.f); });
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && distance < 10000);

					if(armed)
					{
						pursuitOrAlign(distance, 9000.f); faceShootingDirection();
						if(cWielder.isShooting() && timerGunShoot.update(mFT)) shootGun();
					}
					else
					{
						cBoid.pursuit(cTargeter.getTarget());
						tlCharge.update(mFT);
						if(timerCharge.update(mFT)) { tlCharge.reset(); tlCharge.start(); }
					}
				}
				else cWielder.setShooting(false);
			}
			inline void draw() override { recalculateTile(assets.e3Stand, assets.e6Stand, assets.e6Shoot); }

			inline void shootGun()
			{
				assets.playSound("Sounds/spark.wav"); game.createPMuzzle(20, cPhys.getPosPixels());
				for(float i{-10}; i <= 10; i += 10.f) getFactory().createPJEnemyStar(cWielder.getShootingPos(), cDirection8.getDegrees() + i);
			}
	};

	class OBCEJuggernaut : public OBCEArmedBase
	{
		private:
			ssvs::Ticker timerGunShoot{25.f};
			ssvs::Ticker timerShoot{150.f};
			ssvu::Timeline tlShoot{false};
			float lastDeg{0};

		public:
			OBCEJuggernaut(OBCEnemy& mCEnemy, OBCWielder& mCWielder, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mArmed, 18} { }

			inline void init() override
			{
				cEnemy.setWalkSpeed(25.f); cEnemy.setTurnSpeed(3.2f); cEnemy.setMaxVelocity(50.f);
				cKillable.setParticleMult(4);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };
				cKillable.setType(OBCKillable::Type::Organic);
				cKillable.setParticleMult(2);
				cWielder.setWieldDistance(2800.f);

				repeat(tlShoot, [this]{ shootUnarmed(ssvu::getRnd(-10, 10)); }, 8, 1.1f);
				tlShoot.append<ssvu::Wait>(15.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-100.f); });
				repeat(tlShoot, [this]{ shootUnarmed(lastDeg); lastDeg += 265; }, 45, 0.3f);
				tlShoot.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && distance < 10000);

					if(armed)
					{
						pursuitOrAlign(distance, 9000.f); faceShootingDirection();
						if(cWielder.isShooting() && timerGunShoot.update(mFT)) shootGun();
					}
					else
					{
						if(distance > 10000) cBoid.pursuit(cTargeter.getTarget()); else cBoid.evade(cTargeter.getTarget());

						tlShoot.update(mFT);
						if(timerShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
					}
				}
				else cWielder.setShooting(false);
			}
			inline void draw() override { recalculateTile(assets.e4Stand, assets.e7Stand, assets.e7Shoot); }

			inline void shootGun()
			{
				assets.playSound("Sounds/spark.wav"); game.createPMuzzle(20, cPhys.getPosPixels());
				for(float i{-20}; i <= 20; i += 5) getFactory().createPJEnemyStar(cWielder.getShootingPos(), cDirection8.getDegrees() + i);
			}
			inline void shootUnarmed(int mDeg)
			{
				assets.playSound("Sounds/spark.wav"); game.createPMuzzle(20, cPhys.getPosPixels());
				getFactory().createPJEnemyStar(cPhys.getPosI(), cEnemy.getCurrentDegrees() + mDeg);
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
				//cKillable.onDeath += [this]{ for(int i = 0; i < 4; ++i) game.getFactory().createEBallSmall(cPhys.getPosI()); };
				cKillable.setType(OBCKillable::Type::Robotic);
				cFloorSmasher.setActive(true);
				cEnemy.setFaceDirection(false);
				cEnemy.setWalkSpeed(200.f); cEnemy.setTurnSpeed(3.f); cEnemy.setMaxVelocity(400.f);
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget()) cBoid.pursuit(cTargeter.getTarget());
				spin += mFT * 15.f;
			}
			inline void draw() override { cDraw.setRotation(spin); }
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

				repeat(tlShoot, [this]{ shoot(ssvu::getRnd(-15, 15)); }, 20, 0.4f);
				tlShoot.append<ssvu::Wait>(19.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-50.f); });
				repeat(tlShoot, [this]{ shoot(lastDeg); lastDeg += 235; }, 150, 0.1f);
				tlShoot.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });

				tlSummon.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(0.f); });
				repeat(tlSummon, [this]
				{
					body.setVelocity(body.getVelocity() * 0.8f);
					getFactory().createERunner(body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(lastDeg) * 1000.f), false);
					lastDeg += 360 / 16;
				}, 16, 4.5f);
				tlSummon.append<ssvu::Wait>(19.f);
				tlSummon.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(10.f); });
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					if(ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF()) > 10000) cBoid.pursuit(cTargeter.getTarget()); else cBoid.evade(cTargeter.getTarget());

					tlShoot.update(mFT);
					tlSummon.update(mFT);
					if(timerShoot.update(mFT))
					{
						if(ssvu::getRnd(0, 2) > 0) { tlShoot.reset(); tlShoot.start(); }
						else { tlSummon.reset(); tlSummon.start(); }
					}
				}
			}
			inline void shoot(int mDeg)
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(cEnemy.getCurrentDegrees()) * 100.f)};
				getFactory().createPJEnemyStar(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, cPhys.getPosPixels());
			}
	};
}

#endif

