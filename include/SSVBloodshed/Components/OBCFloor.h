// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_FLOOR
#define SSVOB_COMPONENTS_FLOOR

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCFloor : public OBCActorBase
	{
		private:
			bool smashed{false};

		public:
			OBCFloor(OBCPhys& mCPhys, OBCDraw& mCDraw) : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() override { cPhys.getBody().addGroup(OBGroup::OBGFloor); }
			inline void smash() noexcept
			{
				if(smashed) return;
				smashed = true;
				cDraw[0].setTextureRect(assets.tileset[{1, 1}]);
				game.createPDebris(20, toPixels(body.getPosition()));
				game.createPDebrisFloor(4, toPixels(body.getPosition()));
				getEntity().setDrawPriority(OBDrawPriority::OBDPFloorGrate);
			}
			inline bool isSmashed() const noexcept { return smashed; }
	};
}

#endif
