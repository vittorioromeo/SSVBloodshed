// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_BOID
#define SSVOB_COMPONENTS_BOID

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCHealth.h"
#include "SSVBloodshed/Components/OBCFloor.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCKillable.h"

namespace ob
{
	class OBCBoid : public OBCActorNoDrawBase
	{
		private:
			float maxVelocity{150.f};

			inline Vec2f getSeekForce(Vec2f mTarget, float mSlowRadius) const noexcept
			{
				mTarget -= cPhys.getPosF();
				float distance{ssvs::getMagnitude(mTarget)};
				ssvs::resize(mTarget, distance <= mSlowRadius ? maxVelocity * distance / mSlowRadius : maxVelocity);
				return mTarget - cPhys.getVel();
			}
			inline Vec2f getFleeForce(Vec2f mTarget) const noexcept
			{
				mTarget = cPhys.getPosF() - mTarget;
				ssvs::resize(mTarget, maxVelocity);
				return mTarget - cPhys.getVel();
			}

			inline Vec2f getPursuitForce(const Vec2f& mTargetPos, const Vec2f& mTargetVel, float mPredictionMult, float mSlowRadius) const noexcept
			{
				Vec2f distance{mTargetPos - cPhys.getPosF()};
				float prediction{ssvs::getMagnitude(distance) / maxVelocity};
				return getSeekForce(mTargetPos + mTargetVel * (prediction * mPredictionMult), mSlowRadius);
			}
			inline Vec2f getEvadeForce(const Vec2f& mTargetPos, const Vec2f& mTargetVel) const noexcept
			{
				Vec2f distance{mTargetPos - cPhys.getPosF()};
				float prediction{ssvs::getMagnitude(distance) / maxVelocity};
				return getFleeForce(mTargetPos + mTargetVel * prediction);
			}

		public:
			OBCBoid(OBCPhys& mCPhys) : OBCActorNoDrawBase{mCPhys} { }

			inline void pursuit(const OBCPhys& mTarget, float mForceMult = 0.02f, float mPredictionMult = 1.f, float mSlowRadius = 1500.f) noexcept
			{
				body.applyForce(getPursuitForce(mTarget.getPosF(), mTarget.getVel(), mPredictionMult, mSlowRadius) * mForceMult);
			}
			inline void evade(const OBCPhys& mTarget, float mForceMult = 0.02f) noexcept
			{
				body.applyForce(getEvadeForce(mTarget.getPosF(), mTarget.getVel()) * mForceMult);
			}

			inline void setMaxVelocity(float mValue) noexcept { maxVelocity = mValue; }
	};
}

#endif

