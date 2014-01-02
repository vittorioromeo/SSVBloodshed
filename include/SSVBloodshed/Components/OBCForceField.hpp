// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_FORCEFIELD
#define SSVOB_COMPONENTS_FORCEFIELD

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCIdReceiver.hpp"

namespace ob
{
	class OBCForceField : public OBCActorBase
	{
		private:
			OBCIdReceiver& cIdReceiver;
			ssvsc::Segment<float> segment;
			bool active{true}, blockFriendly, blockEnemy;
			float distortion{0}, alpha{0}, rad, forceMult;
			ssvs::Animation animation;

		public:
			OBCForceField(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, Dir8 mDir, bool mBlockFriendly, bool mBlockEnemy, float mForceMult) noexcept
				: OBCActorBase{mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), blockFriendly{mBlockFriendly}, blockEnemy{mBlockEnemy}, rad{getRadFromDir8(mDir)},
				  forceMult{mForceMult}, animation{assets.aForceField} { }

			inline void init()
			{
				controlBoolByIdAction(cIdReceiver, active);

				cDraw.setRotation(ssvu::toDeg(rad));
				getEntity().addGroups(OBGroup::GForceField);
				body.setResolve(false);
				body.addGroups(OBGroup::GForceField);
				body.addGroupsToCheck(OBGroup::GFriendly, OBGroup::GEnemy);

				// Calculate the segment by orbiting the center point
				segment = {ssvs::getOrbitRad(body.getPosition(), rad + ssvu::piHalf, 1500.f), ssvs::getOrbitRad(body.getPosition(), rad - ssvu::piHalf, 1500.f)};

				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(!active) return;

					const auto& dirVec(-ssvs::getVecFromRad<float>(rad));

					// When something touches the force field, spawn particles
					game.createPForceField(1, toPixels(mDI.body.getPosition()));

					distortion = 10;

					// If this force field blocks friendlies or enemies and the detected body matches...
					if((blockFriendly && mDI.body.hasGroup(OBGroup::GFriendly)) || (blockEnemy && mDI.body.hasGroup(OBGroup::GEnemy)))
					{
						bool isMoving{mDI.body.getVelocity().x != 0 || mDI.body.getVelocity().y != 0};

						// Check if the body is "inside" the force field (check if it's on the right side of the segment)
						if(!segment.isPointLeft(Vec2f(mDI.body.getPosition())))
						{
							// If it's not inside, push it away from the force field
							mDI.body.applyAccel(dirVec * 5.f * forceMult);

							// If it's moving and it's not inside, treat the collision as a solid one
							if(isMoving && isRadBlocked(ssvs::getRad(mDI.body.getVelocity())))
								mDI.body.resolvePosition(ssvsc::Utils::getMin1DIntersection(mDI.body.getShape(), body.getShape()));
						}
						else if(!isMoving) mDI.body.applyAccel(dirVec * -5.f * forceMult);
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
					cDraw[0].setTextureRect((*assets.tsSmall)(animation.getTileIndex()));

					alpha = std::fmod(alpha + mFT * 0.06f, ssvu::pi);
					color.a = 255 - std::sin(alpha) * 125;
				}

				cDraw[0].setColor(color);
			}

			inline bool isRadBlocked(float mRad) const noexcept { return ssvu::getDistRad(mRad, rad) <= ssvu::piHalf; }
	};
}

#endif
