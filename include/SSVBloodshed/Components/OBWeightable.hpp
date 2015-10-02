// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEIGHTABLE
#define SSVOB_WEIGHTABLE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCPhys.hpp"

namespace ob
{
class OBWeightable
{
private:
    Body& bodyWeightable;
    bool wasWeighted{false}, weighted{false}, playerOnly;

public:
    OBWeightable(OBCPhys& mCPhys, bool mPlayerOnly) noexcept
    : bodyWeightable(mCPhys.getBody()),
      playerOnly{mPlayerOnly}
    {
        bodyWeightable.setResolve(false);
        bodyWeightable.addGroupsToCheck(OBGroup::GFriendly, OBGroup::GEnemy);

        bodyWeightable.onPreUpdate += [this]
        {
            weighted = false;
        };
        bodyWeightable.onDetection += [this](const DetectionInfo& mDI)
        {
            if(mDI.body.hasGroup(OBGroup::GFlying)) return;
            if((!playerOnly && mDI.body.hasGroup(OBGroup::GEnemy)) ||
               mDI.body.hasGroup(OBGroup::GFriendly))
                weighted = true;
        };
    }
    inline void refresh() { wasWeighted = weighted; }

    inline bool isWeighted() const noexcept { return weighted; }
    inline bool hasBeenWeighted() const noexcept
    {
        return !wasWeighted && weighted;
    }
    inline bool hasBeenUnweighted() const noexcept
    {
        return wasWeighted && !weighted;
    }
};
}

#endif
