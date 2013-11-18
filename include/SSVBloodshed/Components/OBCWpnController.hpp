// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_WPNCONTROLLER
#define SSVOB_COMPONENTS_WPNCONTROLLER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCHealth.hpp"
#include "SSVBloodshed/Components/OBCKillable.hpp"
#include "SSVBloodshed/Components/OBCWielder.hpp"
#include "SSVBloodshed/Weapons/OBWpnType.hpp"
#include "SSVBloodshed/Weapons/OBWpn.hpp"
#include "SSVBloodshed/Components/OBCProjectile.hpp"

namespace ob
{
	class OBCWpnController : public OBCActorNoDrawBase
	{
		private:
			OBWpn wpn;
			ssvs::Ticker tckShoot{0.f};

		public:
			OBCWpnController(OBCPhys& mCPhys, OBGroup mTargetGroup) noexcept : OBCActorNoDrawBase{mCPhys}, wpn{game, mTargetGroup} { }

			inline void init()						{ tckShoot.setLoop(false); }
			inline void update(float mFT) override	{ tckShoot.update(mFT); }

			inline bool shoot(const Vec2i& mPos, float mDeg)
			{
				if(tckShoot.isRunning()) return false;
				tckShoot.restart(wpn.getDelay());
				wpn.shoot(mPos, mDeg); wpn.playSound();
				return true;
			}

			inline void setWpn(OBWpnType mWpn) noexcept				{ wpn.setWpn(std::move(mWpn)); }

			inline OBWpnType& getWpn() noexcept						{ return wpn.getWpnType(); }
			inline const ssvs::Ticker& getTicker() const noexcept	{ return tckShoot; }
			inline ssvs::Ticker& getTicker() noexcept				{ return tckShoot; }
	};
}

#endif
