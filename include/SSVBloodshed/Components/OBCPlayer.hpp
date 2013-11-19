// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PLAYER
#define SSVOB_COMPONENTS_PLAYER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCHealth.hpp"
#include "SSVBloodshed/Components/OBCKillable.hpp"
#include "SSVBloodshed/Components/OBCWielder.hpp"
#include "SSVBloodshed/Components/OBCProjectile.hpp"
#include "SSVBloodshed/Components/OBCWpnController.hpp"
#include "SSVBloodshed/Weapons/OBWpnTypes.hpp"

namespace ob
{
	class OBCVMachine;

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
			OBCVMachine* currentVM{nullptr};

			int currentWpn{0}, currentShards{0}, shards{0};
			std::vector<OBWpnType> weaponTypes{OBWpnTypes::createMachineGun(), OBWpnTypes::createPlasmaBolter(), OBWpnTypes::createPlasmaCannon(), OBWpnTypes::createRocketLauncher(), OBWpnTypes::createGrenadeLauncher()};
			std::vector<sf::IntRect> weaponRects{assets.p1Gun, assets.e1Gun, assets.gunPCannon, assets.p2Gun, assets.p2Gun};
			std::vector<std::string> weaponNames{"machine gun", "plasma bolter", "plasma cannon", "rocket launcher", "grenade launcher"};

			inline void cycleWeapons(int mDir) noexcept
			{
				currentWpn = ssvu::getWrapIdx(currentWpn + mDir, weaponTypes.size());
				cWpnController.setWpn(weaponTypes[currentWpn]);
				cDraw[1].setTextureRect(weaponRects[currentWpn]);
			}
			inline void useVM();

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCWielder& mCWielder, OBCWpnController& mCWpnController) noexcept
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cWielder(mCWielder), cDir8(mCWielder.getCDir8()), cWpnController(mCWpnController) { }

			inline void init()
			{
				cWielder.setWieldDist(7.f);
				cKillable.getCHealth().setCooldown(2.5f);
				cycleWeapons(0);

				cKillable.onDeath += [this]{ assets.playSound("Sounds/playerDeath.wav"); game.testhp.setValue(0.f); };

				getEntity().addGroups(OBGroup::GFriendly, OBGroup::GFriendlyKillable, OBGroup::GPlayer);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GFriendly, OBGroup::GKillable, OBGroup::GFriendlyKillable, OBGroup::GOrganic, OBGroup::GPlayer);
				body.addGroupsToCheck(OBGroup::GSolidGround);
				body.onResolution += [this](const ResolutionInfo& mRI){ if(mRI.body.hasGroup(OBGroup::GLevelBound)) checkTransitions(); };
			}

			inline void updateInput()
			{
				const auto& ix(game.getInput().getIX());
				const auto& iy(game.getInput().getIY());

				//float forceX{body.getVelocity().x - walkSpeed * ix};
				//float forceY{body.getVelocity().y - walkSpeed * iy};
				//body.applyAccel(Vec2f{forceX, forceY});
				//body.setVelocity(ssvs::getResized(Vec2f(ix, iy), walkSpeed));

				Vec2f force{ssvs::getResized(Vec2f(ix, iy), walkSpeed) - body.getVelocity()};
				ssvs::mClampMax(force, walkSpeed / 2.f);
				body.applyAccel(force);

				cWielder.setShooting(game.getInput().getIShoot());
				if(!cWielder.isShooting() && (ix != 0 || iy != 0)) cDir8 = getDir8FromXY(ix, iy);

				if(game.getInput().getIBomb()) bomb();
				if(game.getInput().getISwitch())
				{
					// TODO: generalize to usable objects, like switches
					if(currentVM == nullptr) cycleWeapons(1);
					else useVM();
				}
			}

			inline void attractShards()
			{
				if(!game.isLevelClear())
				{
					auto query(cPhys.getWorld().getQuery<ssvsc::QueryType::Distance>(cPhys.getPosI(), 3500));

					Body* body;
					while((body = query.next()) != nullptr) if(body->hasGroup(OBGroup::GShard)) body->applyAccel(Vec2f(cPhys.getPosI() - body->getPosition()) * 0.004f);
				}
				else
				{
					for(auto& e : manager.getEntities(OBGroup::GShard))
					{
						auto& c(e->getComponent<OBCPhys>());
						c.getBody().addGroupsNoResolve(OBGroup::GSolidGround);
						if(ssvs::getDistEuclidean(c.getPosI(), cPhys.getPosI()) > 6500)
						{
							if(ssvs::getMag(c.getVel()) < 650.f) c.getBody().applyAccel(Vec2f(cPhys.getPosI() - c.getPosI()) * 0.002f);
						}
						else c.setVel(ssvs::getMClampedMax(Vec2f(cPhys.getPosI() - c.getPosI()) / 1.5f, 400.f));
					}
				}
			}

			void updateHUD();
			inline void checkTransitions()
			{
				if(cPhys.getLeft() + cPhys.getVel().x < 0)							game.changeLevel(*this, -1, 0);
				else if(cPhys.getRight() + cPhys.getVel().x > levelWidthCoords)		game.changeLevel(*this, 1, 0);
				else if(cPhys.getTop() + cPhys.getVel().y < 0)						game.changeLevel(*this, 0, -1);
				else if(cPhys.getBottom() + cPhys.getVel().y > levelHeightCoords)	game.changeLevel(*this, 0, 1);
			}

			inline void update(float) override
			{
				updateInput(); updateHUD(); attractShards();

				if(game.isLevelClear()) { shards += currentShards; currentShards = 0; }
				if(cWielder.isShooting()) cWpnController.shoot(cWielder.getShootingPos(), cDir8.getDeg());
			}
			inline void draw() override { cDraw[0].setRotation(cDir8.getDeg()); }

			inline void bomb()
			{
				for(int k{0}; k < 5; ++k)
					for(int i{0}; i < 360; i += 360 / 16) factory.createPJTestBomb(body.getPosition(), cDir8.getDeg() + (i * (360 / 16)), 2.f - k * 0.2f + i * 0.004f, 4.f + k * 0.3f - i * 0.004f);
			}

			inline void shardGrabbed() noexcept { ++currentShards; }

			void setCurrentVM(OBCVMachine* mVMachine);
			inline OBCVMachine* getCurrentVM() { return currentVM; }

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
}

#endif

