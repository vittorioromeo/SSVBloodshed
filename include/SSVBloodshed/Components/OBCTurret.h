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
#include "SSVBloodshed/Weapons/OBWpn.h"

namespace ob
{
	class OBCTurret : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			Dir8 direction;
			ssvs::Ticker tckShoot{125.f};
			ssvu::Timeline tlShoot{false};
			OBWpn wpn{game, OBGroup::GFriendly, OBWpnTypes::createEPlasmaStarGun(0)};

		public:
			OBCTurret(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, Dir8 mDir) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), direction{mDir} { }

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
				if(tckShoot.getCurrent() > 90.f && tckShoot.getCurrent() < 115.f) { game.createPCharge(1, cPhys.getPosPixels(), 20); }
				if(tckShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
			}
			inline void draw() override { cDraw.setRotation(getDegFromDir8(direction)); }

			inline void shoot()
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + getVecFromDir8<int>(direction) * 600};
				wpn.shoot(shootPos, getDegFromDir8(direction));
				game.createPMuzzle(20, cPhys.getPosPixels());
			}

			inline OBCKillable& getCKillable() const noexcept { return cKillable; }
	};
}

#endif

