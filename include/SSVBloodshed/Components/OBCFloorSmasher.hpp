// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_FLOORSMASHER
#define SSVOB_COMPONENTS_FLOORSMASHER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCPhys.hpp"

namespace ob
{
    class OBCFloorSmasher : public Component
    {
    private:
        OBCPhys& cPhys;
        Body& body;
        bool active{false};

    public:
        OBCFloorSmasher(Entity& mE, OBCPhys& mCPhys,
            bool mActive = false) noexcept : Component{mE},
                                             cPhys(mCPhys),
                                             body(cPhys.getBody()),
                                             active{mActive}
        {
            setActive(active);
            body.addGroupsNoResolve(OBGroup::GFloor);
            body.onDetection += [this](const DetectionInfo& mDI)
            {
                if(active && mDI.body.hasGroup(OBGroup::GFloor) &&
                    ssvu::getRndI(0, 10) > 8)
                    getComponentFromBody<OBCFloor>(mDI.body).smash();
            };
        }

        inline void setActive(bool mValue) noexcept
        {
            active = mValue;
            body.setGroupsToCheck(mValue, OBGroup::GFloor);
        }
        inline bool isActive() const noexcept { return active; }
    };
}

#endif
