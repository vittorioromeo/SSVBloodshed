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
			float currentDeg{0.f}, snappedDeg{0.f}, turnSpeed{5.f}, minBounceVel{75.f}, targetDeg;
			bool faceDirection{true}, bounced{false};

		public:
			OBCEnemy(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, OBCTargeter& mCTargeter, OBCBoid& mCBoid) noexcept
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), cTargeter(mCTargeter), cBoid(mCBoid) { }

			inline void init()
			{
				getEntity().addGroups(OBGroup::GEnemy, OBGroup::GEnemyKillable);

				cKillable.getCHealth().setCooldown(0.45f);

				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GEnemy, OBGroup::GKillable, OBGroup::GEnemyKillable, OBGroup::GOrganic);
				body.addGroupsToCheck(OBGroup::GSolidGround, OBGroup::GLevelBound);
				body.setRestitutionX(1.f);
				body.setRestitutionY(1.f);

				body.onPreUpdate += [this]
				{
					constexpr float maxVel{800.f};
					cPhys.setVel(ssvs::getMClamped(cPhys.getVel(), bounced ? minBounceVel : 0.f, maxVel));
					bounced = false;
				};
				body.onResolution += [this](const ResolutionInfo&){ bounced = true; };

				cKillable.onDeath += [this]{ game.createEShard(1 + cKillable.getCHealth().getMaxHealth() / 3, cPhys.getPosI()); };
			}

			inline void update(float mFT) override
			{
				snappedDeg = getDegFromDir8(getDir8FromDeg(currentDeg));

				if(!cTargeter.hasTarget()) return;
				targetDeg = ssvs::getDegTowards(cPhys.getPosF(), cTargeter.getPosF());
				currentDeg = ssvu::getRotatedDeg(currentDeg, targetDeg, turnSpeed * mFT);
			}
			inline void draw() override { if(faceDirection) cDraw[0].setRotation(snappedDeg); }

			inline void setFaceDirection(bool mValue) noexcept	{ faceDirection = mValue; }
			inline void setMinBounceVel(float mMin) noexcept	{ minBounceVel = mMin; }
			inline void setMaxVel(float mMax) noexcept			{ cBoid.setMaxVel(mMax); }
			inline void setTurnSpeed(float mValue) noexcept		{ turnSpeed = mValue; }

			inline OBCKillable& getCKillable() const noexcept	{ return cKillable; }
			inline OBCBoid& getCBoid() const noexcept			{ return cBoid; }
			inline OBCTargeter& getCTargeter() const noexcept	{ return cTargeter; }
			inline float getCurrentDeg() const noexcept			{ return currentDeg; }
			inline float getSnappedDeg() const noexcept			{ return snappedDeg; }
			inline float getTurnSpeed() const noexcept			{ return turnSpeed; }
			inline float getDegDiff() const noexcept			{ return std::abs(ssvu::wrapDeg(currentDeg) - ssvu::wrapDeg(targetDeg)); }
	};
}

#endif

