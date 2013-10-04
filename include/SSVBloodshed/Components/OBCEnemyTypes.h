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
	inline bool raycastToPlayer(OBCPhys& mSeeker, OBCPhys& mTarget)
	{
		const auto& startPos(mSeeker.getPosI());
		Vec2f direction(mTarget.getPosI() - startPos);
		if(direction.x == 0 && direction.y == 0) return false;

		auto gridQuery(mSeeker.getWorld().getQuery<ssvsc::QueryType::RayCast>(startPos, direction));

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
			OBCDir8& cDir8;
			OBCWpnController& cWpnController;
			bool armed{false};
			int wpnHealth{1};

		public:
			OBCEArmedBase(OBCEnemy& mCEnemy, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed, int mWpnHealth)
				: OBCEBase{mCEnemy}, cWielder(mCWielder), cDir8(mCWielder.getCDir8()), cWpnController(mCWpnController), armed{mArmed}, wpnHealth(mWpnHealth)
			{
				cKillable.getCHealth().onDamage += [this]{ if(armed && wpnHealth-- <= 0) { armed = false; game.createPElectric(10, toPixels(cPhys.getPosF())); } };
			}

			inline void pursuitOrAlign(float mDist, float mPursuitDist)
			{
				if(mDist > mPursuitDist) cBoid.pursuit(cTargeter.getTarget());
				else cBoid.seek(ssvs::getOrbitFromDeg(cTargeter.getPosF(), cDir8.getDeg() + 180, mPursuitDist), 0.02f, 750.f);
			}
			inline void faceShootDir() { cDir8 = getDir8FromDeg(cEnemy.getCurrentDeg()); }
			inline void recalculateTile(const sf::IntRect& mUnarmedStand, const sf::IntRect& mArmedStand, const sf::IntRect& mArmedShoot)
			{
				if(armed)
				{
					cDraw[0].setRotation(cDir8.getDeg());
					cDraw[0].setTextureRect(cWielder.isShooting() ? mArmedShoot : mArmedStand);
				}
				else cDraw[0].setTextureRect(mUnarmedStand);
			}

			inline void shootGun()
			{
				if(cWpnController.shoot(cWielder.getShootingPos(), cDir8.getDeg())) game.createPMuzzle(20, toPixels(cWielder.getShootingPos()));
			}
	};

	class OBCERunner : public OBCEArmedBase
	{
		public:
			OBCERunner(OBCEnemy& mCEnemy, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mCWpnController, mArmed, 1} { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createEPlasmaBulletGun());
				cEnemy.setMinBounceVel(125.f); cEnemy.setMaxVel(200.f);
				cKillable.setType(OBCKillable::Type::Organic);
			}
			inline void update(float) override
			{
				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					bool raycast{raycastToPlayer(cPhys, cTargeter.getTarget())};

					cWielder.setShooting(armed && raycast);
					if(armed) faceShootDir();

					if(cWielder.isShooting())
					{
						pursuitOrAlign(distance, 9000.f);
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
				: OBCEArmedBase{mCEnemy, mCWielder, mCWpnController, mArmed, 7}, cFloorSmasher(mCFloorSmasher) { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createEPlasmaBulletGun(1, 8.f));
				cEnemy.setMinBounceVel(20.f); cEnemy.setMaxVel(50.f);
				cWielder.setWieldDist(2000.f);
				cKillable.setType(OBCKillable::Type::Organic);
				cKillable.setParticleMult(2);

				repeat(tlCharge, [this]
				{
					body.setVelocity(cPhys.getVel() * 0.8f);
					game.createPCharge(4, cPhys.getPosPixels(), 45);
				}, 10, 2.5f);
				tlCharge.append<ssvu::WaitUntil>([this]{ return raycastToPlayer(cPhys, cTargeter.getTarget()); });
				tlCharge.append<ssvu::Do>([this]{ cFloorSmasher.setActive(true); lastDeg = cEnemy.getCurrentDeg(); body.applyForce(ssvs::getVecFromDeg(lastDeg, 1250.f)); });
				tlCharge.append<ssvu::Wait>(10.f);
				tlCharge.append<ssvu::Do>([this]{ body.applyForce(ssvs::getVecFromDeg(lastDeg, -150.f)); });
				tlCharge.append<ssvu::Wait>(9.f);
				tlCharge.append<ssvu::Do>([this]{ cFloorSmasher.setActive(false); });
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && raycastToPlayer(cPhys, cTargeter.getTarget()));

					if(armed)
					{
						pursuitOrAlign(distance, 9000.f); faceShootDir();
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
			OBWpn wpn{game, OBGroup::GFriendly, OBWpnTypes::createEPlasmaStarGun(0)};

		public:
			OBCEJuggernaut(OBCEnemy& mCEnemy, OBCWielder& mCWielder, OBCWpnController& mCWpnController, bool mArmed) : OBCEArmedBase{mCEnemy, mCWielder, mCWpnController, mArmed, 18} { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createEPlasmaBulletGun(2, 8.f));
				cEnemy.setMinBounceVel(15.f); cEnemy.setMaxVel(50.f);
				cKillable.setParticleMult(4);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };
				cKillable.setType(OBCKillable::Type::Organic);
				cKillable.setParticleMult(2);
				cWielder.setWieldDist(2800.f);

				repeat(tlShoot, [this]{ shootUnarmed(ssvu::getRnd(-10, 10)); }, 8, 1.1f);
				repeat(tlShoot, [this]{ game.createPCharge(4, cPhys.getPosPixels(), 55); }, 15, 1.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDeg(); });
				repeat(tlShoot, [this]{ shootUnarmed(lastDeg); lastDeg += 265; }, 45, 0.3f);
			}
			inline void update(float mFT) override
			{
				constexpr float distBodySlam{2500.f};
				constexpr float distEvade{10000.f};

				if(cTargeter.hasTarget())
				{
					float distance{ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF())};
					cWielder.setShooting(armed && raycastToPlayer(cPhys, cTargeter.getTarget()));

					if(armed && distance > distBodySlam)
					{
						pursuitOrAlign(distance, distEvade - 1000.f); faceShootDir();
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
				wpn.shoot(cPhys.getPosI(), cEnemy.getCurrentDeg() + mDeg);
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
							e.getComponent<OBCPhys>().setVel(ssvs::getVecFromDeg(360.f / 3.f * i, 400.f));
						}
					};
				}
				else cKillable.setParticleMult(0.3f);

				cKillable.setType(OBCKillable::Type::Robotic);
				cEnemy.setFaceDirection(false);
				cEnemy.setMinBounceVel(100.f); cEnemy.setMaxVel(400.f);

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
			ssvs::Ticker tckShoot{185.f};
			ssvu::Timeline tlShoot{false}, tlSummon{false}, tlCannon{true};
			OBWpn wpn{game, OBGroup::GFriendly, OBWpnTypes::createEPlasmaStarGun()};
			OBWpn wpnC{game, OBGroup::GFriendly, OBWpnTypes::createPlasmaCannon()};
			float lastDeg{0};

		public:
			OBCEGiant(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cKillable.setParticleMult(8);
				cKillable.onDeath += [this]{ assets.playSound("Sounds/alienDeath.wav"); };

				cEnemy.setMinBounceVel(10.f); cEnemy.setMaxVel(75.f);

				repeat(tlCannon, [this]{ shootCannon(0); }, -1, 100.f);

				repeat(tlShoot, [this]{ shoot(ssvu::getRnd(-15, 15)); }, 20, 0.4f);
				repeat(tlShoot, [this]{ game.createPCharge(5, cPhys.getPosPixels(), 65); }, 19, 1.f);
				tlShoot.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDeg(); });
				repeat(tlShoot, [this]{ shoot(lastDeg); lastDeg += 235; }, 150, 0.1f);

				tlSummon.append<ssvu::Do>([this]{ lastDeg = cEnemy.getCurrentDeg(); });
				repeat(tlSummon, [this]
				{
					game.createPCharge(5, cPhys.getPosPixels(), 65);
					body.setVelocity(body.getVelocity() * 0.8f);
					getFactory().createERunner(body.getPosition() + Vec2i(ssvs::getVecFromDeg<float>(lastDeg) * 1000.f), true);
					lastDeg += 360 / 6;
				}, 6, 4.5f);
				tlSummon.append<ssvu::Wait>(19.f);
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					if(ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF()) > 10000) cBoid.pursuit(cTargeter.getTarget()); else cBoid.evade(cTargeter.getTarget());

					tlCannon.update(mFT); tlShoot.update(mFT); tlSummon.update(mFT);
					if(tckShoot.update(mFT))
					{
						if(ssvu::getRnd(0, 2) > 0) { tlShoot.reset(); tlShoot.start(); }
						else { tlSummon.reset(); tlSummon.start(); }
					}
				}
			}
			inline void shoot(int mDeg)
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDeg<float>(cEnemy.getCurrentDeg()) * 100.f)};
				wpn.shoot(shootPos, cEnemy.getCurrentDeg() + mDeg);
				game.createPMuzzle(20, cPhys.getPosPixels());
			}
			inline void shootCannon(int mDeg)
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos1{body.getPosition() + Vec2i(ssvs::getVecFromDeg<float>(cEnemy.getSnappedDeg() + 40) * 1400.f)};
				Vec2i shootPos2{body.getPosition() + Vec2i(ssvs::getVecFromDeg<float>(cEnemy.getSnappedDeg() - 40) * 1400.f)};
				wpnC.shoot(shootPos1, cEnemy.getCurrentDeg() + mDeg);
				wpnC.shoot(shootPos2, cEnemy.getCurrentDeg() + mDeg);
				game.createPMuzzle(35, toPixels(shootPos1));
				game.createPMuzzle(35, toPixels(shootPos2));
			}
	};

	class OBCEEnforcer : public OBCEBase
	{
		private:
			ssvs::Ticker tckShoot{100.f};
			OBWpn wpn{game, OBGroup::GFriendly, OBWpnTypes::createPlasmaCannon()};

		public:
			OBCEEnforcer(OBCEnemy& mCEnemy) : OBCEBase{mCEnemy} { }

			inline void init() override
			{
				cKillable.setParticleMult(3);
				cEnemy.setMinBounceVel(45.f); cEnemy.setMaxVel(115.f);
				tckShoot.setLoop(false);
			}
			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					tckShoot.update(mFT);
					if(ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF()) > 10000) cBoid.pursuit(cTargeter.getTarget()); else cBoid.evade(cTargeter.getTarget());
					if(raycastToPlayer(cPhys, cTargeter.getTarget())) shootCannon(0);
				}
			}
			inline void shootCannon(int mDeg)
			{
				if(tckShoot.isRunning()) return;
				tckShoot.restart(100.f);

				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + Vec2i(ssvs::getVecFromDeg<float>(cEnemy.getSnappedDeg() - 40) * 700.f)};
				wpn.shoot(shootPos, cEnemy.getCurrentDeg() + mDeg);
				game.createPMuzzle(35, toPixels(shootPos));
			}
	};
}

#endif

