// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PLAYER
#define SSVOB_COMPONENTS_PLAYER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCWielder.h"

namespace ob
{
	class OBCPlayer : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			OBCWielder& cWielder;
			OBCDirection8& cDirection8;
			float walkSpeed{125.f};
			ssvs::Ticker shootTimer{4.7f};

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCWielder& mCWielder) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cWielder(mCWielder), cDirection8(mCWielder.getCDirection()) { }

			inline void init() override
			{
				cKillable.onDeath += [this]{ assets.playSound("Sounds/playerDeath.wav"); };

				getEntity().addGroup(OBGroup::GFriendly);
				body.addGroup(OBGroup::GSolid);
				body.addGroup(OBGroup::GFriendly);
				body.addGroup(OBGroup::GOrganic);
				body.addGroupToCheck(OBGroup::GSolid);
			}
			inline void update(float mFrameTime) override
			{
				if(shootTimer.update(mFrameTime)) shootTimer.stop();

				cWielder.setShooting(game.getInput().getIShoot());

				const auto& ix(game.getInput().getIX());
				const auto& iy(game.getInput().getIY());
				const auto& iVec(ssvs::getNormalized(Vec2f(ix, iy)));

				if(!cWielder.isShooting())
				{
					if(ix != 0 || iy != 0) cDirection8 = getDirectionFromXY(ix, iy);
				}
				else if(shootTimer.isStopped()) shoot();

				if(game.getInput().getIBomb()) bomb();

				body.setVelocity(iVec * walkSpeed);
			}
			inline void draw() override
			{
				cDraw[0].setRotation(45 * cDirection8.getInt());
				cDraw[0].setTextureRect(cWielder.isShooting() ? assets.p1Shoot : assets.p1Stand);
			}

			inline void shoot()
			{
				int weapon{0};

				switch(weapon)
				{
					case 0:
						assets.playSound("Sounds/machineGun.wav");
						getFactory().createProjectileBullet(cWielder.getShootingPos(), cDirection8.getDegrees());
						shootTimer.restart(4.5f);
						break;
					case 1:
						getFactory().createProjectilePlasma(cWielder.getShootingPos(), cDirection8.getDegrees());
						shootTimer.restart(9.5f);
						break;
					case 2:
						getFactory().createProjectileTestBomb(cWielder.getShootingPos(), cDirection8.getDegrees());
						shootTimer.restart(25.f);
						break;
					case 3:
						getFactory().createProjectileTestShell(cWielder.getShootingPos(), cDirection8.getDegrees() + 5.f);
						getFactory().createProjectileTestShell(cWielder.getShootingPos(), cDirection8.getDegrees() + 2.5f);
						getFactory().createProjectileTestShell(cWielder.getShootingPos(), cDirection8.getDegrees());
						getFactory().createProjectileTestShell(cWielder.getShootingPos(), cDirection8.getDegrees() - 2.5f);
						getFactory().createProjectileTestShell(cWielder.getShootingPos(), cDirection8.getDegrees() - 5.f);
						shootTimer.restart(16.f);
						break;
				}

				game.createPMuzzle(20, toPixels(cWielder.getShootingPos()));
			}

			inline void bomb()
			{
				for(int k{0}; k < 5; ++k)
				{
					for(int i{0}; i < 360; i += 360 / 16) getFactory().createProjectileTestBomb(body.getPosition(), cDirection8.getDegrees() + (i * (360 / 16)), 2.f - k * 0.2f + i * 0.004f, 4.f + k * 0.3f - i * 0.004f);
				}
			}
	};
}

#endif
