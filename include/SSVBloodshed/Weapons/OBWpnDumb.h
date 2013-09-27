// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPNDUMB
#define SSVOB_WEAPONS_WPNDUMB

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCProjectile.h"
#include "SSVBloodshed/Weapons/OBWpn.h"

namespace ob
{
	class OBWpnDumb
	{
		private:
			OBGame& game;
			OBWpn wpn;
			OBGroup targetGroup;

		public:
			inline OBWpnDumb(OBGame& mGame, OBGroup mTargetGroup) : game(mGame), targetGroup{mTargetGroup} { }
			inline OBWpnDumb(OBGame& mGame, OBGroup mTargetGroup, const OBWpn& mWpn) : game(mGame), targetGroup{mTargetGroup} { setWpn(mWpn); }

			inline void shoot(const Vec2i& mPos, float mDeg) { wpn.shoot(game, mPos, mDeg);  }

			inline void setWpn(const OBWpn& mWpn)
			{
				wpn = mWpn;
				wpn.onShotProjectile += [this](OBCProjectile& mPj){ mPj.setTargetGroup(targetGroup); };
			}
			inline OBWpn& getWpn() noexcept { return wpn; }
	};
}

#endif
