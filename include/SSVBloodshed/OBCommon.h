// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMMON
#define SSVOB_COMMON

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
	using Vec2i = ssvs::Vec2i;
	using Vec2f = ssvs::Vec2f;
	template<typename T, typename TDeleter = std::default_delete<T>> using Uptr = ssvs::Uptr<T, TDeleter>;
	template<typename T> using Vec2 = ssvs::Vec2<T>;
	using Entity = sses::Entity;

	enum OBGroup : unsigned int
	{
		Solid,
		Projectile,
		Player,
		Organic
	};

	template<typename T> inline constexpr float toPixels(T mValue) noexcept		{ return mValue / 100; }
	template<typename T> inline constexpr int toCoords(T mValue) noexcept		{ return mValue * 100; }
	template<typename T> inline Vec2f toPixels(const Vec2<T>& mValue) noexcept	{ return {toPixels(mValue.x), toPixels(mValue.y)}; }
	template<typename T> inline Vec2i toCoords(const Vec2<T>& mValue) noexcept	{ return {toCoords(mValue.x), toCoords(mValue.y)}; }
}

#endif
