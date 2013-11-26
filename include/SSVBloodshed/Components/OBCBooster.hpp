// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_BOOSTER
#define SSVOB_COMPONENTS_BOOSTER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCIdReceiver.hpp"

namespace ob
{
	class OBCBooster : public OBCActorBase
	{
		private:
			OBCIdReceiver& cIdReceiver;
			bool active{true};
			float alpha{0}, rad, forceMult;
			ssvs::Animation animation;

		public:
			OBCBooster(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, Dir8 mDir, float mForceMult) noexcept
				: OBCActorBase{mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), rad{getRadFromDir8(mDir)}, forceMult{mForceMult}, animation{assets.aForceField} { }

			inline void init()
			{
				animation = assets.aBulletBooster;

				controlBoolByIdAction(cIdReceiver, active);

				cDraw.setRotation(ssvu::toDeg(rad));
				getEntity().addGroups(OBGroup::GBooster);
				body.setResolve(false);
				body.addGroups(OBGroup::GBooster);
				body.addGroupsToCheck(OBGroup::GProjectile);

				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!active) return;
					const auto& dirVec(-ssvs::getVecFromRad(rad));

					// When something touches the force field, spawn particles
					game.createPForceField(1, toPixels(mDI.body.getPosition()));

					if(forceMult > 0.f) mDI.body.applyAccel(dirVec * 30.f * forceMult);
					else if(ssvs::getDeg(mDI.body.getVelocity()) != ssvs::getDeg(dirVec))
					{
						mDI.body.setVelocity(dirVec * ssvs::getMag(mDI.body.getVelocity()));
						mDI.body.setPosition(body.getPosition());
					}
				};
			}

			inline void update(FT mFT) override
			{
				auto color(cDraw[0].getColor());

				if(!active) color.a = 100;
				else
				{
					animation.update(mFT);

					if(forceMult != 0) cDraw[0].setTextureRect((*assets.tsSmall)(animation.getTileIndex()));

					alpha = std::fmod(alpha + mFT * 0.06f, ssvu::pi);
					color.a = 255 - std::sin(alpha) * 125;
				}

				cDraw[0].setColor(color);
			}
	};
}

#endif
