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

namespace ob
{
	class OBCTargeter : public OBCActorNoDrawBase
	{
		private:
			OBCPhys* target{nullptr};
			OBGroup targetGroup;
			sses::EntityStat targetStat;

		public:
			OBCTargeter(OBCPhys& mCPhys, OBGroup mTargetGroup) : OBCActorNoDrawBase{mCPhys}, targetGroup(mTargetGroup) { }

			inline void update(float) override
			{
				if(game.getManager().hasEntity(targetGroup))
				{
					targetStat = game.getManager().getEntities(targetGroup).front()->getStat();
					target = &game.getManager().getEntities(targetGroup).front()->getComponent<OBCPhys>();
				}

				if(!game.getManager().isAlive(targetStat)) target = nullptr;
			}

			inline bool hasTarget() const noexcept { return target != nullptr && game.getManager().isAlive(targetStat); }
			inline OBCPhys& getTarget() const noexcept { return *target; }
	};

	class OBCBoid : public OBCActorNoDrawBase
	{
		public:
			enum class State{Nothing, Pursuit, Wander};

		private:
			State state{State::Nothing};
			Vec2f targetPos, targetVel;
			float slowRadius{1500.f}, maxVelocity{150.f}, forceMult{0.02f};
			ssvs::Ticker wanderTimer{250.f};

			inline Vec2f seek(const Vec2f& mTargetPos) const noexcept
			{
				Vec2f desired{mTargetPos - cPhys.getPos<float>()};

				float distance{ssvs::getMagnitude(desired)};
				ssvs::normalize(desired);

				if(distance <= slowRadius) ssvs::resize(desired, maxVelocity * distance / slowRadius);
				else ssvs::resize(desired, maxVelocity);

				return desired - cPhys.getVel();
			}
			inline Vec2f pursuit() const noexcept
			{
				return seek(targetPos + ssvs::getResized(targetVel, ssvs::getMagnitude(targetPos - cPhys.getPos<float>()) / maxVelocity));
			}
			inline void randomWanderTarget() noexcept
			{
				ssvs::nullify(targetVel);
				targetPos = cPhys.getPos<float>() + Vec2f(ssvu::getRnd(-10000, 10000), ssvu::getRnd(-10000, 10000));
			}

		public:
			OBCBoid(OBCPhys& mCPhys) : OBCActorNoDrawBase{mCPhys} { }

			inline void update(float mFrameTime) override
			{
				switch(state)
				{
					case State::Nothing: break;
					case State::Pursuit:
						body.applyForce(pursuit() * forceMult);
						break;
					case State::Wander:
						body.applyForce(pursuit() * forceMult);
						if(wanderTimer.update(mFrameTime)) randomWanderTarget();
						break;
				}
			}

			inline void setState(State mState) noexcept				{ state = mState; }
			inline void setTarget(const OBCPhys& mTarget) noexcept	{ targetPos = mTarget.getPos<float>(); targetVel = mTarget.getVel(); }
			inline void setTargetPos(const Vec2f& mValue) noexcept	{ targetPos = mValue; }
			inline void setTargetVel(const Vec2f& mValue) noexcept	{ targetVel = mValue; }
			inline void setSlowRadius(float mValue) noexcept		{ slowRadius = mValue; }
			inline void setMaxVelocity(float mValue) noexcept		{ maxVelocity = mValue; }
			inline void setForceMult(float mValue) noexcept			{ forceMult = mValue; }

			inline State getState() const noexcept				{ return state; }
			inline const Vec2f& getTargetPos() const noexcept	{ return targetPos; }
			inline const Vec2f& getTargetVel() const noexcept	{ return targetVel; }
	};

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
					cBoid.setState(OBCBoid::State::Pursuit);
					cBoid.setTarget(cTargeter.getTarget());
					float targetDegrees(ssvs::getDegreesTowards(cPhys.getPos<float>(), cBoid.getTargetPos()));

					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;
				}
				else
				{
					cBoid.setState(OBCBoid::State::Wander);
					currentDegrees = ssvs::getDegrees(cPhys.getVel());
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;
				}

			}

			inline void draw() override { if(faceDirection) cDraw.setRotation(snappedDegrees); }

			inline void setFaceDirection(bool mValue) noexcept	{ faceDirection = mValue; }
			inline void setWalkSpeed(float mValue) noexcept		{ walkSpeed = mValue; }
			inline void setTurnSpeed(float mValue) noexcept		{ turnSpeed = mValue; }
			inline void setMaxVelocity(float mMax) noexcept		{ cBoid.setMaxVelocity(mMax); }

			inline OBCKillable& getCKillable() const noexcept	{ return cKillable; }
			inline float getCurrentDegrees() const noexcept		{ return currentDegrees; }
	};
}

#endif

