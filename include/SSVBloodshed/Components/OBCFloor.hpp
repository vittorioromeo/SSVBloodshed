// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_FLOOR
#define SSVOB_COMPONENTS_FLOOR

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"

namespace ob
{
	class OBCFloor : public OBCActorBase
	{
		private:
			bool smashed{false};

			inline void becomeGrate() noexcept
			{
				smashed = true;
				cDraw[0].setTextureRect(assets.getFloorGrateVariant());
				getEntity().setDrawPriority(OBLayer::LFloorGrate);
			}

		public:
			OBCFloor(OBCPhys& mCPhys, OBCDraw& mCDraw, bool mGrate) noexcept : OBCActorBase{mCPhys, mCDraw}, smashed{mGrate} { }

			inline void init()
			{
				body.addGroups(OBGroup::GFloor);
				body.setResolve(false);
				if(smashed) becomeGrate();
			}
			inline void smash() noexcept
			{
				if(smashed) return;
				smashed = true;
				game.createPDebris(20, cPhys.getPosPx());
				game.createPDebrisFloor(4, cPhys.getPosPx());
				becomeGrate();
			}
			inline bool isSmashed() const noexcept { return smashed; }
	};
}

#endif
