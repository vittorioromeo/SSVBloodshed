// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMYTYPES
#define SSVOB_COMPONENTS_ENEMYTYPES

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCHealth.hpp"
#include "SSVBloodshed/Components/OBCFloor.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCEnemy.hpp"
#include "SSVBloodshed/Components/OBCForceField.hpp"
#include "SSVBloodshed/Components/OBCBulletForceField.hpp"
#include "SSVBloodshed/Components/OBCBooster.hpp"
#include "SSVBloodshed/Components/OBCFloorSmasher.hpp"
#include "SSVBloodshed/Components/OBCKillable.hpp"
#include "SSVBloodshed/Components/OBCWielder.hpp"
#include "SSVBloodshed/Components/OBCWpnController.hpp"
#include "SSVBloodshed/Components/OBCTurret.hpp"
#include "SSVBloodshed/Weapons/OBWpnTypes.hpp"

namespace ob
{
inline bool raycastToPlayer(OBCPhys& mSeeker, OBCPhys& mTarget,
bool mCheckBulletForceField, bool mCheckForceField)
{
    const auto& startPos(mSeeker.getPosI());
    Vec2f dir(mTarget.getPosI() - startPos);
    // direction =
    // Vec2f(getVecFromDir8(getDir8FromDeg(ssvs::getDeg(direction))));

    auto gridQuery(mSeeker.getWorld().getQuery<ssvsc::QueryType::RayCast>(
    startPos, ssvs::getNormalized(dir)));

    Body* body;
    while((body = gridQuery.next()) != nullptr) {
        // If the detected body is the seeker or a friend of the seeker,
        // continue
        if(body == &mSeeker.getBody() || body->hasGroup(OBGroup::GEnemy))
            continue;

        // If the detected body is a bullet force field, check if the angle is
        // valid
        if(mCheckBulletForceField && body->hasGroup(OBGroup::GBulletForceField))
        {
            auto& cBulletForceField(
            getComponentFromBody<OBCBulletForceField>(*body));
            if(cBulletForceField.isRadBlocked(ssvs::getRad(dir))) return false;
        }

        // If the detected body is a force field, check if the angle is valid
        if(mCheckForceField && body->hasGroup(OBGroup::GForceField)) {
            auto& cForceField(getComponentFromBody<OBCForceField>(*body));
            if(cForceField.isRadBlocked(ssvs::getRad(dir))) return false;
        }

        // If the detected body is a target, return true
        if(body->hasGroup(OBGroup::GFriendly)) return true;

        // If the detected body is an obstacle, return false
        if(body->hasGroup(OBGroup::GSolidAir)) return false;
    }

    return false;
}

class OBCEBase : public OBCActor
{
protected:
    OBCEnemy& cEnemy;
    OBCKillable& cKillable;
    OBCBoid& cBoid;
    OBCTargeter& cTargeter;
    OBCHealth& cHealth;

public:
    OBCEBase(Entity& mE, OBCEnemy& mCEnemy)
        : OBCActor{mE, mCEnemy.getCPhys(), mCEnemy.getCDraw()}, cEnemy(mCEnemy),
          cKillable(cEnemy.getCKillable()), cBoid(cEnemy.getCBoid()),
          cTargeter(cEnemy.getCTargeter()),
          cHealth(mCEnemy.getCKillable().getCHealth())
    {
    }

    inline bool isPlayerInSightRanged() const noexcept
    {
        return raycastToPlayer(cPhys, cTargeter.getTarget(), true, false);
    }
    inline bool isPlayerInSightMelee() const noexcept
    {
        return raycastToPlayer(cPhys, cTargeter.getTarget(), false, true);
    }
};

class OBCEArmedBase : public OBCEBase
{
protected:
    OBCWielder& cWielder;
    OBCDir8& cDir8;
    OBCWpnController& cWpnController;
    bool armed;
    int wpnHealth;

public:
    OBCEArmedBase(Entity& mE, OBCEnemy& mCEnemy, OBCWielder& mCWielder,
    OBCWpnController& mCWpnController, bool mArmed, int mWpnHealth)
        : OBCEBase{mE, mCEnemy}, cWielder(mCWielder),
          cDir8(mCWielder.getCDir8()), cWpnController(mCWpnController),
          armed{mArmed}, wpnHealth(mWpnHealth)
    {
        cHealth.onDamage += [this](OBCActorND*)
        {
            if(armed && wpnHealth-- <= 0) {
                armed = false;
                game.createPElectric(10, toPixels(cPhys.getPosF()));
            }
        };
    }

    inline void pursuitOrAlign(float mDist, float mPursuitDist)
    {
        if(mDist > mPursuitDist)
            cBoid.pursuit(cTargeter.getTarget());
        else
            cBoid.seek(ssvs::getOrbitDeg(
                       cTargeter.getPosF(), cDir8.getDeg() + 180, mPursuitDist),
            0.02f, 750.f);
    }
    inline void recalculateTile()
    {
        if(!armed)
            cDraw[1].setColor(sf::Color::Transparent);
        else
            cDir8 = getDir8FromDeg(cEnemy.getCurrentDeg());
    }

    inline void shootGun()
    {
        cWpnController.shoot(this, cWielder.getShootingPos(), cDir8.getDeg(),
        cWielder.getShootingPosPx());
    }
};

class OBCERunner : public OBCEArmedBase
{
private:
    RunnerType type;

public:
    OBCERunner(Entity& mE, OBCEnemy& mCEnemy, OBCWielder& mCWielder,
    OBCWpnController& mCWpnController, RunnerType mType)
        : OBCEArmedBase{mE, mCEnemy, mCWielder, mCWpnController,
          mType != RunnerType::Unarmed, 1},
          type{mType}
    {
        cPhys.setMass(1.f);
        cWpnController.setWpn(OBWpnTypes::createEPlasmaBulletGun());
        cEnemy.setMinBounceVel(125.f);
        cEnemy.setMaxVel(200.f);
        cKillable.setType(OBCKillable::Type::Organic);
        cWielder.setHoldDist(2.f);
        cWielder.setWieldDist(8.f);
    }
    inline void update(FT) override
    {
        recalculateTile();

        if(!cTargeter.hasTarget()) {
            cWielder.setShooting(false);
            return;
        }

        cWielder.setShooting(
        armed && cEnemy.getDegDiff() < 50.f && isPlayerInSightRanged());

        if(cWielder.isShooting()) {
            pursuitOrAlign(cTargeter.getDist(), 9000.f);
            shootGun();
        }
        else
            cBoid.pursuit(cTargeter.getTarget());
    }
};

class OBCECharger : public OBCEArmedBase
{
private:
    OBCFloorSmasher& cFloorSmasher;
    Ticker tckCharge{250.f};
    ssvu::Timeline tlCharge{false};
    float lastDeg{0};
    ChargerType type;

public:
    OBCECharger(Entity& mE, OBCEnemy& mCEnemy, OBCFloorSmasher& mCFloorSmasher,
    OBCWielder& mCWielder, OBCWpnController& mCWpnController, ChargerType mType)
        : OBCEArmedBase{mE, mCEnemy, mCWielder, mCWpnController,
          mType != ChargerType::Unarmed, 7},
          cFloorSmasher(mCFloorSmasher), type{mType}
    {
        cPhys.setMass(100.f);

        cWpnController.setWpn(OBWpnTypes::createEPlasmaBulletGun(1, 8.f));
        if(type == ChargerType::GrenadeLauncher) {
            cWpnController.setWpn(OBWpnTypes::createGrenadeLauncher());
            cDraw[1].setTextureRect(assets.e2GunGL);
        }

        cEnemy.setMinBounceVel(20.f);
        cEnemy.setMaxVel(50.f);
        cWielder.setHoldDist(5.f);
        cWielder.setWieldDist(16.f);
        cKillable.setType(OBCKillable::Type::Organic);
        cKillable.setParticleMult(2);

        tlCharge.append<ssvu::WaitUntil>([this]
        {
            return !armed;
        });
        repeat(tlCharge,
        [this]
        {
            body.setVelocity(cPhys.getVel() * 0.8f);
            game.createPCharge(4, cPhys.getPosPx(), 45);
        },
        10, 2.5f);
        tlCharge.append<ssvu::WaitUntil>([this]
        {
            game.createPCharge(1, cPhys.getPosPx(), 45);
            return isPlayerInSightMelee();
        });
        tlCharge.append<ssvu::Do>([this]
        {
            cFloorSmasher.setActive(true);
            lastDeg = cEnemy.getCurrentDeg();
            body.applyAccel(ssvs::getVecFromDeg(lastDeg, 1250.f));
        });
        tlCharge.append<ssvu::Wait>(10.f);
        tlCharge.append<ssvu::Do>([this]
        {
            body.applyAccel(ssvs::getVecFromDeg(lastDeg, -150.f));
        });
        tlCharge.append<ssvu::Wait>(9.f);
        tlCharge.append<ssvu::Do>([this]
        {
            cFloorSmasher.setActive(false);
        });
    }
    inline void update(FT mFT) override
    {
        recalculateTile();

        if(!cTargeter.hasTarget()) {
            cWielder.setShooting(false);
            return;
        }

        cWielder.setShooting(
        armed && cEnemy.getDegDiff() < 50.f && isPlayerInSightRanged());

        if(cWielder.isShooting()) {
            pursuitOrAlign(cTargeter.getDist(), 9000.f);
            shootGun();
        }
        else
        {
            cBoid.pursuit(cTargeter.getTarget());
            tlCharge.update(mFT);
            if(tckCharge.update(mFT)) {
                tlCharge.reset();
                tlCharge.start();
            }
        }
    }
};

class OBCEJuggernaut : public OBCEArmedBase
{
private:
    Ticker tckShoot{150.f};
    ssvu::Timeline tlShoot{false};
    float lastDeg{0};
    OBWpn wpn{
    game, OBGroup::GFriendlyKillable, OBWpnTypes::createEPlasmaStarGun(0)};
    JuggernautType type;

public:
    OBCEJuggernaut(Entity& mE, OBCEnemy& mCEnemy, OBCWielder& mCWielder,
    OBCWpnController& mCWpnController, JuggernautType mType)
        : OBCEArmedBase{mE, mCEnemy, mCWielder, mCWpnController,
          mType != JuggernautType::Unarmed, 18},
          type{mType}
    {
        cPhys.setMass(10000.f);

        cWpnController.setWpn(OBWpnTypes::createEPlasmaBulletGun(2, 8.f));
        if(type == JuggernautType::RocketLauncher) {
            cWpnController.setWpn(OBWpnTypes::createRocketLauncher());
            cDraw[1].setTextureRect(assets.e3GunRL);
        }

        cEnemy.setMinBounceVel(15.f);
        cEnemy.setMaxVel(50.f);
        cKillable.setParticleMult(4);
        cKillable.onDeath += [this]
        {
            assets.playSound("Sounds/alienDeath.wav");
        };
        cKillable.setType(OBCKillable::Type::Organic);
        cKillable.setParticleMult(2);
        cWielder.setWieldDist(22.f);
        cWielder.setHoldDist(6.f);

        repeat(tlShoot,
        [this]
        {
            shootUnarmed(ssvu::getRndI(-10, 10));
        },
        8, 1.1f);
        repeat(tlShoot,
        [this]
        {
            game.createPCharge(4, cPhys.getPosPx(), 55);
        },
        15, 1.f);
        tlShoot.append<ssvu::Do>([this]
        {
            lastDeg = cEnemy.getCurrentDeg();
        });
        repeat(tlShoot,
        [this]
        {
            shootUnarmed(lastDeg);
            lastDeg += 265;
        },
        45, 0.3f);
    }
    inline void update(FT mFT) override
    {
        constexpr float distBodySlam{2750.f};
        constexpr float distEvade{10000.f};

        recalculateTile();

        if(!cTargeter.hasTarget()) {
            cWielder.setShooting(false);
            return;
        }

        const auto& dist(cTargeter.getDist());
        cWielder.setShooting(armed && cEnemy.getDegDiff() < 50.f &&
                             isPlayerInSightRanged() && dist > distBodySlam);

        if(cWielder.isShooting()) {
            if(dist < distBodySlam) cBoid.pursuit(cTargeter.getTarget());
            pursuitOrAlign(dist, distEvade - 1000.f);
            shootGun();
        }
        else if(!armed)
        {
            if(dist > distEvade || dist < distBodySlam)
                cBoid.pursuit(cTargeter.getTarget());
            else
                cBoid.evade(cTargeter.getTarget());

            tlShoot.update(mFT);
            if(tckShoot.update(mFT)) {
                tlShoot.reset();
                tlShoot.start();
            }
        }
    }

    inline void shootUnarmed(int mDeg)
    {
        assets.playSound("Sounds/spark.wav");
        game.createPMuzzleBullet(20, cPhys.getPosPx());
        wpn.shoot(
        this, cPhys.getPosI(), cEnemy.getCurrentDeg() + mDeg, cPhys.getPosPx());
    }
};

class OBCEBall : public OBCEBase
{
private:
    BallType type;
    bool small{false};

public:
    OBCEBall(Entity& mE, OBCEnemy& mCEnemy, BallType mType, bool mSmall)
        : OBCEBase{mE, mCEnemy}, type{mType}, small{mSmall}
    {
        cPhys.setMass(100.f);
        if(!small) {
            cKillable.onDeath += [this]
            {
                constexpr int splitCount{3};
                for(int i{0}; i < splitCount; ++i) {
                    auto& e(factory.createEBall(cPhys.getPosI(), type, true));
                    e.getComponent<OBCPhys>().setVel(
                    ssvs::getVecFromRad((ssvu::tau / splitCount) * i, 400.f));
                }
            };
        }
        else
            cKillable.setParticleMult(0.3f);

        cKillable.setType(OBCKillable::Type::Robotic);
        cEnemy.setFaceDirection(false);
        cEnemy.setMinBounceVel(100.f);
        cEnemy.setMaxVel(400.f);

        if(type == BallType::Flying) {
            body.addGroups(OBGroup::GFlying);
            body.onResolution += [this](const ResolutionInfo& mRI)
            {
                if(mRI.body.hasGroup(OBGroup::GSolidGround) &&
                   !mRI.body.hasGroup(OBGroup::GSolidAir))
                    mRI.noResolvePosition = mRI.noResolveVelocity = true;
            };
            body.addGroupsToCheck(OBGroup::GSolidAir);
        }
    }
    inline void update(FT mFT) override
    {
        if(type == BallType::Flying && !small && ssvu::getRndI(0, 9) > 7)
            game.createPElectric(1, cPhys.getPosPx());
        if(cTargeter.hasTarget()) cBoid.pursuit(cTargeter.getTarget());
        cDraw.rotate(15.f * mFT);
    }
};

class OBCEGiant : public OBCEBase
{
private:
    Ticker tckShoot{250.f};
    ssvu::Timeline tlShoot{false}, tlSummon{false}, tlCannon{true};
    OBWpn wpn{
    game, OBGroup::GFriendlyKillable, OBWpnTypes::createEPlasmaStarGun()};
    OBWpn wpnC{
    game, OBGroup::GFriendlyKillable, OBWpnTypes::createPlasmaCannon()};
    float lastDeg{0};

public:
    OBCEGiant(Entity& mE, OBCEnemy& mCEnemy) : OBCEBase{mE, mCEnemy}
    {
        cKillable.setParticleMult(8);
        cKillable.onDeath += [this]
        {
            assets.playSound("Sounds/alienDeath.wav");
        };

        cEnemy.setMinBounceVel(10.f);
        cEnemy.setMaxVel(75.f);

        repeat(tlCannon,
        [this]
        {
            shootCannon(0);
        },
        -1, 100.f);

        repeat(tlShoot,
        [this]
        {
            shoot(ssvu::getRndI(-15, 15));
        },
        20, 0.4f);
        repeat(tlShoot,
        [this]
        {
            game.createPCharge(5, cPhys.getPosPx(), 65);
        },
        19, 1.f);
        tlShoot.append<ssvu::Do>([this]
        {
            lastDeg = cEnemy.getCurrentDeg();
        });
        repeat(tlShoot,
        [this]
        {
            shoot(lastDeg);
            lastDeg += 235;
        },
        150, 0.1f);

        tlSummon.append<ssvu::Do>([this]
        {
            lastDeg = cEnemy.getCurrentDeg();
        });
        repeat(tlSummon,
        [this]
        {
            game.createPCharge(5, cPhys.getPosPx(), 65);
            body.setVelocity(body.getVelocity() * 0.8f);
            factory.createERunner(
            body.getPosition() +
            Vec2i(ssvs::getVecFromDeg<float>(lastDeg) * 1500.f),
            RunnerType::PlasmaBolter);
            lastDeg += 360 / 6;
        },
        6, 4.5f);
        tlSummon.append<ssvu::Wait>(19.f);
    }
    inline void update(FT mFT) override
    {
        if(!cTargeter.hasTarget()) return;

        if(ssvs::getDistEuclidean(cTargeter.getPosF(), cPhys.getPosF()) > 10000)
            cBoid.pursuit(cTargeter.getTarget());
        else
            cBoid.evade(cTargeter.getTarget());

        tlCannon.update(mFT);
        tlShoot.update(mFT);
        tlSummon.update(mFT);
        if(tckShoot.update(mFT)) {
            if(ssvu::getRndI(0, 2) > 0) {
                tlShoot.reset();
                tlShoot.start();
            }
            else
            {
                tlSummon.reset();
                tlSummon.start();
            }
        }
    }
    inline void shoot(int mDeg)
    {
        assets.playSound("Sounds/spark.wav");
        Vec2i shootPos{
        body.getPosition() +
        Vec2i(ssvs::getVecFromDeg<float>(cEnemy.getCurrentDeg()) * 100.f)};
        wpn.shoot(
        this, shootPos, cEnemy.getCurrentDeg() + mDeg, toPixels(shootPos));
        game.createPMuzzleBullet(20, cPhys.getPosPx());
    }
    inline void shootCannon(int mDeg)
    {
        assets.playSound("Sounds/spark.wav");
        Vec2i shootPos1{
        body.getPosition() +
        Vec2i(
        ssvs::getVecFromDeg<float>(cEnemy.getSnappedDeg() + 40) * 1400.f)};
        Vec2i shootPos2{
        body.getPosition() +
        Vec2i(
        ssvs::getVecFromDeg<float>(cEnemy.getSnappedDeg() - 40) * 1400.f)};
        wpnC.shoot(
        this, shootPos1, cEnemy.getCurrentDeg() + mDeg, toPixels(shootPos1));
        wpnC.shoot(
        this, shootPos2, cEnemy.getCurrentDeg() + mDeg, toPixels(shootPos1));
        game.createPMuzzleBullet(35, toPixels(shootPos1));
        game.createPMuzzleBullet(35, toPixels(shootPos2));
    }
};

class OBCEEnforcer : public OBCEBase
{
private:
    Ticker tckShoot{100.f};
    OBWpn wpn{
    game, OBGroup::GFriendlyKillable, OBWpnTypes::createPlasmaCannon()};

public:
    OBCEEnforcer(Entity& mE, OBCEnemy& mCEnemy) : OBCEBase{mE, mCEnemy}
    {
        cPhys.setMass(400.f);
        cKillable.setParticleMult(3);
        cEnemy.setMinBounceVel(45.f);
        cEnemy.setMaxVel(115.f);
        tckShoot.setLoop(false);
    }
    inline void update(FT mFT) override
    {
        if(!cTargeter.hasTarget()) return;

        tckShoot.update(mFT);
        if(cTargeter.getDist() > 9000.f)
            cBoid.pursuit(cTargeter.getTarget());
        else
            cBoid.evade(cTargeter.getTarget());
        if(isPlayerInSightRanged() && cEnemy.getDegDiff() < 50.f)
            shootCannon(0);
    }
    inline void shootCannon(int mDeg)
    {
        if(tckShoot.isRunning()) return;
        tckShoot.restart(100.f);

        assets.playSound("Sounds/spark.wav");
        Vec2i shootPos{
        body.getPosition() +
        Vec2i(ssvs::getVecFromDeg<float>(cEnemy.getSnappedDeg() - 40) * 700.f)};
        wpn.shoot(
        this, shootPos, cEnemy.getCurrentDeg() + mDeg, toPixels(shootPos));
        game.createPMuzzleBullet(35, toPixels(shootPos));
    }
};
}

#endif
