// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PLAYER
#define SSVOB_COMPONENTS_PLAYER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhysics.h"

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
			ssvsc::Body& body;
			Action action{Action::Standing};
			Direction direction{Direction::E};
			float walkSpeed{150.f};

		public:
			OBCPlayer(OBGame& mGame, OBCPhysics& mCPhysics) : game(mGame), cPhysics(mCPhysics), body(cPhysics.getBody()) { }
			~OBCPlayer() { }

			inline void init() override
			{
				body.onPreUpdate += [this]{ };
				body.onPostUpdate += [this]{ };
				body.onDetection += [this](const ssvsc::DetectionInfo&){ };
				body.onResolution += [this](const ssvsc::ResolutionInfo&){ };
				cPhysics.onResolution += [this](const Vec2i&) { };
			}
			void update(float mFrameTime) override
			{

			}

			inline void move(Direction mDirection, float mFrameTime)
			{

			}

			inline Action getAction() const noexcept		{ return action; }
			inline Direction getDirection() const noexcept	{ return direction; }
	};
}

#endif
