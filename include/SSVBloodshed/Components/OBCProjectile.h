// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PROJECTILE
#define SSVOB_COMPONENTS_PROJECTILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCFloor.h"

namespace ob
{
	class OBCProjectile : public sses::Component
	{
		private:
			OBGame& game;
			OBCPhys& cPhys;
			OBCDraw& cDraw;
			ssvsc::Body& body;
			ssvs::Ticker life{150.f};
			float speed{125.f}, degrees{0.f}, curveSpeed{0.f};
			int pierceOrganic{0}, damage{1};
			bool destroyFloor{false};
			OBGroup targetGroup{OBGroup::Enemy};

			inline void destroy() { getEntity().destroy(); onDestroy(); }

		public:
			ssvu::Delegate<void()> onDestroy;

			OBCProjectile(OBGame& mGame, OBCPhys& mCPhys, OBCDraw& mCDraw, float mSpeed, float mDegrees) : game(mGame), cPhys(mCPhys), cDraw(mCDraw), body(cPhys.getBody()), speed{mSpeed}, degrees{mDegrees} { }

			inline void init() override
			{
				body.addGroup(OBGroup::Projectile);
				body.addGroupToCheck(OBGroup::Solid);
				body.addGroupToCheck(OBGroup::Floor);
				body.setResolve(false);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(!mDI.body.hasGroup(targetGroup))
					{
						if(!mDI.body.hasGroup(OBGroup::Organic) && mDI.body.hasGroup(OBGroup::Solid))
						{
							game.createPDebris(6, toPixels(body.getPosition()));
							destroy();
						}
					}
					else
					{
						auto& e(*static_cast<Entity*>(mDI.body.getUserData()));
						e.getComponent<OBCHealth>().damage(damage);

						if(pierceOrganic-- == 0) destroy();
					}

					if(destroyFloor && mDI.body.hasGroup(OBGroup::Floor))
					{
						auto& e(*static_cast<Entity*>(mDI.body.getUserData()));
						e.getComponent<OBCFloor>().smash();
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
			inline void setDestroyFloor(bool mValue) noexcept	{ destroyFloor = mValue; }
			inline void setTargetGroup(OBGroup mValue) noexcept	{ targetGroup = mValue; }
	};
}

#endif
