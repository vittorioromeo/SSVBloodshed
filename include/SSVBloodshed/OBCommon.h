// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMMON
#define SSVOB_COMMON

#include <array>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SSVUtils/SSVUtils.h>
#include <SSVUtilsJson/SSVUtilsJson.h>
#include <SSVStart/SSVStart.h>
#include <SSVEntitySystem/SSVEntitySystem.h>
#include <SSVMenuSystem/SSVMenuSystem.h>
#include <SSVSCollision/SSVSCollision.h>

namespace ob
{
	// Typedefs
	template<typename T> using Vec2 = ssvs::Vec2<T>;
	template<typename T, typename TDeleter = std::default_delete<T>> using Uptr = ssvs::Uptr<T, TDeleter>;
	using Vec2i = ssvs::Vec2i;
	using Vec2f = ssvs::Vec2f;
	using Vec2u = ssvs::Vec2u;
	using Entity = sses::Entity;
	using World = ssvsc::World<ssvsc::HashGrid, ssvsc::Impulse>;
	using Body = World::BodyType;
	using Sensor = World::SensorType;
	using DetectionInfo = World::DetectionInfoType;
	using ResolutionInfo = World::ResolutionInfoType;

	// Game enums
	enum OBGroup : unsigned int
	{
		GSolidGround,
		GSolidAir,
		GProjectile,
		GOrganic,
		GFloor,
		GFriendly,
		GEnemy,
		GIdReceiver,
		GPPlate
	};
	enum OBLayer : int
	{
		LPlayer,
		LEnemy,
		LWall,
		LProjectile,
		LPSTemp,
		LPit,
		LFloorGrate,
		LPSPerm,
		LFloor,
		LBackground
	};
	enum class PPlateType{Single, Multi};
	enum OBIdAction : int {Toggle, Open, Close};

	// Level editor enums
	enum OBLETType : int
	{
		LETFloor,
		LETWall,
		LETGrate,
		LETPit,
		LETTurretSP,
		LETTurretCP,
		LETTurretBP,
		LETSpawner,
		LETPlayer,
		LETRunner,
		LETRunnerArmed,
		LETCharger,
		LETChargerArmed,
		LETJuggernaut,
		LETJuggernautArmed,
		LETGiant,
		LETBall,
		LETBallFlying,
		LETEnforcer,
		LETWallD,
		LETDoor,
		LETPPlateSingle,
		LETPPlateMulti
	};

	// Pixel <-> coords utils
	template<typename T> inline constexpr float toPixels(T mValue) noexcept		{ return mValue / 100; }
	template<typename T> inline constexpr int toCoords(T mValue) noexcept		{ return mValue * 100; }
	template<typename T> inline Vec2f toPixels(const Vec2<T>& mValue) noexcept	{ return {toPixels(mValue.x), toPixels(mValue.y)}; }
	template<typename T> inline Vec2i toCoords(const Vec2<T>& mValue) noexcept	{ return {toCoords(mValue.x), toCoords(mValue.y)}; }

	// Direction utils
	constexpr float dir8Step{45.f};
	enum Dir8 : int {E = 0, SE = 1, S = 2, SW = 3, W = 4, NW = 5, N = 6, NE = 7};
	template<typename T = float> inline T getDegFromDir8(Dir8 mDir) noexcept { return T(int(mDir) * dir8Step); }
	template<typename T> inline Dir8 getDir8FromDeg(T mDegrees) noexcept
	{
		mDegrees = ssvu::wrapDegrees(mDegrees);
		int i{static_cast<int>((mDegrees + dir8Step / 2) / dir8Step)};
		return Dir8(i % 8);
	}
	template<typename T> inline Dir8 getDir8FromXY(T mX, T mY) noexcept
	{
		if(mX < 0 && mY == 0)		return Dir8::W;
		else if(mX > 0 && mY == 0)	return Dir8::E;
		else if(mX == 0 && mY < 0)	return Dir8::N;
		else if(mX == 0 && mY > 0)	return Dir8::S;
		else if(mX < 0 && mY < 0)	return Dir8::NW;
		else if(mX < 0 && mY > 0)	return Dir8::SW;
		else if(mX > 0 && mY < 0)	return Dir8::NE;
		return Dir8::SE;
	}
	template<typename T = int> inline std::array<T, 2> getXYFromDir8(Dir8 mDir) noexcept
	{
		switch(mDir)
		{
			case Dir8::E:	return {{1, 0}};
			case Dir8::SE:	return {{1, 1}};
			case Dir8::S:	return {{0, 1}};
			case Dir8::SW:	return {{-1, 1}};
			case Dir8::W:	return {{-1, 0}};
			case Dir8::NW:	return {{-1, -1}};
			case Dir8::N:	return {{0, -1}};
			case Dir8::NE:	return {{1, -1}};
		}
		return {{0, 0}};
	}
	template<typename T> inline Dir8 getDir8FromVec(const Vec2<T>& mVec) noexcept		{ return getDir8FromXY(mVec.x, mVec.y); }
	template<typename T = int> inline Vec2<T> getVecFromDir8(Dir8 mDir) noexcept	{ const auto& xy(getXYFromDir8<T>(mDir)); return {xy[0], xy[1]}; }

	// Timeline shortcuts
	inline void repeat(ssvu::Timeline& mTimeline, const ssvu::Action& mAction, unsigned int mTimes, float mWait)
	{
		assert(mTimes > 0);
		auto& action(mTimeline.append<ssvu::Do>(mAction));
		mTimeline.append<ssvu::Wait>(mWait);
		mTimeline.append<ssvu::Go>(action.getIndex(), mTimes - 1);
	}

	// Other utils
	inline Entity& getEntityFromBody(Body& mBody) { return *static_cast<Entity*>(mBody.getUserData()); }
}

#endif
