// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PROJECTILE
#define SSVOB_COMPONENTS_PROJECTILE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCFloor.hpp"
#include "SSVBloodshed/Components/OBCHealth.hpp"

namespace ob
{
	class OBCProjectile : public OBCActor
	{
		private:
			ssvs::Ticker tckLife{150.f};
			OBCActorND* shooter{nullptr};
			float acceleration{0.f}, minSpeed{0}, maxSpeed{1000}, dmgMult{1.f}, curveSpeed{0.f}, dmg{1};
			OBGroup targetGroup{OBGroup::GEnemyKillable};
			int pierceOrganic{0};
			bool bounce{false}, fallInPit{false}, killDestructible{false};

			inline void refreshMult()
			{
				if(targetGroup == OBGroup::GEnemyKillable) dmgMult = OBConfig::getDmgMultPlayer();
				else if(targetGroup == OBGroup::GFriendlyKillable) dmgMult = OBConfig::getDmgMultEnemy();
				else dmgMult = OBConfig::getDmgMultGlobal();
			}

		public:
			ssvu::Delegate<void()> onDestroy;

			inline OBCProjectile(OBCActorND* mShooter, OBCPhys& mCPhys, OBCDraw& mCDraw, float mSpeed, float mDeg) noexcept
				: OBCActor{mCPhys, mCDraw}, shooter{mShooter} { body.setVelocity(ssvs::getVecFromDeg(mDeg, mSpeed)); }

			inline void init()
			{
				body.addGroups(OBGroup::GProjectile);
				body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GSolidAir);
				body.addGroupsNoResolve(OBGroup::GFriendly, OBGroup::GEnemy, OBGroup::GProjectile);
				body.setResolve(false);
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(fallInPit && mDI.body.hasGroup(OBGroup::GPit)) getEntity().destroy();

					if(killDestructible && mDI.body.hasGroup(OBGroup::GEnvDestructible))
					{
						getComponentFromBody<OBCHealth>(mDI.body).damage(sses::getNullEntityStat(), nullptr, 100000);
						destroy();
					}

					auto shooterStat(shooter->getEntity().getStat());
					if(mDI.body.hasGroup(targetGroup) && getComponentFromBody<OBCHealth>(mDI.body).damage(shooterStat, shooter, dmg * dmgMult) && pierceOrganic-- == 0)
					{
						destroy();
					}
					else if(!mDI.body.hasGroup(OBGroup::GOrganic) && mDI.body.hasGroup(OBGroup::GSolidAir))
					{
						if(bounce) return;
						game.createPDebris(6, cPhys.getPosPx());
						assets.playSound("Sounds/bulletHitWall.wav");
						destroy();
					}
				};
				body.setRestitutionX(1.f);
				body.setRestitutionY(1.f);

				refreshMult();
			}
			inline void destroy() { getEntity().destroy(); onDestroy(); }

			inline void update(FT mFT) override
			{
				auto newVel(body.getVelocity());
				ssvs::resize(newVel, ssvs::getMag(newVel) + acceleration * mFT);
				ssvs::mClamp(newVel, minSpeed, maxSpeed);

				body.setVelocity(ssvs::getVecFromRad(ssvs::getRad(newVel) + curveSpeed * mFT, ssvs::getMag(newVel)));

				if(tckLife.update(mFT)) destroy();
			}
			inline void draw() override { cDraw.setRotation(ssvs::getDeg(body.getVelocity())); }

			inline OBCProjectile& createChild(Entity& mEntity)
			{
				auto& pj(mEntity.getComponent<OBCProjectile>());
				pj.setTargetGroup(targetGroup);
				return pj;
			}

			inline void setLife(float mValue) noexcept				{ tckLife.restart(mValue); }
			inline void setCurveSpeed(float mValue) noexcept		{ curveSpeed = mValue; }
			inline void setDamage(float mValue) noexcept			{ dmg = mValue; }
			inline void setPierceOrganic(int mValue) noexcept		{ pierceOrganic = mValue; }
			inline void setTargetGroup(OBGroup mValue) noexcept		{ targetGroup = mValue; refreshMult(); }
			inline void setKillDestructible(bool mValue) noexcept	{ killDestructible = mValue; }
			inline void setAcceleration(float mValue) noexcept		{ acceleration = mValue; }
			inline void setMinSpeed(float mValue) noexcept			{ minSpeed = mValue; }
			inline void setMaxSpeed(float mValue) noexcept			{ maxSpeed = mValue; }
			inline void setBounce(bool mValue) noexcept				{ bounce = mValue; body.setResolve(bounce); }
			inline void setSpeed(float mValue) noexcept				{ body.setVelocity(ssvs::getResized(body.getVelocity(), mValue)); }
			inline void setFallInPit(bool mValue) noexcept			{ fallInPit = mValue; }

			inline float getLife() const noexcept			{ return tckLife.getCurrent(); }
			inline float getCurveSpeed() const noexcept		{ return curveSpeed; }
			inline float getDamage() const noexcept			{ return dmg; }
			inline float getSpeed() const noexcept			{ return ssvs::getMag(body.getVelocity()); }
			inline int getPierceOrganic() const noexcept	{ return pierceOrganic; }
			inline OBGroup getTargetGroup() const noexcept	{ return targetGroup; }
			inline float getRad() const noexcept			{ return ssvs::getRad(body.getVelocity()); }
			inline float getDeg() const noexcept			{ return ssvs::getDeg(body.getVelocity()); }
	};
}

#endif
