// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ENEMY
#define SSVOB_COMPONENTS_ENEMY

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"

namespace ob
{
	class OBCEnemy : public sses::Component
	{
		public:
			OBGame& game;
			OBCPhys& cPhys;
			OBCDraw& cDraw;
			OBAssets& assets;
			ssvsc::Body& body;
			float walkSpeed{100.f}, currentDegrees{0.f}, turnSpeed{7.5f};
			float snappedDegrees{0.f};
			int health{3}; int gibMult{1};

			//ssvs::Ticker shootTimer{120.f};
			//ssvu::Timeline shootTimeline;

		public:
			OBCEnemy(OBGame& mGame, OBCPhys& mCPhys, OBCDraw& mCDraw) : game(mGame), cPhys(mCPhys), cDraw(mCDraw), assets(game.getAssets()), body(cPhys.getBody()) { }

			inline void init() override
			{
				body.setRestitutionX(1.7f);
				body.setRestitutionY(1.7f);
				body.onPreUpdate += [this]{ body.setVelocity(ssvs::getMClamped(body.getVelocity(), -120.f, 120.f)); };
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::Projectile))
					{
						--health;
						game.createPBlood(6 * gibMult, toPixels(body.getPosition()));

						if(health <= 0)
						{
							game.createPBlood(20 * gibMult * gibMult, toPixels(body.getPosition()), gibMult);
							game.createPGib(35 * gibMult * gibMult, toPixels(body.getPosition()));
							getEntity().destroy();
						}
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				for(const auto& e : game.getManager().getEntities(OBGroup::Player))
				{
					auto& ecPhys(e->getComponent<OBCPhys>());
					float targetDegrees(ssvs::getDegreesTowards(Vec2f(body.getPosition()), Vec2f(ecPhys.getBody().getPosition())));

					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					snappedDegrees = static_cast<int>(ssvu::wrapDegrees(currentDegrees) / 45) * 45;

					body.applyForce(ssvs::getVecFromDegrees(snappedDegrees, walkSpeed) * 0.05f);
				}

				/*shootTimeline.update(mFrameTime);

				if(shootTimer.update(mFrameTime))
				{
					shootTimeline.reset();
					shootTimeline.start();
				}*/
			}
			inline void draw() override
			{
				auto& s0(cDraw[0]);
				//s0.setTextureRect(assets.get<Tileset>("tileset")Enemy[{0, 0}]);
				s0.setRotation(snappedDegrees);
			}

			inline void shoot(int mDeg)
			{
				Vec2i shootPosition{body.getPosition() + Vec2i(ssvs::getVecFromDegrees<float>(currentDegrees) * 2100.f)};
				game.getFactory().createProjectilePlasma(shootPosition, currentDegrees + mDeg);
				game.createPMuzzle(20, toPixels(body.getPosition()));
			}
	};
}

#endif
