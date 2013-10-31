// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_TURRET
#define SSVOB_COMPONENTS_TURRET

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCKillable.h"
#include "SSVBloodshed/Components/OBCWpnController.h"
#include "SSVBloodshed/Weapons/OBWpnTypes.h"
#include "SSVBloodshed/Weapons/OBWpn.h"

namespace ob
{
	class OBCTurret : public OBCActorBase
	{
		private:
			OBCKillable& cKillable;
			Dir8 direction;
			ssvs::Ticker tckShoot{0.f};
			ssvu::Timeline tlShoot{false};

			OBWpn wpn;
			float shootDelay, pjDelay;
			int shootCount;

		public:
			OBCTurret(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCKillable& mCKillable, Dir8 mDir, const OBWpnType& mWpn, float mShootDelay, float mPJDelay, int mShootCount) noexcept
				: OBCActorBase{mCPhys, mCDraw}, cKillable(mCKillable), direction{mDir}, wpn{game, OBGroup::GFriendlyKillable, mWpn}, shootDelay{mShootDelay}, pjDelay{mPJDelay}, shootCount{mShootCount} { }

			inline void init()
			{
				cDraw.setRotation(getDegFromDir8(direction));

				getEntity().addGroups(OBGroup::GEnemy, OBGroup::GEnemyKillable);
				body.setResolve(false);
				body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GEnemy, OBGroup::GKillable, OBGroup::GEnemyKillable);

				tckShoot.restart(shootDelay);
				repeat(tlShoot, [this]{ shoot(); }, shootCount, pjDelay);

				cKillable.onDeath += [this]{ game.createEShard(5, cPhys.getPosI()); };
			}
			inline void update(float mFT) override
			{
				tlShoot.update(mFT);
				if(tckShoot.getCurrent() > shootDelay / 1.5f && tckShoot.getCurrent() < shootDelay) game.createPCharge(1, cPhys.getPosPx(), 20);
				if(tckShoot.update(mFT)) { tlShoot.reset(); tlShoot.start(); }
			}

			inline void shoot()
			{
				assets.playSound("Sounds/spark.wav");
				Vec2i shootPos{body.getPosition() + getVecFromDir8<int>(direction) * 600};
				wpn.shoot(shootPos, getDegFromDir8(direction));
			}

			inline OBCKillable& getCKillable() const noexcept { return cKillable; }
	};

	// TODO: move to its own file
	class OBCForceField : public OBCActorBase
	{
		private:
			Dir8 dir;
			ssvsc::Segment<float> segment;
			bool destroyProjectiles, blockFriendly, blockEnemy;
			bool booster{false};
			float distortion{0}, alpha{0};

		public:
			OBCForceField(OBCPhys& mCPhys, OBCDraw& mCDraw, Dir8 mDir, bool mDestroyProjectiles, bool mBlockFriendly, bool mBlockEnemy) noexcept
				: OBCActorBase{mCPhys, mCDraw}, dir{mDir}, destroyProjectiles{mDestroyProjectiles}, blockFriendly{mBlockFriendly}, blockEnemy{mBlockEnemy} { }

			inline void init()
			{
				booster = !destroyProjectiles && !blockFriendly && !blockEnemy;

				cDraw.setRotation(getDegFromDir8(dir));
				getEntity().addGroups(OBGroup::GForceField);
				body.setResolve(false);
				body.addGroups(OBGroup::GForceField);
				body.addGroupsToCheck(OBGroup::GProjectile, OBGroup::GFriendly, OBGroup::GEnemy);

				// Calculate the segment by orbiting the center point
				segment = {ssvs::getOrbitDeg(body.getPosition(), getDeg() + 90.f, 1500.f), ssvs::getOrbitDeg(body.getPosition(), getDeg() - 90.f, 1500.f)};

				body.onDetection += [this](const DetectionInfo& mDI)
				{
					// When something touches the force field, spawn particles
					game.createPForceField(1, toPixels(mDI.body.getPosition()));

					if(!booster)
					{
						distortion = 10;

						// If this force field blocks friendlies or enemies and the detected body matches...
						if((blockFriendly && mDI.body.hasGroup(OBGroup::GFriendly)) || (blockEnemy && mDI.body.hasGroup(OBGroup::GEnemy)))
						{
							const auto& dirVec(getVecFromDir8<float>(dir));
							bool isMoving{mDI.body.getVelocity().x != 0 || mDI.body.getVelocity().y != 0};

							// Check if the body is "inside" the force field (check if it's on the right side of the segment)
							if(!segment.isPointLeft(Vec2f(mDI.body.getPosition())))
							{
								// If it's not inside, push it away from the force field
								mDI.body.applyForce(dirVec * -25.f);

								// If it's moving and it's not inside, treat the collision as a solid one
								if(isMoving && isDegBlocked(ssvs::getDeg(mDI.body.getVelocity()))) mDI.body.resolvePosition(ssvsc::Utils::getMin1DIntersection(mDI.body.getShape(), body.getShape()));
							}
							else if(!isMoving) mDI.body.applyForce(dirVec * 25.f);
						}

						// Eventually destroy projectiles that move against the force field
						if(destroyProjectiles && mDI.body.hasGroup(OBGroup::GProjectile))
						{
							auto& cProjectile(getComponentFromBody<OBCProjectile>(mDI.body));
							if(isDegBlocked(cProjectile.getDeg())) cProjectile.destroy();
						}
					}
					else if(mDI.body.hasGroup(OBGroup::GProjectile)) mDI.body.applyForce(getVecFromDir8<float>(dir) * -30.f);
				};
			}

			inline void update(float mFT) override
			{
				if(!booster && distortion > 0.f)
				{
					distortion -= mFT;
					cDraw.setGlobalScale(distortion <= 0.f ? 1.f : ssvu::getRndR(0.9f, 2.1f));
				}

				alpha = std::fmod(alpha + mFT * 0.06f, ssvu::pi);
				auto color(cDraw[0].getColor());
				color.a = 255 - std::sin(alpha) * 125;
				cDraw[0].setColor(color);
			}

			inline bool isDegBlocked(float mDeg) const noexcept { return ssvu::getDistDeg(mDeg, getDeg()) <= 90.f; }

			inline Dir8 getDir() const noexcept		{ return dir; }
			inline float getDeg() const noexcept	{ return getDegFromDir8(dir); }

			inline bool destroysProjectiles() const noexcept { return destroyProjectiles; }
	};
}

#endif

