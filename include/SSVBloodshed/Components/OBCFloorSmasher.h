// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_FLOORSMASHER
#define SSVOB_COMPONENTS_FLOORSMASHER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"

namespace ob
{
	class OBCFloorSmasher : public sses::Component
	{
		private:
			OBCPhys& cPhys;
			Body& body;
			bool active{false};

		public:
			OBCFloorSmasher(OBCPhys& mCPhys, bool mActive = false) noexcept : cPhys(mCPhys), body(cPhys.getBody()), active{mActive} { }

			inline void init() override
			{
				setActive(active);
				body.addGroupsNoResolve(OBGroup::GFloor);
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(active && mDI.body.hasGroup(OBGroup::GFloor) && ssvu::getRnd(0, 10) > 8) getEntityFromBody(mDI.body).getComponent<OBCFloor>().smash();
				};
			}

			inline void setActive(bool mValue) noexcept
			{
				active = mValue;
				if(mValue) body.addGroupsToCheck(OBGroup::GFloor);
				else body.delGroupsToCheck(OBGroup::GFloor);
			}
			inline bool isActive() const noexcept { return active; }
	};
}

#endif
