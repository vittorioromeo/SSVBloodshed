// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_TURRET
#define SSVOB_COMPONENTS_TURRET

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCWpnController.h"
#include "SSVBloodshed/Weapons/OBWpnTypes.h"
#include "SSVBloodshed/Weapons/OBWpnDumb.h"

namespace ob
{
	class OBCTurret : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			Direction8 direction;
			ssvs::Ticker timerShoot{125.f};
			ssvu::Timeline tlShoot{false};
			OBWpnDumb wpn{game, OBGroup::GFriendly, OBWpnTypes::createEPlasmaStarGun(0)};

		public:
			OBCTurret(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, Direction8 mDirection) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), direction{mDirection} { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GEnemy);
				body.setResolve(false);
				body.addGroup(OBGroup::GSolidGround);
				body.addGroup(OBGroup::GSolidAir);
				body.addGroup(OBGroup::GEnemy);

				repeat(tlShoot, [this]{ shoot(); }, 3, 5.2f);
			}
			inline void update(float mFT) override
			{
				tlShoot.update(mFT);
				if(timerShoot.getCurrent() > 90.f && timerShoot.getCurrent() < 115.f) { game.createPCharge(1, cPhys.getPosPixels(), 20); }
				if(timerShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
			}
			inline void draw() override { cDraw.setRotation(getDegreesFromDirection8(direction)); }

			inline void shoot()
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + getVecFromDirection8<int>(direction) * 600};
				wpn.shoot(shootPos, getDegreesFromDirection8(direction));
				game.createPMuzzle(20, cPhys.getPosPixels());
			}

			inline OBCKillable& getCKillable() const noexcept { return cKillable; }
	};
}

#endif

