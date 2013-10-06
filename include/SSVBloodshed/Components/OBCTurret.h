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
			ssvs::Ticker tckShoot{0.f};
			ssvu::Timeline tlShoot{false};

			OBWpn wpn;
			float shootDelay, pjDelay;
			int shootCount;

		public:
			OBCTurret(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, Dir8 mDir, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount)
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), direction{mDir}, wpn{game, OBGroup::GFriendlyKillable, mWpn}, shootDelay{mShootDelay}, pjDelay{mPJDelay}, shootCount{mShootCount} { }

			inline void init() override
			{
				getEntity().addGroups(OBGroup::GEnemy, OBGroup::GEnemyKillable);
				body.setResolve(false);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GEnemy, OBGroup::GEnemyKillable);

				tckShoot.restart(shootDelay);
				repeat(tlShoot, [this]{ shoot(); }, shootCount, pjDelay);

				cKillable.onDeath += [this]{ game.createEShard(5, cPhys.getPosI()); };
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

