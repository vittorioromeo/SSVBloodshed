// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PLAYER
#define SSVOB_COMPONENTS_PLAYER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhysics.h"
#include "SSVBloodshed/Components/OBCRender.h"

namespace ob
{
	class OBCPlayer : public sses::Component
	{
		public:
			enum class Action{Standing, Aiming};
			enum class Direction{N, S, W, E, NW, NE, SW, SE};

		private:
			OBGame& game;
			OBCPhysics& cPhysics;
			OBCRender& cRender;
			OBAssets& assets;
			ssvsc::Body& body;
			Action action{Action::Standing};
			Direction direction{Direction::E};
			float walkSpeed{125.f};
			ssvs::Ticker shootTimer{4.7f};

			inline float getDegreesFromDirection(Direction mDirection)
			{
				switch(mDirection)
				{
					case Direction::N:	return -90.f;
					case Direction::S:	return 90.f;
					case Direction::W:	return 180.f;
					case Direction::E:	return 0.f;
					case Direction::NW:	return 225.f;
					case Direction::NE:	return -45.f;
					case Direction::SW:	return 135.f;
					case Direction::SE:	return 45.f;
				}

				return 0.f;
			}

		public:
			OBCPlayer(OBGame& mGame, OBCPhysics& mCPhysics, OBCRender& mCRender, OBAssets& mAssets) : game(mGame), cPhysics(mCPhysics), cRender(mCRender), assets(mAssets), body(cPhysics.getBody()) { }

			inline void init() override
			{
				body.onPreUpdate += [this]{ };
				body.onPostUpdate += [this]{ };
				body.onDetection += [this](const ssvsc::DetectionInfo&){ };
				body.onResolution += [this](const ssvsc::ResolutionInfo&){ };
				cPhysics.onResolution += [this](const Vec2i&) { };
			}
			inline void update(float mFrameTime) override
			{
				action = game.getIShoot() ? Action::Aiming : Action::Standing;

				const auto& ix(game.getIX());
				const auto& iy(game.getIY());
				const auto& iVec(ssvs::getNormalized(Vec2f(ix, iy)));
				const auto& dirVec(ssvs::getVecFromDegrees(getDegreesFromDirection(direction)));

				if(action != Action::Aiming)
				{
					if(ix == -1 && iy == 0)			direction = Direction::W;
					else if(ix == 1 && iy == 0)		direction = Direction::E;
					else if(ix == 0 && iy == -1)	direction = Direction::N;
					else if(ix == 0 && iy == 1)		direction = Direction::S;
					else if(ix == -1 && iy == -1)	direction = Direction::NW;
					else if(ix == -1 && iy == 1)	direction = Direction::SW;
					else if(ix == 1 && iy == -1)	direction = Direction::NE;
					else if(ix == 1 && iy == 1)		direction = Direction::SE;
				}
				else if(shootTimer.update(mFrameTime))
				{
					game.getFactory().createTestProj(body.getPosition() + Vec2i(dirVec * 1100.f), getDegreesFromDirection(direction));
				}

				if(ix != 0 || iy != 0) body.setVelocity(iVec * walkSpeed);
				else body.setVelocity(Vec2f{0.f, 0.f});
			}
			inline void draw() override
			{
				auto& s0(cRender[0]);
				auto& s1(cRender[1]);
				auto& s1Offset(cRender.getSpriteOffsets()[1]);
				s1.setColor({255, 255, 255, action != Action::Aiming ? static_cast<unsigned char>(0) : static_cast<unsigned char>(255)});

				const auto& tileX(action == Action::Aiming ? 2u : 0u);

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

				switch(direction)
				{
					case Direction::N:	setSpriteO(-1);	s1Offset = Vec2f(0.f, -10.f);	break;
					case Direction::S:	setSpriteO(1);	s1Offset = Vec2f(0.f, 10.f);	break;
					case Direction::W:	setSpriteO(-2);	s1Offset = Vec2f(-10.f, 0.f);	break;
					case Direction::E:	setSpriteO(0);	s1Offset = Vec2f(10.f, 0.f);	break;
					case Direction::NW:	setSpriteD(2);	s1Offset = Vec2f(-10.f, -10.f);	break;
					case Direction::NE:	setSpriteD(-1);	s1Offset = Vec2f(10.f, -10.f);	break;
					case Direction::SW:	setSpriteD(1);	s1Offset = Vec2f(-10.f, 10.f);	break;
					case Direction::SE:	setSpriteD(0);	s1Offset = Vec2f(10.f, 10.f);	break;
				}
			}

			inline Action getAction() const noexcept		{ return action; }
			inline Direction getDirection() const noexcept	{ return direction; }
	};

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
				body.addGroup(OBGroup::Projectile);
				body.addGroupToCheck(OBGroup::Solid);
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::Solid))
					{
						if(!mDI.body.hasGroup(OBGroup::Organic)) for(int i = 0; i < 7; ++i) game.debrisParticleSystem.createDebris(toPixels(body.getPosition()));
						getEntity().destroy();
					}
				};
			}
			inline void update(float mFrameTime) override { body.setVelocity(ssvs::getVecFromDegrees(degrees, speed)); }
			inline void draw() override { cRender.setRotation(degrees); }
	};

	class OBCEnemy : public sses::Component
	{
		private:
			OBGame& game;
			OBCPhysics& cPhysics;
			OBCRender& cRender;
			OBAssets& assets;
			ssvsc::Body& body;
			float walkSpeed{100.f};
			float currentDegrees{0.f};
			float turnSpeed{7.5f};
			int health{12};
			//ssvs::Ticker shootTimer{4.7f};

		public:
			OBCEnemy(OBGame& mGame, OBCPhysics& mCPhysics, OBCRender& mCRender, OBAssets& mAssets) : game(mGame), cPhysics(mCPhysics), cRender(mCRender), assets(mAssets), body(cPhysics.getBody()) { }

			inline void init() override
			{
				body.onPreUpdate += [this]{ body.setVelocity(ssvs::getMClamped(body.getVelocity(), -120.f, 120.f)); };
				body.onDetection += [this](const ssvsc::DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::Solid)) body.applyForce(Vec2f(mDI.intersection) * 1.f);
					if(mDI.body.hasGroup(OBGroup::Projectile))
					{
						--health;
						for(int i = 0; i < 30; ++i) game.bloodParticleSystem.createBlood(toPixels(body.getPosition()));

						if(health <= 0)
						{
							for(int i = 0; i < 110; ++i) game.bloodParticleSystem.createBlood(toPixels(body.getPosition()));
							for(int i = 0; i < 200; ++i) game.gibParticleSystem.createGib(toPixels(body.getPosition()));
							getEntity().destroy();
						}
					}
				};
			}
			inline void update(float mFrameTime) override
			{
				for(const auto& e : game.getManager().getEntities(OBGroup::Player))
				{
					auto& ecPhysics(e->getComponent<OBCPhysics>());
					float targetDegrees(ssvs::getDegreesTowards(Vec2f(body.getPosition()), Vec2f(ecPhysics.getBody().getPosition())));

					currentDegrees = ssvu::getRotatedDegrees(currentDegrees, targetDegrees, turnSpeed * mFrameTime);
					float sd = static_cast<int>(currentDegrees / 45.f) * 45.f;

					body.applyForce(ssvs::getVecFromDegrees(sd, walkSpeed) * 0.05f);
				}
			}
			inline void draw() override
			{
				auto& s0(cRender[0]);
				s0.setTextureRect(assets.tilesetEnemy[{0, 0}]);
				float sd = static_cast<int>(currentDegrees / 45.f) * 45.f;
				s0.setRotation(sd);
			}
	};
}

#endif
