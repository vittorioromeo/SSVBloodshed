// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PROJECTILE
#define SSVOB_COMPONENTS_PROJECTILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCFloor.h"

namespace ob
{
	class OBCProjectile : public OBCActorBase
	{
		private:
			ssvs::Ticker life{150.f};
			float speed{125.f}, degrees{0.f}, curveSpeed{0.f};
			int pierceOrganic{0}, damage{1};
			OBGroup targetGroup{OBGroup::GEnemy};

			inline void destroy() { getEntity().destroy(); onDestroy(); }

		public:
			ssvu::Delegate<void()> onDestroy;

			OBCProjectile(OBCPhys& mCPhys, OBCDraw& mCDraw, float mSpeed, float mDegrees) : OBCActorBase{mCPhys, mCDraw}, speed{mSpeed}, degrees{mDegrees} { }

			inline void init() override
			{
				body.addGroup(OBGroup::GProjectile);
				body.addGroupToCheck(OBGroup::GSolid);
				body.setResolve(false);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(!mDI.body.hasGroup(targetGroup))
					{
						if(!mDI.body.hasGroup(OBGroup::GOrganic) && mDI.body.hasGroup(OBGroup::GSolid))
						{
							game.createPDebris(6, toPixels(body.getPosition()));
							assets.playSound("Sounds/bulletHitWall.wav");
							destroy();
						}
					}
					else
					{
						auto& e(*static_cast<Entity*>(mDI.body.getUserData()));
						if(e.getComponent<OBCHealth>().damage(damage))
						{
							if(pierceOrganic-- == 0) destroy();
						}
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				degrees += curveSpeed * mFrameTime;
				body.setVelocity(ssvs::getVecFromDegrees(degrees, speed));
				if(life.update(mFrameTime)) destroy();
			}
			inline void draw() override { cDraw.setRotation(degrees); }

			inline void setLife(float mValue) noexcept			{ life.restart(mValue); }
			inline void setSpeed(float mValue) noexcept			{ speed = mValue; }
			inline void setDegrees(float mValue) noexcept		{ degrees = mValue; }
			inline void setCurveSpeed(float mValue) noexcept	{ curveSpeed = mValue; }
			inline void setDamage(int mValue) noexcept			{ damage = mValue; }
			inline void setPierceOrganic(int mValue) noexcept	{ pierceOrganic = mValue; }
			inline void setTargetGroup(OBGroup mValue) noexcept	{ targetGroup = mValue; }

			inline float getSpeed() const noexcept				{ return speed; }
	};
}

#endif
