// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PHYSICS
#define SSVOB_COMPONENTS_PHYSICS

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class OBGame;

	class OBCPhysics : public sses::Component
	{
		private:
			static constexpr int crushedMax{3}, crushedTolerance{1};

			ssvsc::World& world;
			ssvsc::Body& body;
			Vec2i lastResolution;
			int crushedLeft{0}, crushedRight{0}, crushedTop{0}, crushedBottom{0};

		public:
			ssvu::Delegate<void(sses::Entity&)> onDetection;
			ssvu::Delegate<void(const Vec2i&)> onResolution;

			OBCPhysics(ssvsc::World& mWorld, bool mIsStatic, const Vec2i& mPosition, const Vec2i& mSize) : world(mWorld), body(world.create(mPosition, mSize, mIsStatic)) { }
			inline ~OBCPhysics() { body.destroy(); }

			inline void init() override
			{
				body.setUserData(&getEntity());

				body.onDetection += [this](const ssvsc::DetectionInfo& mDetectionInfo)
				{
					if(mDetectionInfo.userData == nullptr) return;
					Entity* entity(static_cast<Entity*>(mDetectionInfo.userData));
					onDetection(*entity);
				};
				body.onResolution += [this](const ssvsc::ResolutionInfo& mResolutionInfo)
				{
					onResolution(mResolutionInfo.resolution);

					lastResolution = mResolutionInfo.resolution;
					if(mResolutionInfo.resolution.x > 0) crushedLeft = crushedMax;
					else if(mResolutionInfo.resolution.x < 0) crushedRight = crushedMax;
					if(mResolutionInfo.resolution.y > 0) crushedTop = crushedMax;
					else if(mResolutionInfo.resolution.y < 0) crushedBottom = crushedMax;
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

			inline ssvsc::World& getWorld() const				{ return world; }
			inline ssvsc::Body& getBody() const					{ return body; }
			inline const Vec2i& getPos() const					{ return body.getPosition(); }
			inline const Vec2i& getLastResolution() const		{ return lastResolution; }
			inline bool isCrushedLeft() const					{ return crushedLeft > crushedTolerance; }
			inline bool isCrushedRight() const					{ return crushedRight > crushedTolerance; }
			inline bool isCrushedTop() const					{ return crushedTop > crushedTolerance; }
			inline bool isCrushedBottom() const					{ return crushedBottom > crushedTolerance; }
			inline int getCrushedLeft() const					{ return crushedLeft; }
			inline int getCrushedRight() const					{ return crushedRight; }
			inline int getCrushedTop() const					{ return crushedTop; }
			inline int getCrushedBottom() const					{ return crushedBottom; }
	};
}

#endif
