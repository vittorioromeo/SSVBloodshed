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
		public:
			struct Data
			{
				Vec2i pos;
				int currentWpn;
				float health;
				Dir8 dir;
				int shards;
				// Ammo and other stuff
			};

		private:
			OBCKillable& cKillable;
			OBCWielder& cWielder;
			OBCDir8& cDir8;
			OBCWpnController& cWpnController;
			float walkSpeed{125.f};

			int currentWpn{0}, currentShards{0}, shards{0};
			std::vector<OBWpnType> weaponTypes{OBWpnTypes::createMachineGun(), OBWpnTypes::createPlasmaBolter(), OBWpnTypes::createPlasmaCannon()};

			inline void cycleWeapons(int mDir) noexcept { currentWpn += mDir; cWpnController.setWpn(weaponTypes[currentWpn % weaponTypes.size()]); }

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCWielder& mCWielder, OBCWpnController& mCWpnController)
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cWielder(mCWielder), cDir8(mCWielder.getCDir8()), cWpnController(mCWpnController) { }

			inline void init() override
			{
				cKillable.getCHealth().setCooldown(2.5f);
				cycleWeapons(0);

				cKillable.onDeath += [this]
				{
					assets.playSound("Sounds/playerDeath.wav");
					game.testhp.setValue(0.f);
				};

				getEntity().addGroups(OBGroup::GFriendly, OBGroup::GPlayer);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GFriendly, OBGroup::GOrganic, OBGroup::GPlayer);
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

				if(game.getInput().getISwitch()) cycleWeapons(1);
			}

			inline void attractShards()
			{
				if(!game.isLevelClear())
				{
					auto query(cPhys.getWorld().getQuery<ssvsc::QueryType::Distance>(cPhys.getPosI(), 3500));

					Body* body;
					while((body = query.next()) != nullptr) if(body->hasGroup(OBGroup::GShard)) body->applyForce(Vec2f(cPhys.getPosI() - body->getPosition()) * 0.004f);
				}
				else
				{
					for(auto& e : getManager().getEntities(OBGroup::GShard))
					{
						auto& c(e->getComponent<OBCPhys>());
						if(ssvs::getMagnitude(c.getVel()) < 650.f) c.getBody().applyForce(Vec2f(cPhys.getPosI() - c.getPosI()) * 0.002f);
						c.getBody().addGroupNoResolve(OBGroup::GSolidGround);
					}
				}
			}

			inline void update(float) override
			{
				updateInput();

				{ // TODO:
					auto& cHealth(getEntity().getComponent<OBCHealth>());
					game.testhp.setValue(cHealth.getHealth());
					game.testhp.setMaxValue(cHealth.getMaxHealth());
				}

				{ // TODO:
					game.txtShards.setString(ssvu::toStr(shards + currentShards));
				}

				attractShards();
				if(game.isLevelClear()) { shards += currentShards; currentShards = 0; }

				{
					if(cPhys.getPosI().x < toCoords(0))		game.changeLevel(*this, -1, 0);
					if(cPhys.getPosI().x > toCoords(320))	game.changeLevel(*this, 1, 0);
					if(cPhys.getPosI().y < toCoords(0))		game.changeLevel(*this, 0, -1);
					if(cPhys.getPosI().y > toCoords(240))	game.changeLevel(*this, 0, 1);
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

			inline void shardGrabbed() noexcept { ++currentShards; }

			inline void initFromData(const Data& mData) noexcept
			{
				cPhys.setPos(mData.pos);
				currentWpn = mData.currentWpn; cycleWeapons(0);
				cKillable.getCHealth().setHealth(mData.health);
				cDir8.setDir(mData.dir);
				shards = mData.shards;
			}
			inline Data getData() const noexcept
			{
				Data result;
				result.pos = cPhys.getPosI();
				result.currentWpn = currentWpn;
				result.health = cKillable.getCHealth().getHealth();
				result.dir = cDir8;
				result.shards = shards;
				return result;
			}
	};

	class OBCShard : public OBCActorBase
	{
		public:
			OBCShard(OBCPhys& mCPhys, OBCDraw& mCDraw) : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GShard);
				body.addGroup(OBGroup::GShard);
				body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GFriendly);
				body.addGroupNoResolve(OBGroup::GOrganic);
				body.setRestitutionX(0.8f);
				body.setRestitutionY(0.8f);
				body.onPreUpdate += [this]{ body.setVelocity(ssvs::getCClampedMax(body.getVelocity() * 0.99f, 600.f)); };
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::GPlayer))
					{
						getEntityFromBody(mDI.body).getComponent<OBCPlayer>().shardGrabbed();
						getEntity().destroy();
						game.createPShard(20, cPhys.getPosPixels());
					}
				};

				body.setVelocity(ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(100.f, 370.f)));
				cDraw.setBlendMode(sf::BlendMode::BlendAdd);
				cDraw.setGlobalScale({0.65f, 0.65f});
			}

			inline void update(float) override
			{
				cDraw[0].rotate(ssvs::getMagnitude(body.getVelocity()) * 0.01f);
			}
	};
}

#endif

