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
			OBCTurret(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, Dir8 mDir, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount) noexcept
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), direction{mDir}, wpn{game, OBGroup::GFriendlyKillable, mWpn}, shootDelay{mShootDelay}, pjDelay{mPJDelay}, shootCount{mShootCount} { }

			inline void init()
			{
				cDraw.setRotation(getDegFromDir8(direction));

				getEntity().addGroups(OBGroup::GEnemy, OBGroup::GEnemyKillable);
				body.setResolve(false);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GEnemy, OBGroup::GKillable, OBGroup::GEnemyKillable);

				tckShoot.restart(shootDelay);
				repeat(tlShoot, [this]{ shoot(); }, shootCount, pjDelay);

				cKillable.onDeath += [this]{ game.createEShard(5, cPhys.getPosI()); };
			}
			inline void update(float mFT) override
			{
				tlShoot.update(mFT);
				if(tckShoot.getCurrent() > shootDelay / 1.5f && tckShoot.getCurrent() < shootDelay) game.createPCharge(1, cPhys.getPosPx(), 20);
				if(tckShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
			}

			inline void shoot()
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + getVecFromDir8<int>(direction) * 600};
				wpn.shoot(shootPos, getDegFromDir8(direction));
			}

			inline OBCKillable& getCKillable() const noexcept { return cKillable; }
	};

	class OBCForceField : public OBCActorBase
	{
		private:
			Dir8 dir;

		public:
			OBCForceField(OBCPhys& mCPhys, OBCDraw& mCDraw, Dir8 mDir) noexcept
				: OBCActorBase{mCPhys, mCDraw}, dir{mDir} { }

			inline void init()
			{
				cDraw.setRotation(getDegFromDir8(dir));
				getEntity().addGroups(OBGroup::GForceField);
				body.setResolve(false);
				body.addGroups(OBGroup::GForceField);
				body.addGroupsToCheck(OBGroup::GProjectile);

				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!mDI.body.hasGroup(OBGroup::GProjectile)) return;

					auto& cProjectile(getEntityFromBody(mDI.body).getComponent<OBCProjectile>());
					if(ssvu::getDiffDeg(cProjectile.getDeg(), getDegFromDir8(dir)) <= 50.f) cProjectile.destroy();
				};
			}
	};
}

#endif

