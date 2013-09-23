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

namespace ob
{
	class OBCEnemy : public OBCActorBase
	{
		private:
			OBCHealth& cHealth;
			float walkSpeed{100.f}, currentDegrees{0.f}, turnSpeed{7.5f};
			float snappedDegrees{0.f}, maxVelocity{120.f};
			int gibMult{1};

		public:
			OBCEnemy(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCHealth& mCHealth) : OBCActorBase{mCPhys, mCDraw}, cHealth(mCHealth) { }

			inline void init() override
			{
				cHealth.onDamage += [this]
				{
					game.createPBlood(6 * gibMult, toPixels(body.getPosition()));
					if(cHealth.isDead())
					{
						game.createPBlood(20 * gibMult * gibMult, toPixels(body.getPosition()), gibMult);
						game.createPGib(35 * gibMult * gibMult, toPixels(body.getPosition()));
						getEntity().destroy();
					}
				};

				getEntity().addGroup(OBGroup::GEnemy);
				body.addGroup(OBGroup::GSolid);
				body.addGroup(OBGroup::GEnemy);
				body.addGroup(OBGroup::GOrganic);
				body.addGroupToCheck(OBGroup::GSolid);
				body.setRestitutionX(0.9f);
				body.setRestitutionY(0.9f);
				//body.onPreUpdate += [this]{ body.setVelocity(ssvs::getMClamped(body.getVelocity(), -120.f, 120.f)); };
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::GFriendly))
					{
						auto& e(*static_cast<Entity*>(mDI.body.getUserData()));
						e.getComponent<OBCHealth>().damage(1);
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				for(const auto& e : game.getManager().getEntities(OBGroup::GPlayer))
				{
					auto& ecPhys(e->getComponent<OBCPhys>());
					float targetDegrees(ssvs::getDegreesTowards(Vec2f(body.getPosition()), Vec2f(ecPhys.getBody().getPosition())));

					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;
				}

				if(ssvs::getMagnitude(body.getVelocity()) < maxVelocity) body.applyForce(ssvs::getVecFromDegrees(snappedDegrees, walkSpeed) * 0.05f);
				else body.setVelocity(body.getVelocity() * 0.99f);
			}

			inline void draw() override
			{
				auto& s0(cDraw[0]);
				//s0.setTextureRect(assets.get<Tileset>("tileset")GEnemy[{0, 0}]);
				s0.setRotation(snappedDegrees);
			}

			inline void setWalkSpeed(float mValue) noexcept	{ walkSpeed = mValue; }
			inline void setTurnSpeed(float mValue) noexcept	{ turnSpeed = mValue; }
			inline void setGibMult(int mValue) noexcept		{ gibMult = mValue; }
			inline void setMaxVelocity(float mMax) noexcept	{ maxVelocity = mMax; }

			inline OBCHealth& getCHealth() const noexcept	{ return cHealth; }
			inline float getCurrentDegrees() const noexcept	{ return currentDegrees; }
	};
}

#endif
