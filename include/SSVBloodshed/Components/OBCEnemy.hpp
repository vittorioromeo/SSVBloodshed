// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMY
#define SSVOB_COMPONENTS_ENEMY

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCKillable.hpp"
#include "SSVBloodshed/Components/OBCTargeter.hpp"
#include "SSVBloodshed/Components/OBCBoid.hpp"

namespace ob
{
    class OBCEnemy : public OBCActor
    {
    private:
        OBCKillable& cKillable;
        OBCTargeter& cTargeter;
        OBCBoid& cBoid;
        float currentDeg{0.f}, snappedDeg{0.f}, turnSpeed{5.f},
            minBounceVel{75.f}, targetDeg;
        bool faceDirection{true}, bounced{false};

    public:
        OBCEnemy(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw,
            OBCKillable& mCKillable, OBCTargeter& mCTargeter,
            OBCBoid& mCBoid) noexcept : OBCActor{mE, mCPhys, mCDraw},
                                        cKillable(mCKillable),
                                        cTargeter(mCTargeter),
                                        cBoid(mCBoid)
        {
            getEntity().addGroups(OBGroup::GEnemy, OBGroup::GEnemyKillable);

            cKillable.getCHealth().setCooldown(0.45f);

            body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir,
                OBGroup::GEnemy, OBGroup::GKillable, OBGroup::GEnemyKillable,
                OBGroup::GOrganic);
            body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GLevelBound);
            body.setRestitutionX(1.f);
            body.setRestitutionY(1.f);

            body.onPreUpdate += [this]
            {
                constexpr float maxVel{800.f};
                cPhys.setVel(ssvs::getMClamped(
                    cPhys.getVel(), bounced ? minBounceVel : 0.f, maxVel));
                bounced = false;
            };
            body.onResolution += [this](const ResolutionInfo&)
            {
                bounced = true;
            };

            cKillable.onDeath += [this]
            {
                game.createEShard(1 + cKillable.getCHealth().getMaxHealth() / 3,
                    cPhys.getPosI());
            };
        }

        inline void update(FT mFT) override
        {
            snappedDeg = ob::getSnappedDeg(currentDeg);

            if(!cTargeter.hasTarget()) return;
            targetDeg =
                ssvs::getDegTowards(cPhys.getPosF(), cTargeter.getPosF());
            turnTowards(mFT, targetDeg);
        }
        inline void draw() override
        {
            if(faceDirection) cDraw[0].setRotation(snappedDeg);
        }

        inline void turnTowards(FT mFT, float mTargetDeg) noexcept
        {
            currentDeg =
                ssvu::getRotatedDeg(currentDeg, mTargetDeg, turnSpeed * mFT);
        }

        inline void setFaceDirection(bool mValue) noexcept
        {
            faceDirection = mValue;
        }
        inline void setMinBounceVel(float mMin) noexcept
        {
            minBounceVel = mMin;
        }
        inline void setMaxVel(float mMax) noexcept { cBoid.setMaxVel(mMax); }
        inline void setTurnSpeed(float mValue) noexcept { turnSpeed = mValue; }

        inline OBCKillable& getCKillable() const noexcept { return cKillable; }
        inline OBCBoid& getCBoid() const noexcept { return cBoid; }
        inline OBCTargeter& getCTargeter() const noexcept { return cTargeter; }
        inline float getCurrentDeg() const noexcept { return currentDeg; }
        inline float getSnappedDeg() const noexcept { return snappedDeg; }
        inline float getTurnSpeed() const noexcept { return turnSpeed; }
        inline float getDegDiff() const noexcept
        {
            return ssvu::getDistDeg(currentDeg, targetDeg);
        }
    };
}

#endif
