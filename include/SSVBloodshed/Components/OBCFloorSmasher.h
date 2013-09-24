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
			ssvsc::Body& body;
			bool active{false};

		public:
			OBCFloorSmasher(OBCPhys& mCPhys) : cPhys(mCPhys), body(cPhys.getBody()) { }

			inline void init() override
			{
				body.addGroupNoResolve(OBGroup::GFloor);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(active && mDI.body.hasGroup(OBGroup::GFloor) && ssvu::getRnd(0, 10) > 8) static_cast<Entity*>(mDI.body.getUserData())->getComponent<OBCFloor>().smash();
				};
			}

			inline void setActive(bool mValue) noexcept
			{
				active = mValue;

				if(mValue) body.addGroupToCheck(OBGroup::GFloor);
				else body.delGroupToCheck(OBGroup::GFloor);
			}
	};
}

#endif
