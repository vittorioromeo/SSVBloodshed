// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_FLOOR
#define SSVOB_COMPONENTS_FLOOR

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCFloor : public OBCActorBase
	{
		private:
			bool smashed{false};

		public:
			OBCFloor(OBCPhys& mCPhys, OBCDraw& mCDraw) : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() override { body.addGroup(OBGroup::GFloor); }
			inline void smash() noexcept
			{
				if(smashed) return;
				smashed = true;
				cDraw[0].setTextureRect(ssvu::getRnd(0, 10) < 9 ? assets.floorGrate : (ssvu::getRnd(0, 2) < 1 ? assets.floorGrateAlt1 : assets.floorGrateAlt2)); // TODO: assets.getRandomFloorGrateIntRectOrSomething
				game.createPDebris(20, toPixels(body.getPosition()));
				game.createPDebrisFloor(4, toPixels(body.getPosition()));
				getEntity().setDrawPriority(OBLayer::PFloorGrate);
			}
			inline bool isSmashed() const noexcept { return smashed; }
	};
}

#endif
