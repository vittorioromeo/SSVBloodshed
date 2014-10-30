// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_BULLETFORCEFIELD
#define SSVOB_COMPONENTS_BULLETFORCEFIELD

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCIdReceiver.hpp"

namespace ob
{
	class OBCBulletForceField : public OBCActor
	{
		private:
			OBCIdReceiver& cIdReceiver;
			bool active{true}, blockFriendly, blockEnemy;
			float rad, distortion{0}, alpha{0};

		public:
			OBCBulletForceField(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, Dir8 mDir, bool mBlockFriendly, bool mBlockEnemy) noexcept
				: OBCActor{mE, mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), blockFriendly{mBlockFriendly}, blockEnemy{mBlockEnemy}, rad{getRadFromDir8(mDir)}
			{
				controlBoolByIdAction(cIdReceiver, active);

				cDraw.setRotation(ssvu::toDeg(rad));
				getEntity().addGroups(OBGroup::GBulletForceField);
				body.setResolve(false);
				body.addGroups(OBGroup::GBulletForceField);
				body.addGroupsToCheck(OBGroup::GProjectile);

				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!active) return;

					// When something touches the force field, spawn particles
					game.createPForceField(1, toPixels(mDI.body.getPosition()));

					distortion = 10;

					auto& cProjectile(getComponentFromBody<OBCProjectile>(mDI.body));
					const auto& targetGroup(cProjectile.getTargetGroup());

					if(targetGroup == OBGroup::GKillable ||								// If the projectile kills anything
						(blockFriendly && targetGroup == OBGroup::GEnemyKillable) ||	// Or if it kills enemies
						(blockEnemy && targetGroup == OBGroup::GFriendlyKillable))		// Or if it kills friendlies
					{
						if(isRadBlocked(cProjectile.getRad())) cProjectile.destroy();
					}
				};
			}

			inline void update(FT mFT) override
			{
				auto color(cDraw[0].getColor());

				if(!active) color.a = 100;
				else
				{
					if(distortion > 0.f)
					{
						distortion -= mFT;
						cDraw.setGlobalScale(distortion <= 0.f ? 1.f : ssvu::getRndR(0.9f, 1.1f));
					}

					alpha = std::fmod(alpha + mFT * 0.06f, ssvu::pi);
					color.a = 255 - std::sin(alpha) * 125;
				}

				cDraw[0].setColor(color);
			}

			inline bool isRadBlocked(float mRad) const noexcept { return ssvu::getDistRad(mRad, rad) <= ssvu::piHalf; }
	};
}

#endif
