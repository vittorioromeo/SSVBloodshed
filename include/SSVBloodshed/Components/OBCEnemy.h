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
	class OBCEnemy : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			OBCTargeter& cTargeter;
			OBCBoid& cBoid;
			float currentDeg{0.f}, snappedDeg{0.f}, turnSpeed{5.f};
			bool faceDirection{true};
			float bodyDamage{1};

		public:
			OBCEnemy(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCTargeter& mCTargeter, OBCBoid& mCBoid) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cTargeter(mCTargeter), cBoid(mCBoid) { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GEnemy);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GEnemy, OBGroup::GOrganic);
				body.addGroupToCheck(OBGroup::GSolidGround);
				body.setRestitutionX(1.f);
				body.setRestitutionY(1.f);

				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::GFriendly)) getEntityFromBody(mDI.body).getComponent<OBCHealth>().damage(bodyDamage);
				};

				cKillable.onDeath += [this]{ for(int i{0}; i < 1; ++i) getFactory().createShard(cPhys.getPosI()); };
			}

			inline void update(float mFT) override
			{
				if(cTargeter.hasTarget())
				{
					float targetDeg(ssvs::getDegTowards(cPhys.getPosF(), cTargeter.getPosF()));
					currentDeg = ssvu::getRotatedDeg(currentDeg, targetDeg, turnSpeed * mFT);
					snappedDeg = getDegFromDir8(getDir8FromDeg(currentDeg));
				}
			}

			inline void draw() override { if(faceDirection) cDraw.setRotation(snappedDeg); }

			inline void setFaceDirection(bool mValue) noexcept	{ faceDirection = mValue; }
			inline void setMaxVelocity(float mMax) noexcept		{ cBoid.setMaxVelocity(mMax); }
			inline void setTurnSpeed(float mValue) noexcept		{ turnSpeed = mValue; }

			inline OBCKillable& getCKillable() const noexcept	{ return cKillable; }
			inline OBCBoid& getCBoid() const noexcept			{ return cBoid; }
			inline OBCTargeter& getCTargeter() const noexcept	{ return cTargeter; }
			inline float getCurrentDeg() const noexcept			{ return currentDeg; }
			inline float getSnappedDeg() const noexcept			{ return snappedDeg; }
			inline float getTurnSpeed() const noexcept			{ return turnSpeed; }
	};
}

#endif

