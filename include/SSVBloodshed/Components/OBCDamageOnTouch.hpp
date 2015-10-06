// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DAMAGEONTOUCH
#define SSVOB_COMPONENTS_DAMAGEONTOUCH

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"

namespace ob
{
    class OBCDamageOnTouch : public OBCActorND
    {
    private:
        float dmg;
        OBGroup targetGroup;

    public:
        OBCDamageOnTouch(Entity& mE, OBCPhys& mCPhys, float mDamage,
            OBGroup mTargetGroup) noexcept : OBCActorND{mE, mCPhys},
                                             dmg{mDamage},
                                             targetGroup{mTargetGroup}
        {
            body.addGroupsToCheck(targetGroup);
            body.onDetection += [this](const DetectionInfo& mDI)
            {
                auto thisStat(getEntity().getStat());
                if(mDI.body.hasGroup(targetGroup) &&
                    !mDI.body.hasGroup(OBGroup::GEnvDestructible))
                    getComponentFromBody<OBCHealth>(mDI.body).damage(
                        thisStat, this, dmg);
            };
        }

        inline void setDamage(float mValue) noexcept { dmg = mValue; }
        inline void setTargetGroup(OBGroup mValue) noexcept
        {
            targetGroup = mValue;
        }

        inline float getDamage() const noexcept { return dmg; }
        inline OBGroup getTargetGroup() const noexcept { return targetGroup; }
    };
}

#endif
