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
#include "SSVBloodshed/Components/OBCProjectile.h"
#include "SSVBloodshed/Components/OBCWpnController.h"
#include "SSVBloodshed/Weapons/OBWpnTypes.h"

namespace ob
{
	class OBCPlayer : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			OBCWielder& cWielder;
			OBCDir8& cDir8;
			OBCWpnController& cWpnController;
			float walkSpeed{125.f};

			int currentWpn{0};
			std::vector<OBWpnType> weaponTypes{OBWpnTypes::createMachineGun(), OBWpnTypes::createPlasmaBolter(), OBWpnTypes::createPlasmaCannon()};

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCWielder& mCWielder, OBCWpnController& mCWpnController)
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cWielder(mCWielder), cDir8(mCWielder.getCDir8()), cWpnController(mCWpnController) { }

			inline void init() override
			{
				cWpnController.setWpn(OBWpnTypes::createMachineGun());

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

			inline void updateInput()
			{
				const auto& ix(game.getInput().getIX());
				const auto& iy(game.getInput().getIY());
				body.setVelocity(ssvs::getResized(Vec2f(ix, iy), walkSpeed));

				cWielder.setShooting(game.getInput().getIShoot());
				if(!cWielder.isShooting() && (ix != 0 || iy != 0)) cDir8 = getDir8FromXY(ix, iy);

				if(game.getInput().getIBomb()) bomb();

				if(game.getInput().getISwitch()) cWpnController.setWpn(weaponTypes[++currentWpn % weaponTypes.size()]);
			}

			inline void update(float) override
			{
				updateInput();

				{ // TODO:
					auto& cHealth(getEntity().getComponent<OBCHealth>());
					game.testhp.setValue(cHealth.getHealth());
					game.testhp.setMaxValue(cHealth.getMaxHealth());
				}

				if(cWielder.isShooting())
					if(cWpnController.shoot(cWielder.getShootingPos(), cDir8.getDeg()))
						game.createPMuzzle(20, toPixels(cWielder.getShootingPos()));
			}
			inline void draw() override
			{
				cDraw[0].setRotation(cDir8.getDeg());
				cDraw[0].setTextureRect(cWielder.isShooting() ? assets.p1Shoot : assets.p1Stand);
			}

			inline void bomb()
			{
				for(int k{0}; k < 5; ++k)
					for(int i{0}; i < 360; i += 360 / 16) getFactory().createPJTestBomb(body.getPosition(), cDir8.getDeg() + (i * (360 / 16)), 2.f - k * 0.2f + i * 0.004f, 4.f + k * 0.3f - i * 0.004f);
			}
	};
}

#endif
