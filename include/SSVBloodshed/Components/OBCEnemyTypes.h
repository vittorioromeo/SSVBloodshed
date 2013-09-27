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
#include "SSVBloodshed/Components/OBCWpnController.h"
#include "SSVBloodshed/Weapons/OBWpnTypes.h"

namespace ob
{
	bool raycastToPlayer(OBGame& mGame, OBCPhys& mSeeker, OBCPhys& mTarget)
	{
		const auto& startPos(mSeeker.getPosI());
		Vec2f direction(mTarget.getPosI() - startPos);
		if(direction.x == 0 && direction.y == 0) return false;

		auto gridQuery(mGame.getWorld().getQuery<ssvsc::HashGrid, ssvsc::QueryType::RayCast>(startPos, direction));

		Body* body;
		while((body = gridQuery.next()) != nullptr)
		{
			if(body == &mSeeker.getBody()) continue;
			if(body->hasGroup(OBGroup::GEnemy)) continue;
			if(body->hasGroup(OBGroup::GFriendly)) return true;
			if(body->hasGroup(OBGroup::GSolidAir)) return false;
		}

		return false;
	}

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
			OBCWpnController& cWpnController;
			bool armed{false};
			int weaponHealth{1};

		public:
			OBCEArmedBase(OBCEnemy& mCEnemy, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed, int mWeaponHealth)
				: OBCEBase{mCEnemy}, cWielder(mCWielder), cDirection8(mCWielder.getCDirection()), cWpnController(mCWpnController), armed{mArmed}, weaponHealth(mWeaponHealth)
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

			inline void shootGun()
			{
				if(cWpnController.shoot(cWielder.getShootingPos(), cDirection8.getDegrees())) game.createPMuzzle(20, toPixels(cWielder.getShootingPos()));
			}
	};

	class OBCERunner : public OBCEArmedBase
	{
		public:
			OBCERunner(OBCEnemy& mCEnemy, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mCWpnController, mArmed, 1} { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createEPlasmaStarGun(0));
				cEnemy.setWalkSpeed(150.f); cEnemy.setTurnSpeed(4.5f); cEnemy.setMaxVelocity(200.f);
				cKillable.setType(OBCKillable::Type::Organic);
			}
			inline void update(float) override
			{
				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && raycastToPlayer(game, cPhys, cTargeter.getTarget()));

					if(armed)
					{
						pursuitOrAlign(distance, 9000.f); faceShootingDirection();
						if(cWielder.isShooting()) shootGun();
					}
					else cBoid.pursuit(cTargeter.getTarget());
				}
				else cWielder.setShooting(false);
			}
			inline void draw() override { recalculateTile(assets.e1UAStand, assets.e1AStand, assets.e1AShoot); }
	};

	class OBCECharger : public OBCEArmedBase
	{
		private:
			OBCFloorSmasher& cFloorSmasher;
			ssvs::Ticker tckCharge{250.f};
			ssvu::Timeline tlCharge{false};
			float lastDeg{0};

		public:
			OBCECharger(OBCEnemy& mCEnemy, OBCFloorSmasher& mCFloorSmasher, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed)
				: OBCEArmedBase{mCEnemy, mCWielder, mCWpnController, mArmed, 9}, cFloorSmasher(mCFloorSmasher) { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createEPlasmaStarGun(1));
				cEnemy.setWalkSpeed(25.f); cEnemy.setTurnSpeed(3.2f); cEnemy.setMaxVelocity(50.f);
				cWielder.setWieldDistance(2000.f);
				cKillable.setType(OBCKillable::Type::Organic);
				cKillable.setParticleMult(2);

				repeat(tlCharge, [this]
				{
					body.setVelocity(cPhys.getVel() * 0.8f);
					game.createPCharge(4, cPhys.getPosPixels(), 45);
				}, 10, 2.5f);
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
					cWielder.setShooting(armed && raycastToPlayer(game, cPhys, cTargeter.getTarget()));

					if(armed)
					{
						pursuitOrAlign(distance, 9000.f); faceShootingDirection();
						if(cWielder.isShooting()) shootGun();
					}
					else
					{
						cBoid.pursuit(cTargeter.getTarget());
						tlCharge.update(mFT);
						if(tckCharge.update(mFT)) { tlCharge.reset(); tlCharge.start(); }
					}
				}
				else cWielder.setShooting(false);
			}
			inline void draw() override { recalculateTile(assets.e2UAStand, assets.e2AStand, assets.e2AShoot); }
	};

	class OBCEJuggernaut : public OBCEArmedBase
	{
		private:
			ssvs::Ticker tckShoot{150.f};
			ssvu::Timeline tlShoot{false};
			float lastDeg{0};

		public:
			OBCEJuggernaut(OBCEnemy& mCEnemy, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mCWpnController, mArmed, 18} { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createEPlasmaStarGun(2));
				cEnemy.setWalkSpeed(25.f); cEnemy.setTurnSpeed(3.2f); cEnemy.setMaxVelocity(50.f);
				cKillable.setParticleMult(4);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };
				cKillable.setType(OBCKillable::Type::Organic);
				cKillable.setParticleMult(2);
				cWielder.setWieldDistance(2800.f);

				repeat(tlShoot, [this]{ shootUnarmed(ssvu::getRnd(-10, 10)); }, 8, 1.1f);
				repeat(tlShoot, [this]{ game.createPCharge(4, cPhys.getPosPixels(), 55); }, 15, 1.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-100.f); });
				repeat(tlShoot, [this]{ shootUnarmed(lastDeg); lastDeg += 265; }, 45, 0.3f);
				tlShoot.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });
			}
			inline void update(float mFT) override
			{
				constexpr float distBodySlam{2500.f};
				constexpr float distEvade{10000.f};

				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && raycastToPlayer(game, cPhys, cTargeter.getTarget()));

					if(armed && distance > distBodySlam)
					{
						pursuitOrAlign(distance, distEvade - 1000.f); faceShootingDirection();
						if(cWielder.isShooting()) shootGun();
					}
					else
					{
						if(distance > distEvade || distance < distBodySlam) cBoid.pursuit(cTargeter.getTarget()); else cBoid.evade(cTargeter.getTarget());

						tlShoot.update(mFT);
						if(tckShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
					}
				}
				else cWielder.setShooting(false);
			}
			inline void draw() override { recalculateTile(assets.e3UAStand, assets.e3AStand, assets.e3AShoot); }

			inline void shootUnarmed(int mDeg)
			{
				assets.playSound("Sounds/spark.wav"); game.createPMuzzle(20, cPhys.getPosPixels());
				getFactory().createPJStar(cPhys.getPosI(), cEnemy.getCurrentDegrees() + mDeg);
			}
	};

	class OBCEBall : public OBCEBase
	{
		private:
			float spin{0};
			bool flying{false}, small{false};

		public:
			OBCEBall(OBCEnemy& mCEnemy, bool mFlying, bool mSmall) : OBCEBase{mCEnemy}, flying{mFlying}, small{mSmall} { }

			inline void init() override
			{
				if(!small)
				{
					cKillable.onDeath += [this]
					{
						for(int i{0}; i < 3; ++i)
						{
							auto& e(getFactory().createEBall(cPhys.getPosI(), flying, true));
							e.getComponent<OBCPhys>().setVel(ssvs::getVecFromDegrees(360.f / 3.f * i, 400.f));
						}
					};
				}
				else cKillable.setParticleMult(0.3f);

				cKillable.setType(OBCKillable::Type::Robotic);
				cEnemy.setFaceDirection(false);
				cEnemy.setWalkSpeed(200.f); cEnemy.setTurnSpeed(3.f); cEnemy.setMaxVelocity(400.f);

				if(flying)
				{
					body.onResolution += [this](const ResolutionInfo& mRI)
					{
						if(!mRI.body.hasGroup(OBGroup::GSolidAir) && mRI.body.hasGroup(OBGroup::GSolidGround))
							mRI.noResolvePosition = mRI.noResolveVelocity = true;
					};
					body.addGroupToCheck(OBGroup::GSolidAir);
				}
			}
			inline void update(float mFT) override
			{
				if(flying && !small && ssvu::getRnd(0, 9) > 7) game.createPElectric(1, cPhys.getPosPixels());
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
			ssvu::Timeline tlShoot{false}, tlSummon{false};
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
				repeat(tlShoot, [this]{ game.createPCharge(5, cPhys.getPosPixels(), 65); }, 19, 1.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(-50.f); });
				repeat(tlShoot, [this]{ shoot(lastDeg); lastDeg += 235; }, 150, 0.1f);
				tlShoot.append<ssvu::Do>([this]{ cEnemy.setWalkSpeed(100.f); });

				tlSummon.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDegrees(); cEnemy.setWalkSpeed(0.f); });
				repeat(tlSummon, [this]
				{
					game.createPCharge(5, cPhys.getPosPixels(), 65);
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
				getFactory().createPJStar(shootPos, cEnemy.getCurrentDegrees() + mDeg);
				game.createPMuzzle(20, cPhys.getPosPixels());
			}
	};
}

#endif

