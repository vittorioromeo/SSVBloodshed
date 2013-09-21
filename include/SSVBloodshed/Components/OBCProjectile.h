// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PROJECTILE
#define SSVOB_COMPONENTS_PROJECTILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhysics.h"
#include "SSVBloodshed/Components/OBCRender.h"

namespace ob
{
	class OBCProjectile : public sses::Component
	{
		private:
			OBGame& game;
			OBCPhysics& cPhysics;
			OBCRender& cRender;
			ssvsc::Body& body;
			float speed{125.f}, degrees{0.f};

		public:
			OBCProjectile(OBGame& mGame, OBCPhysics& mCPhysics, OBCRender& mCRender, float mSpeed, float mDegrees)
				: game(mGame), cPhysics(mCPhysics), cRender(mCRender), body(cPhysics.getBody()), speed{mSpeed}, degrees{mDegrees} { }

			inline void init() override
			{
				body.setResolve(false);
				body.addGroup(OBGroup::Projectile);
				body.addGroupToCheck(OBGroup::Solid);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::Solid))
					{
						if(!mDI.body.hasGroup(OBGroup::Organic)) for(int i = 0; i < 6; ++i) game.getPSTemp().createDebris(toPixels(body.getPosition()));
						getEntity().destroy();
					}
				};
			}
			inline void update(float) override { body.setVelocity(ssvs::getVecFromDegrees(degrees, speed)); }
			inline void draw() override { cRender.setRotation(degrees); }
	};
}

#endif
