// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMY
#define SSVOB_COMPONENTS_ENEMY

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCTargeter.h"
#include "SSVBloodshed/Components/OBCBoid.h"

namespace ob
{
	class OBCTurret : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			Direction8 direction;
			ssvs::Ticker timerShoot{125.f};
			ssvu::Timeline tlShoot{false};

		public:
			OBCTurret(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, Direction8 mDirection) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), direction{mDirection} { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GEnemy);
				body.setResolve(false);
				body.addGroup(OBGroup::GSolidGround);
				body.addGroup(OBGroup::GSolidAir);
				body.addGroup(OBGroup::GEnemy);

				repeat(tlShoot, [this]{ shoot(); }, 3, 5.2f);
			}
			inline void update(float mFT) override
			{
				tlShoot.update(mFT);
				if(timerShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
			}
			inline void draw() override { cDraw.setRotation(getDegreesFromDirection8(direction)); }

			inline void shoot()
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + getVecFromDirection8<int>(direction) * 600};
				getFactory().createPJEnemyStar(shootPos, getDegreesFromDirection8(direction));
				game.createPMuzzle(20, cPhys.getPosPixels());
			}

			inline OBCKillable& getCKillable() const noexcept { return cKillable; }
	};

	class OBCEnemy : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			OBCTargeter& cTargeter;
			OBCBoid& cBoid;
			float walkSpeed{100.f}, currentDegrees{0.f}, turnSpeed{7.5f}, snappedDegrees{0.f};
			bool faceDirection{true};
			float bodyDamage{1};

		public:
			OBCEnemy(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCTargeter& mCTargeter, OBCBoid& mCBoid) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cTargeter(mCTargeter), cBoid(mCBoid) { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GEnemy);
				body.addGroup(OBGroup::GSolidGround);
				body.addGroup(OBGroup::GSolidAir);
				body.addGroup(OBGroup::GEnemy);
				body.addGroup(OBGroup::GOrganic);
				body.addGroupToCheck(OBGroup::GSolidGround);
				body.setRestitutionX(0.9f);
				body.setRestitutionY(0.9f);

				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::GFriendly)) getEntityFromBody(mDI.body).getComponent<OBCHealth>().damage(bodyDamage);
				};
			}

			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					float targetDegrees(ssvs::getDegreesTowards(cPhys.getPosF(), cTargeter.getPosF()));
					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFT);
					snappedDegrees = getDegreesFromDirection8(getDirection8FromDegrees(currentDegrees));
				}
			}

			inline void draw() override { if(faceDirection) cDraw.setRotation(snappedDegrees); }

			inline void setFaceDirection(bool mValue) noexcept	{ faceDirection = mValue; }
			inline void setWalkSpeed(float mValue) noexcept		{ walkSpeed = mValue; }
			inline void setTurnSpeed(float mValue) noexcept		{ turnSpeed = mValue; }
			inline void setMaxVelocity(float mMax) noexcept		{ cBoid.setMaxVelocity(mMax); }

			inline OBCKillable& getCKillable() const noexcept	{ return cKillable; }
			inline OBCBoid& getCBoid() const noexcept			{ return cBoid; }
			inline OBCTargeter& getCTargeter() const noexcept	{ return cTargeter; }
			inline float getCurrentDegrees() const noexcept		{ return currentDegrees; }
			inline float getSnappedDegrees() const noexcept		{ return snappedDegrees; }
	};
}

#endif

