// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_WEAPONCONTROLLER
#define SSVOB_COMPONENTS_WEAPONCONTROLLER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCWielder.h"
#include "SSVBloodshed/Components/OBCProjectile.h"
#include "SSVBloodshed/Weapons/OBWpn.h"

namespace ob
{
	class OBCWpnController : public OBCActorNoDrawBase
	{
		private:
			OBWpn wpnType;
			ssvs::Ticker tckShoot{0.f};
			OBGroup targetGroup;

		public:
			OBCWpnController(OBCPhys& mCPhys, OBGroup mTargetGroup) : OBCActorNoDrawBase{mCPhys}, targetGroup{mTargetGroup} { }

			inline void init() override				{ tckShoot.setLoop(false); }
			inline void update(float mFT) override	{ tckShoot.update(mFT); }

			inline bool shoot(const Vec2i& mPos, float mDeg)
			{
				if(tckShoot.isRunning()) return false;
				assets.playSound(wpnType.getSoundId()); tckShoot.restart(wpnType.getDelay());
				wpnType.onShoot(*this, mPos, mDeg);
				return true;
			}

			inline OBCProjectile& shotProjectile(Entity& mEntity)
			{
				auto& cProjectile(mEntity.getComponent<OBCProjectile>());
				cProjectile.setTargetGroup(targetGroup);
				cProjectile.setDamage(wpnType.getPjDamage());
				cProjectile.setSpeed(wpnType.getPjSpeed());
				return cProjectile;
			}

			inline void setWpnType(const OBWpn& mWpnType) noexcept	{ wpnType = mWpnType; }

			inline OBWpn& getWpnType() noexcept					{ return wpnType; }
			inline const ssvs::Ticker& getTicker() const noexcept	{ return tckShoot; }
			inline ssvs::Ticker& getTicker() noexcept				{ return tckShoot; }
	};
}

#endif
