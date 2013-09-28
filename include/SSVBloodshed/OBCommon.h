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
	using Entity = sses::Entity;
	using Vec2i = ssvs::Vec2i;
	using Vec2f = ssvs::Vec2f;
	using Vec2u = ssvs::Vec2u;

	using World = ssvsc::World;
	using Body = ssvsc::Body;
	using DetectionInfo = ssvsc::DetectionInfo;
	using ResolutionInfo = ssvsc::ResolutionInfo;

	// Game enums
	enum OBGroup : unsigned int
	{
		GSolidGround,
		GSolidAir,
		GProjectile,
		GOrganic,
		GFloor,
		GFriendly,
		GEnemy
	};
	enum OBLayer : int
	{
		LWall,
		LPlayer,
		LEnemy,
		LPSTemp,
		LProjectile,
		LPit,
		LFloorGrate,
		LPSPerm,
		LFloor,
		LBackground
	};

	// Level editor enums
	enum OBLETType : unsigned int
	{
		LETFloor,
		LETWall,
		LETGrate,
		LETPit,
		LETTurret,
		LETSpawner,
	};

	// Pixel <-> coords utils
	template<typename T> inline constexpr float toPixels(T mValue) noexcept		{ return mValue / 100; }
	template<typename T> inline constexpr int toCoords(T mValue) noexcept		{ return mValue * 100; }
	template<typename T> inline Vec2f toPixels(const Vec2<T>& mValue) noexcept	{ return {toPixels(mValue.x), toPixels(mValue.y)}; }
	template<typename T> inline Vec2i toCoords(const Vec2<T>& mValue) noexcept	{ return {toCoords(mValue.x), toCoords(mValue.y)}; }

	// Direction utils
	constexpr float direction8Step{45.f};
	enum Direction8 : int {E = 0, SE = 1, S = 2, SW = 3, W = 4, NW = 5, N = 6, NE = 7};
	template<typename T = float> inline T getDegreesFromDirection8(Direction8 mDirection) noexcept { return T(int(mDirection) * direction8Step); }
	template<typename T> inline Direction8 getDirection8FromDegrees(T mDegrees) noexcept
	{
		mDegrees = ssvu::wrapDegrees(mDegrees);
		int i{static_cast<int>((mDegrees + direction8Step / 2) / direction8Step)};
		return Direction8(i % 8);
	}
	template<typename T> inline Direction8 getDirection8FromXY(T mX, T mY) noexcept
	{
		if(mX < 0 && mY == 0)		return Direction8::W;
		else if(mX > 0 && mY == 0)	return Direction8::E;
		else if(mX == 0 && mY < 0)	return Direction8::N;
		else if(mX == 0 && mY > 0)	return Direction8::S;
		else if(mX < 0 && mY < 0)	return Direction8::NW;
		else if(mX < 0 && mY > 0)	return Direction8::SW;
		else if(mX > 0 && mY < 0)	return Direction8::NE;
		return Direction8::SE;
	}
	template<typename T = int> inline std::array<T, 2> getXYFromDirection8(Direction8 mDirection) noexcept
	{
		switch(mDirection)
		{
			case Direction8::E:		return {{1, 0}};
			case Direction8::SE:	return {{1, 1}};
			case Direction8::S:		return {{0, 1}};
			case Direction8::SW:	return {{-1, 1}};
			case Direction8::W:		return {{-1, 0}};
			case Direction8::NW:	return {{-1, -1}};
			case Direction8::N:		return {{0, -1}};
			case Direction8::NE:	return {{1, -1}};
		}
		return {{0, 0}};
	}
	template<typename T> inline Direction8 getDirection8FromVec(const Vec2<T>& mVec) noexcept		{ return getDirection8FromXY(mVec.x, mVec.y); }
	template<typename T = int> inline Vec2<T> getVecFromDirection8(Direction8 mDirection) noexcept	{ const auto& xy(getXYFromDirection8<T>(mDirection)); return {xy[0], xy[1]}; }

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

	// SFML shortcuts (TODO: remove? move to ssvs?)
	template<typename T> float getGlobalLeft(const T& mElement)		{ return mElement.getGlobalBounds().left; }
	template<typename T> float getGlobalRight(const T& mElement)	{ return mElement.getGlobalBounds().left + mElement.getGlobalBounds().width; }
	template<typename T> float getGlobalTop(const T& mElement)		{ return mElement.getGlobalBounds().top; }
	template<typename T> float getGlobalBottom(const T& mElement)	{ return mElement.getGlobalBounds().top + mElement.getGlobalBounds().height; }
	template<typename T> float getGlobalWidth(const T& mElement)	{ return mElement.getGlobalBounds().width; }
	template<typename T> float getGlobalHeight(const T& mElement)	{ return mElement.getGlobalBounds().height; }

	template<typename T> float getLocalLeft(const T& mElement)		{ return mElement.getLocalBounds().left; }
	template<typename T> float getLocalRight(const T& mElement)		{ return mElement.getLocalBounds().left + mElement.getLocalBounds().width; }
	template<typename T> float getLocalTop(const T& mElement)		{ return mElement.getLocalBounds().top; }
	template<typename T> float getLocalBottom(const T& mElement)	{ return mElement.getLocalBounds().top + mElement.getLocalBounds().height; }
	template<typename T> float getLocalWidth(const T& mElement)		{ return mElement.getLocalBounds().width; }
	template<typename T> float getLocalHeight(const T& mElement)	{ return mElement.getLocalBounds().height; }
}

#endif
