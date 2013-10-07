// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPN
#define SSVOB_WEAPONS_WPN

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCProjectile.h"
#include "SSVBloodshed/Weapons/OBWpnType.h"

namespace ob
{
	class OBWpn
	{
		private:
			OBGame& game;
			OBWpnType wpnType;
			OBGroup targetGroup;

		public:
			inline OBWpn(OBGame& mGame, OBGroup mTargetGroup) noexcept : game(mGame), targetGroup{mTargetGroup} { }
			inline OBWpn(OBGame& mGame, OBGroup mTargetGroup, const OBWpnType& mWpn) noexcept : game(mGame), targetGroup{mTargetGroup} { setWpn(mWpn); }

			inline void shoot(const Vec2i& mPos, float mDeg)	{ wpnType.shoot(game, mPos, mDeg); }
			inline void playSound()								{ wpnType.playSound(game); }

			inline void setWpn(const OBWpnType& mWpnType) noexcept
			{
				wpnType = mWpnType;
				wpnType.onShotProjectile += [this](OBCProjectile& mPj){ mPj.setTargetGroup(targetGroup); };
			}

			inline OBGame& getGame() noexcept					{ return game; }
			inline const OBWpnType& getWpnType() const noexcept	{ return wpnType; }
			inline OBWpnType& getWpnType() noexcept				{ return wpnType; }
			inline float getDelay() const noexcept				{ return wpnType.getDelay(); }
			inline float getPjDamage() const noexcept			{ return wpnType.getPjDamage(); }
			inline float getPjSpeed() const noexcept			{ return wpnType.getPjSpeed(); }
	};
}

#endif
