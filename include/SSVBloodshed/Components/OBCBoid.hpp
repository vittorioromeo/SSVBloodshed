// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_BOID
#define SSVOB_COMPONENTS_BOID

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"

namespace ob
{
	class OBCBoid : public OBCActorNoDrawBase
	{
		private:
			float maxVel{150.f};

			inline Vec2f getSeekForce(Vec2f mTarget, float mSlowRadius) const noexcept
			{
				mTarget -= cPhys.getPosF();
				float distance{ssvs::getMag(mTarget)};
				ssvs::resize(mTarget, distance <= mSlowRadius ? maxVel * distance / mSlowRadius : maxVel);
				return mTarget - cPhys.getVel();
			}
			inline Vec2f getFleeForce(Vec2f mTarget) const noexcept
			{
				mTarget = cPhys.getPosF() - mTarget;
				ssvs::resize(mTarget, maxVel);
				return mTarget - cPhys.getVel();
			}

			inline Vec2f getPursuitForce(const Vec2f& mTargetPos, const Vec2f& mTargetVel, float mPredictionMult, float mSlowRadius) const noexcept
			{
				Vec2f distance{mTargetPos - cPhys.getPosF()};
				float prediction{ssvs::getMag(distance) / maxVel};
				return getSeekForce(mTargetPos + mTargetVel * (prediction * mPredictionMult), mSlowRadius);
			}
			inline Vec2f getEvadeForce(const Vec2f& mTargetPos, const Vec2f& mTargetVel) const noexcept
			{
				Vec2f distance{mTargetPos - cPhys.getPosF()};
				float prediction{ssvs::getMag(distance) / maxVel};
				return getFleeForce(mTargetPos + mTargetVel * prediction);
			}

		public:
			inline OBCBoid(OBCPhys& mCPhys) : OBCActorNoDrawBase{mCPhys} { }

			inline void seek(const Vec2f& mTargetPos, float mForceMult = 0.02f, float mSlowRadius = 1500.f) noexcept
			{
				body.applyForce(getSeekForce(mTargetPos, mSlowRadius) * mForceMult);
			}
			inline void pursuit(const OBCPhys& mTarget, float mForceMult = 0.02f, float mPredictionMult = 1.f, float mSlowRadius = 1500.f) noexcept
			{
				body.applyForce(getPursuitForce(mTarget.getPosF(), mTarget.getVel(), mPredictionMult, mSlowRadius) * mForceMult);
			}
			inline void evade(const OBCPhys& mTarget, float mForceMult = 0.02f) noexcept
			{
				body.applyForce(getEvadeForce(mTarget.getPosF(), mTarget.getVel()) * mForceMult);
			}

			inline void setMaxVel(float mValue) noexcept	{ maxVel = mValue; }
			inline float getMaxVel() const noexcept			{ return maxVel; }
	};
}

#endif

