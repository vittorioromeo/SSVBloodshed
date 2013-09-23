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

	// Game enums
	enum OBGroup : unsigned int
	{
		GSolid,
		GProjectile,
		GOrganic,
		GFloor,
		GFriendly,
		GEnemy
	};

	enum OBLayer : int
	{
		PWall,
		PPlayer,
		PEnemy,
		PParticleTemp,
		PProjectile,
		PFloorGrate,
		PParticlePerm,
		PFloor,
		PBackground
	};

	// Pixel <-> coords utils
	template<typename T> inline constexpr float toPixels(T mValue) noexcept		{ return mValue / 100; }
	template<typename T> inline constexpr int toCoords(T mValue) noexcept		{ return mValue * 100; }
	template<typename T> inline Vec2f toPixels(const Vec2<T>& mValue) noexcept	{ return {toPixels(mValue.x), toPixels(mValue.y)}; }
	template<typename T> inline Vec2i toCoords(const Vec2<T>& mValue) noexcept	{ return {toCoords(mValue.x), toCoords(mValue.y)}; }

	// Direction utils
	enum Direction : int {E = 0, SE = 1, S = 2, SW = 3, W = 4, NW = 5, N = 6, NE = 7};
	template<typename T = float> inline T getDegreesFromDirection(Direction mDirection) noexcept { return T(static_cast<int>(mDirection) * T(45)); }
	template<typename T> inline Direction getDirectionFromXY(T mX, T mY) noexcept
	{
		if(mX < 0 && mY == 0)		return Direction::W;
		else if(mX > 0 && mY == 0)	return Direction::E;
		else if(mX == 0 && mY < 0)	return Direction::N;
		else if(mX == 0 && mY > 0)	return Direction::S;
		else if(mX < 0 && mY < 0)	return Direction::NW;
		else if(mX < 0 && mY > 0)	return Direction::SW;
		else if(mX > 0 && mY < 0)	return Direction::NE;
		return Direction::SE;
	}
	template<typename T = int> inline std::array<T, 2> getXYFromDirection(Direction mDirection) noexcept
	{
		switch(mDirection)
		{
			case Direction::E:	return {{1, 0}};
			case Direction::SE:	return {{1, 1}};
			case Direction::S:	return {{0, 1}};
			case Direction::SW:	return {{-1, 1}};
			case Direction::W:	return {{-1, 0}};
			case Direction::NW:	return {{-1, -1}};
			case Direction::N:	return {{0, -1}};
			case Direction::NE:	return {{1, -1}};
		}
		return {{0, 0}};
	}
	template<typename T> inline Direction getDirectionFromVec(const Vec2<T>& mVec) noexcept { return getDirectionFromXY(mVec.x, mVec.y); }
	template<typename T = int> inline Vec2<T> getVecFromDirection(Direction mDirection) noexcept { const auto& xy(getXYFromDirection<T>(mDirection)); return {xy[0], xy[1]}; }

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
