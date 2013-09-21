// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PROJECTILE
#define SSVOB_COMPONENTS_PROJECTILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"

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
			float speed{125.f}, degrees{0.f};
			bool pierceOrganic{true};

		public:
			OBCProjectile(OBGame& mGame, OBCPhys& mCPhysics, OBCDraw& mCRender, float mSpeed, float mDegrees)
				: game(mGame), cPhys(mCPhysics), cDraw(mCRender), body(cPhys.getBody()), speed{mSpeed}, degrees{mDegrees} { }

			inline void init() override
			{
				body.setResolve(false);
				body.addGroup(OBGroup::Projectile);
				body.addGroupToCheck(OBGroup::Solid);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::Solid))
					{
						if(!mDI.body.hasGroup(OBGroup::Organic))
						{
							game.createPDebris(6, toPixels(body.getPosition()));
							getEntity().destroy();
						}
						else
						{
							if(!pierceOrganic) getEntity().destroy();
						}
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				body.setVelocity(ssvs::getVecFromDegrees(degrees, speed));
				if(life.update(mFrameTime)) getEntity().destroy();
			}
			inline void draw() override { cDraw.setRotation(degrees); }
	};
}

#endif
