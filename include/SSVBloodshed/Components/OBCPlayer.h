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
			int currentWeapon{0};

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCWielder& mCWielder) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cWielder(mCWielder), cDirection8(mCWielder.getCDirection()) { }

			inline void init() override
			{
				cKillable.onDeath += [this]
				{
					assets.playSound("Sounds/playerDeath.wav");
					game.testhp.setValue(0.f);
				};

				getEntity().addGroup(OBGroup::GFriendly);
				body.addGroup(OBGroup::GSolidGround);
				body.addGroup(OBGroup::GSolidAir);
				body.addGroup(OBGroup::GFriendly);
				body.addGroup(OBGroup::GOrganic);
				body.addGroupToCheck(OBGroup::GSolidGround);
			}
			inline void update(float mFT) override
			{
				{
					auto& cHealth(getEntity().getComponent<OBCHealth>());
					game.testhp.setValue(cHealth.getHealth());
					game.testhp.setMaxValue(cHealth.getMaxHealth());
				}

				if(shootTimer.update(mFT)) shootTimer.stop();

				cWielder.setShooting(game.getInput().getIShoot());

				const auto& ix(game.getInput().getIX());
				const auto& iy(game.getInput().getIY());
				const auto& iVec(ssvs::getNormalized(Vec2f(ix, iy)));

				if(!cWielder.isShooting())
				{
					if(ix != 0 || iy != 0) cDirection8 = getDirection8FromXY(ix, iy);
				}
				else if(shootTimer.isStopped()) shootGun();

				if(game.getInput().getIBomb()) bomb();
				if(game.getInput().getISwitch()) currentWeapon = (currentWeapon + 1) % 4;

				body.setVelocity(iVec * walkSpeed);
			}
			inline void draw() override
			{
				cDraw[0].setRotation(cDirection8.getDegrees());
				cDraw[0].setTextureRect(cWielder.isShooting() ? assets.p1Shoot : assets.p1Stand);
			}

			inline void shootGun()
			{
				switch(currentWeapon)
				{
					case 0:
						assets.playSound("Sounds/machineGun.wav"); shootTimer.restart(4.5f);
						getFactory().createPJBullet(cWielder.getShootingPos(), cDirection8.getDegrees());
						break;
					case 1:
						assets.playSound("Sounds/machineGun.wav"); shootTimer.restart(9.5f);
						getFactory().createPJPlasma(cWielder.getShootingPos(), cDirection8.getDegrees());
						break;
					case 2:
						assets.playSound("Sounds/machineGun.wav"); shootTimer.restart(16.f);
						getFactory().createPJTestBomb(cWielder.getShootingPos(), cDirection8.getDegrees());
						break;
					case 3:
						assets.playSound("Sounds/machineGun.wav"); shootTimer.restart(16.f);
						for(float i{-5}; i <= 5; i += 2.5f) getFactory().createPJTestShell(cWielder.getShootingPos(), cDirection8.getDegrees() + i);
						break;
				}

				game.createPMuzzle(20, toPixels(cWielder.getShootingPos()));
			}

			inline void bomb()
			{
				for(int k{0}; k < 5; ++k)
					for(int i{0}; i < 360; i += 360 / 16) getFactory().createPJTestBomb(body.getPosition(), cDirection8.getDegrees() + (i * (360 / 16)), 2.f - k * 0.2f + i * 0.004f, 4.f + k * 0.3f - i * 0.004f);
			}
	};
}

#endif
