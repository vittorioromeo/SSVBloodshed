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
			bool smashed{false}, grate{false};

			inline void makeGrate()
			{
				grate = true;
				cDraw[0].setTextureRect(assets.getFloorGrateVariant());
				getEntity().setDrawPriority(OBLayer::LFloorGrate);
			}

		public:
			OBCFloor(OBCPhys& mCPhys, OBCDraw& mCDraw, bool mGrate) : OBCActorBase{mCPhys, mCDraw}, grate{mGrate} { }

			inline void init() override { body.addGroup(OBGroup::GFloor); if(grate) makeGrate(); }
			inline void smash() noexcept
			{
				if(smashed || grate) return;
				smashed = true;
				game.createPDebris(20, cPhys.getPosPixels());
				game.createPDebrisFloor(4, cPhys.getPosPixels());
				makeGrate();
			}
			inline bool isSmashed() const noexcept { return smashed; }
	};
}

#endif
