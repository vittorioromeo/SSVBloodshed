// Copyright (c) 2013-2014 Vittorio Romeo
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
#include "SSVBloodshed/Components/OBCUsable.hpp"
#include "SSVBloodshed/Weapons/OBWpnTypes.hpp"

namespace ob
{
	class OBCVMachine;

	class OBCPlayer : public OBCActor
	{
		friend class OBGameHUD;

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
			OBCUsable* currentUsable{nullptr};
			Vec2i validShootingPos;

			// TODO: refactor
			float comboTime{0.f}, comboTimeMax{100.f};
			int comboCount{0};

			struct WeaponData { OBWpnType wpn; sf::IntRect rect; std::string name; };
			int currentWpn{0}, currentShards{0}, shards{0};
			std::vector<WeaponData> weapons
			{
				{OBWpnTypes::createMachineGun(),		assets.p1Gun,			"machine gun"},
				{OBWpnTypes::createPlasmaBolter(),		assets.e1Gun,			"plasma bolter"},
				{OBWpnTypes::createPlasmaCannon(),		assets.gunPCannon,		"plasma cannon"},
				{OBWpnTypes::createRocketLauncher(),	assets.p2Gun,			"rocket launcher"},
				{OBWpnTypes::createGrenadeLauncher(),	assets.p2Gun,			"grenade launcher"},
				{OBWpnTypes::createShockwaveGun(),		assets.wpnShockwave,	"shockwave gun"}
			};

			inline void cycleWeapons(int mDir) noexcept
			{
				currentWpn = ssvu::getMod(currentWpn + mDir, weapons.size());
				const auto& wpnData(weapons[currentWpn]);

				cWpnController.setWpn(wpnData.wpn);
				cDraw[1].setTextureRect(wpnData.rect);
			}

		public:
			OBCPlayer(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCWielder& mCWielder, OBCWpnController& mCWpnController) noexcept
				: OBCActor{mCPhys, mCDraw}, cKillable(mCKillable), cWielder(mCWielder), cDir8(mCWielder.getCDir8()), cWpnController(mCWpnController) { }

			inline void init()
			{
				cWielder.setWieldDist(7.f);
				cKillable.getCHealth().setCooldown(2.6f);
				cycleWeapons(0);

				cKillable.onDeath += [this]{ assets.playSound("Sounds/playerDeath.wav"); game.playerDeath(*this); };

				getEntity().addGroups(OBGroup::GFriendly, OBGroup::GFriendlyKillable, OBGroup::GPlayer);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GFriendly, OBGroup::GKillable, OBGroup::GFriendlyKillable, OBGroup::GOrganic, OBGroup::GPlayer);
				body.addGroupsToCheck(OBGroup::GSolidGround);
				body.onResolution += [this](const ResolutionInfo& mRI){ if(mRI.body.hasGroup(OBGroup::GLevelBound)) checkTransitions(); };
			}

			inline void updateValidShootingPos()
			{
				auto gridQuery(getGame().getWorld().getQuery<ssvsc::QueryType::RayCast>(getCPhys().getPosI(), cDir8.getVec()));

				Body* b;
				while((b = gridQuery.next()) != nullptr)
				{
					if(b == &getCPhys().getBody()) continue;
					if(b->hasGroup(OBGroup::GSolidGround)) break;
				}

				auto rayDirVec(gridQuery.getLastPos() - getCPhys().getPosF());
				auto maxDist(toCoords(cWielder.getWieldDist()));
				ssvs::mClampMax(rayDirVec, maxDist);

				auto distDiff(maxDist - ssvs::getMag(rayDirVec) < 0.05f ? 0.f : 1.f);

				validShootingPos = getCPhys().getPosI() + ssvs::Vec2i(rayDirVec) - ssvs::Vec2i(rayDirVec * distDiff);
			}

			inline void updateInput()
			{
				const auto& ix(game.getInput().getIX());
				const auto& iy(game.getInput().getIY());

				auto accel(ssvs::getResized(Vec2f(ix, iy), walkSpeed) - body.getVelocity());
				body.applyAccel(ssvs::getMClampedMax(accel, walkSpeed / 2.f));

				cWielder.setShooting(game.getInput().getIShoot());
				if(!cWielder.isShooting() && (ix != 0 || iy != 0)) cDir8 = getDir8FromXY(ix, iy);

				if(game.getInput().getIBomb()) bomb();
				if(game.getInput().getISwitch())
				{
					if(currentUsable == nullptr) cycleWeapons(1);
					else currentUsable->onUse(*this);
				}
			}

			inline void updateUsable()
			{
				currentUsable = nullptr;
				for(auto& e : manager.getEntities(OBGroup::GUsable))
				{
					auto& usableCPhys(e->getComponent<OBCPhys>());
					if(ssvs::getDistSquaredEuclidean(usableCPhys.getPosI(), getCPhys().getPosI()) > 1300 * 1300) continue;

					currentUsable = &(e->getComponent<OBCUsable>());
					break;
				}
			}

			inline void attractShards()
			{
				if(!game.isLevelClear())
				{
					auto query(cPhys.getWorld().getQuery<ssvsc::QueryType::Distance>(cPhys.getPosI(), 3500));

					Body* b;
					while((b = query.next()) != nullptr) if(b->hasGroup(OBGroup::GShard)) b->applyAccel(Vec2f(cPhys.getPosI() - b->getPosition()) * 0.004f);
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

			inline void checkTransitions()
			{
				if(cPhys.getLeft() + cPhys.getVel().x < 0)							game.changeLevel(*this, -1, 0);
				else if(cPhys.getRight() + cPhys.getVel().x > levelWidthCoords)		game.changeLevel(*this, 1, 0);
				else if(cPhys.getTop() + cPhys.getVel().y < 0)						game.changeLevel(*this, 0, -1);
				else if(cPhys.getBottom() + cPhys.getVel().y > levelHeightCoords)	game.changeLevel(*this, 0, 1);
			}

			inline void update(FT mFT) override
			{
				updateValidShootingPos(); updateUsable(); updateInput(); attractShards(); updateCombo(mFT);
				game.refreshHUD(*this);

				if(game.isLevelClear()) { shards += currentShards; currentShards = 0; }
				if(cWielder.isShooting()) cWpnController.shoot(this, validShootingPos, cDir8.getDeg(), cWielder.getShootingPosPx());
			}
			inline void draw() override
			{
				cDraw[0].setRotation(cDir8.getDeg());
			}

			inline void bomb()
			{
				for(int k{0}; k < 5; ++k)
					for(int i{0}; i < 360; i += 360 / 16) factory.createPJTestBomb(this, body.getPosition(), cDir8.getDeg() + (i * (360 / 16)), 2.f - k * 0.2f + i * 0.004f, 4.f + k * 0.3f - i * 0.004f);
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

			inline void useVM(OBCVMachine& mVMachine);

			inline void updateCombo(FT mFT)
			{
				if(comboTime <= 0) comboCount = 0;
				else comboTime -= mFT * ssvu::getClampedMax(1 + comboCount * 0.05f, 1.5f);
			}
			inline void onKill(Entity& mEntity)
			{
				if(mEntity.hasGroup(OBGroup::GEnemy))
				{
					comboTime = comboTimeMax;
					++comboCount;
				}
			}

			inline OBCKillable& getCKillable() noexcept { return cKillable; }
			inline OBCHealth& getCHealth() noexcept { return cKillable.getCHealth(); }
	};
}

#include "SSVBloodshed/Components/OBCHealth.inl"

namespace ob
{
	inline void OBGameHUD::refresh(OBCPlayer& mP)
	{
		auto& cHealth(mP.getCHealth());
		testhp.setValue(cHealth.getHealth());
		testhp.setMaxValue(cHealth.getMaxHealth());
		txtShards.setString(ssvu::toStr(mP.shards + mP.currentShards));
		txtVM.setString(mP.currentUsable == nullptr ? mP.weapons[mP.currentWpn].name : mP.currentUsable->getMsg());

		txtCombo.setString(mP.comboCount > 3 ? "x" + ssvu::toStr(mP.comboCount) : "");
		txtCombo.setOrigin(ssvs::getGlobalHalfSize(txtCombo));
		ssvs::Vec2f offset(ssvu::getRndR<float>(-mP.comboCount, mP.comboCount), ssvu::getRndR<float>(-mP.comboCount, mP.comboCount));
		ssvs::cClamp(offset, -250.f, 250.f);
		txtCombo.setPosition(overlayCamera.getCenter() + offset / 10.f);

		auto c(txtCombo.getColor());
		auto k = ssvu::getMapEased<ssvu::Easing::Cubic, ssvu::Easing::InOut>(mP.comboTime, 0.f, mP.comboTimeMax, 0.f, 180.f);
		c.a = ssvu::getClamped(k + ssvu::getRndR<float>(0.f, mP.comboCount), 0.f, 255.f);

		txtCombo.setColor(c);
	}
}

#endif

