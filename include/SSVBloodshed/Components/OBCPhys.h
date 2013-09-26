// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PHYSICS
#define SSVOB_COMPONENTS_PHYSICS

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCPhys : public sses::Component
	{
		private:
			static constexpr int crushedMax{3}, crushedTolerance{1};
			OBGame& game;
			World& world;
			Body& body;
			Vec2i lastResolution;
			int crushedLeft{0}, crushedRight{0}, crushedTop{0}, crushedBottom{0};

		public:
			OBCPhys(OBGame& mGame, bool mIsStatic, const Vec2i& mPosition, const Vec2i& mSize) : game(mGame), world(mGame.getWorld()), body(world.create(mPosition, mSize, mIsStatic)) { }
			inline ~OBCPhys() { body.destroy(); }

			inline void init() override
			{
				body.setUserData(&getEntity());
				body.onResolution += [this](const ResolutionInfo& mResolutionInfo)
				{
					lastResolution = mResolutionInfo.resolution;
					if(lastResolution.x > 0) crushedLeft = crushedMax; else if(lastResolution.x < 0) crushedRight = crushedMax;
					if(lastResolution.y > 0) crushedTop = crushedMax; else if(lastResolution.y < 0) crushedBottom = crushedMax;
				};
				body.onPreUpdate += [this]
				{
					lastResolution = {0, 0};
					if(crushedLeft > 0) --crushedLeft;
					if(crushedRight > 0) --crushedRight;
					if(crushedTop > 0) --crushedTop;
					if(crushedBottom > 0) --crushedBottom;
				};
			}
			inline void update(float) override { }

			inline void setPos(const Vec2i& mPos) noexcept			{ body.setPosition(mPos); }
			inline void setVel(const Vec2f& mVel) noexcept			{ body.setVelocity(mVel); }

			inline OBGame& getGame() const noexcept					{ return game; }
			inline World& getWorld() const noexcept			{ return world; }
			inline Body& getBody() const noexcept			{ return body; }
			inline const Vec2i& getLastResolution() const noexcept	{ return lastResolution; }
			inline bool isCrushedLeft() const noexcept				{ return crushedLeft > crushedTolerance; }
			inline bool isCrushedRight() const noexcept				{ return crushedRight > crushedTolerance; }
			inline bool isCrushedTop() const noexcept				{ return crushedTop > crushedTolerance; }
			inline bool isCrushedBottom() const noexcept			{ return crushedBottom > crushedTolerance; }
			inline int getCrushedLeft() const noexcept				{ return crushedLeft; }
			inline int getCrushedRight() const noexcept				{ return crushedRight; }
			inline int getCrushedTop() const noexcept				{ return crushedTop; }
			inline int getCrushedBottom() const	noexcept			{ return crushedBottom; }

			inline const Vec2i& getPosI() const noexcept			{ return body.getPosition(); }
			inline Vec2f getPosPixels() const noexcept				{ return toPixels(body.getPosition()); }
			inline Vec2f getPosF() const noexcept					{ return Vec2f(body.getPosition()); }
			inline const Vec2f& getVel() const noexcept				{ return body.getVelocity(); }
			inline float getLeft() const noexcept					{ return body.getShape().getLeft(); }
			inline float getRight() const noexcept					{ return body.getShape().getRight(); }
			inline float getTop() const noexcept					{ return body.getShape().getTop(); }
			inline float getBottom() const noexcept					{ return body.getShape().getBottom(); }

	};
}

#endif
