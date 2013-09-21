// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PLAYER
#define SSVOB_COMPONENTS_PLAYER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"

namespace ob
{
	class OBCPlayer : public sses::Component
	{
		public:
			enum class Action{Idle, Shooting};

		private:
			OBGame& game;
			OBCPhys& cPhys;
			OBCDraw& cDraw;
			OBAssets& assets;
			ssvsc::Body& body;
			Action action{Action::Idle};
			Direction direction{Direction::E};
			float walkSpeed{125.f};
			ssvs::Ticker shootTimer{4.7f};

		public:
			OBCPlayer(OBGame& mGame, OBCPhys& mCPhysics, OBCDraw& mCRender, OBAssets& mAssets) : game(mGame), cPhys(mCPhysics), cDraw(mCRender), assets(mAssets), body(cPhys.getBody()) { }

			inline void update(float mFrameTime) override
			{
				action = game.getIShoot() ? Action::Shooting : Action::Idle;

				const auto& ix(game.getIX());
				const auto& iy(game.getIY());
				const auto& iVec(ssvs::getNormalized(Vec2f(ix, iy)));

				if(action != Action::Shooting)
				{
					if(ix != 0 || iy != 0) direction = getDirectionFromXY(ix, iy);
				}
				else if(shootTimer.update(mFrameTime)) shoot();

				if(game.getIBomb()) bomb();

				body.setVelocity(iVec * walkSpeed);
			}
			inline void draw() override
			{
				auto& s0(cDraw[0]);
				auto& s1(cDraw[1]);
				auto& s1Offset(cDraw.getOffsets()[1]);
				s1.setColor({255, 255, 255, action != Action::Shooting ? static_cast<unsigned char>(0) : static_cast<unsigned char>(255)});

				const auto& tileX(action == Action::Shooting ? 2u : 0u);

				auto setSpriteO = [&](int mRot)
				{
					s0.setTextureRect(assets.tilesetPlayer[{tileX, 0}]); s0.setRotation(90 * mRot);
					s1.setTextureRect(assets.tilesetPlayer[{4, 0}]); s1.setRotation(90 * mRot);
				};
				auto setSpriteD = [&](int mRot)
				{
					s0.setTextureRect(assets.tilesetPlayer[{1 + tileX, 0}]); s0.setRotation(90 * mRot);
					s1.setTextureRect(assets.tilesetPlayer[{5, 0}]); s1.setRotation(90 * mRot);
				};

				const auto& xy(getXYFromDirection(direction));
				const auto& intDir(static_cast<int>(direction));
				s1Offset = Vec2f(xy[0], xy[1]) * 10.f;
				if(intDir % 2 == 0) setSpriteO(intDir / 2); else setSpriteD(intDir / 2);
			}

			inline void shoot()
			{
				Vec2i shootPosition{body.getPosition() + Vec2i(getVecFromDirection<float>(direction) * 1100.f)};
				game.getFactory().createProjectileBullet(shootPosition, getDegreesFromDirection(direction));
				game.createPMuzzle(20, toPixels(shootPosition));
			}

			inline void bomb() { }

			inline Action getAction() const noexcept		{ return action; }
			inline Direction getDirection() const noexcept	{ return direction; }
	};
}

#endif
