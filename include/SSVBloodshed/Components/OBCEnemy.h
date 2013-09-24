// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMY
#define SSVOB_COMPONENTS_ENEMY

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCFloor.h"
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
			float walkSpeed{100.f}, currentDegrees{0.f}, turnSpeed{7.5f};
			float snappedDegrees{0.f};
			bool faceDirection{true};

		public:
			OBCEnemy(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCTargeter& mCTargeter, OBCBoid& mCBoid) : OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cTargeter(mCTargeter), cBoid(mCBoid) { }

			inline void init() override
			{
				getEntity().addGroup(OBGroup::GEnemy);
				body.addGroup(OBGroup::GSolid);
				body.addGroup(OBGroup::GEnemy);
				body.addGroup(OBGroup::GOrganic);
				body.addGroupToCheck(OBGroup::GSolid);
				body.setRestitutionX(0.9f);
				body.setRestitutionY(0.9f);

				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::GFriendly))
						static_cast<Entity*>(mDI.body.getUserData())->getComponent<OBCHealth>().damage(1);
				};
			}

			inline void update(float mFrameTime) override
			{
				if(cTargeter.hasTarget())
				{
					float targetDegrees(ssvs::getDegreesTowards(cPhys.getPos<float>(), cTargeter.getTarget().getPos<float>()));
					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;
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

