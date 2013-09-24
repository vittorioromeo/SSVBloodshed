// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PLAYER
#define SSVOB_COMPONENTS_PLAYER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCKillable.h"

namespace ob
{
	class OBCPlayer : public OBCActorBase
	{
		public:
			enum class Action{Idle, Shooting};

		private:
			OBCKillable& cKillable;
			Action action{Action::Idle};
			Direction direction{Direction::E};
			float walkSpeed{125.f};
			ssvs::Ticker shootTimer{4.7f};

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable) { }

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

				action = game.getInput().getIShoot() ? Action::Shooting : Action::Idle;

				const auto& ix(game.getInput().getIX());
				const auto& iy(game.getInput().getIY());
				const auto& iVec(ssvs::getNormalized(Vec2f(ix, iy)));

				if(action != Action::Shooting)
				{
					if(ix != 0 || iy != 0) direction = getDirectionFromXY(ix, iy);
				}
				else if(shootTimer.isStopped()) shoot();

				if(game.getInput().getIBomb()) bomb();

				body.setVelocity(iVec * walkSpeed);
			}
			inline void draw() override
			{
				// TODO: new component and merge with Gunner
				const auto& intDir(static_cast<int>(direction));
				cDraw.setRotation(45 * intDir);

				if(action == Action::Shooting)
				{
					cDraw.getOffsets()[1] = ssvs::getVecFromDegrees(getDegreesFromDirection(direction)) * 10.f;
					cDraw[0].setTextureRect(assets.p1Shoot);
					cDraw[1].setColor({255, 255, 255, 255});
					cDraw[1].setTextureRect(assets.p1Gun);
				}
				else
				{
					cDraw[0].setTextureRect(assets.p1Stand);
					cDraw[1].setColor({255, 255, 255, 0});
				}
			}

			inline void shoot()
			{
				Vec2i shootPosition{cPhys.getPosI() + Vec2i(getVecFromDirection<float>(direction) * 1000.f)};
				//Vec2i shootPosition(ssvs::getOrbitFromDegrees(cPhys.getPosF(), getDegreesFromDirection(direction), 1000.f));
				int weapon{0};

				switch(weapon)
				{
					case 0:
						assets.playSound("Sounds/machineGun.wav");
						getFactory().createProjectileBullet(shootPosition, getDegreesFromDirection(direction));
						shootTimer.restart(4.5f);
						break;
					case 1:
						getFactory().createProjectilePlasma(shootPosition, getDegreesFromDirection(direction));
						shootTimer.restart(9.5f);
						break;
					case 2:
						getFactory().createProjectileTestBomb(shootPosition, getDegreesFromDirection(direction));
						shootTimer.restart(25.f);
						break;
					case 3:
						getFactory().createProjectileTestShell(shootPosition, getDegreesFromDirection(direction) + 5.f);
						getFactory().createProjectileTestShell(shootPosition, getDegreesFromDirection(direction) + 2.5f);
						getFactory().createProjectileTestShell(shootPosition, getDegreesFromDirection(direction));
						getFactory().createProjectileTestShell(shootPosition, getDegreesFromDirection(direction) - 2.5f);
						getFactory().createProjectileTestShell(shootPosition, getDegreesFromDirection(direction) - 5.f);
						shootTimer.restart(16.f);
						break;
				}

				game.createPMuzzle(20, toPixels(shootPosition));
			}

			inline void bomb()
			{
				for(int k{0}; k < 5; ++k)
				{
					for(int i{0}; i < 360; i += 360 / 16) getFactory().createProjectileTestBomb(body.getPosition(), getDegreesFromDirection(direction) + (i * (360 / 16)), 2.f - k * 0.2f + i * 0.004f, 4.f + k * 0.3f - i * 0.004f);
				}
			}

			inline Action getAction() const noexcept		{ return action; }
			inline Direction getDirection() const noexcept	{ return direction; }
	};
}

#endif
