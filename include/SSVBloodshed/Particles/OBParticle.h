// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLE
#define SSVOB_PARTICLES_PARTICLE

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class Particle
	{
		private:
			Vec2f position, velocity;
			float acceleration{0.f};
			sf::Color color;
			float size{1.f}, life{100.f}, lifeMax{100.f}, alphaMult{1.f};

		public:
			inline Particle(const Vec2f& mPosition, const Vec2f& mVelocity, float mAcceleration, const sf::Color& mColor, float mSize, float mLife, float mAlphaMult)
				: position{mPosition}, velocity{mVelocity}, acceleration{mAcceleration}, color{mColor}, size{mSize}, life{mLife}, lifeMax{mLife}, alphaMult{mAlphaMult} { }

			inline void update(float mFrameTime)
			{
				life -= mFrameTime;
				color.a = static_cast<unsigned char>(ssvu::getClamped(life * (255.f / lifeMax) * alphaMult, 0.f, 255.f));
				velocity *= acceleration; // TODO: * mFrameTime?
				position += velocity * mFrameTime;
			}

			inline void setPosition(const Vec2f& mPosition) noexcept	{ position = mPosition; }
			inline void setColor(const sf::Color& mColor) noexcept		{ color = mColor; }
			inline void setVelocity(const Vec2f& mVelocity) noexcept	{ velocity = mVelocity; }
			inline void setAcceleration(float mAcceleration) noexcept	{ acceleration = mAcceleration; }
			inline void setLife(float mLife) noexcept					{ life = lifeMax = mLife; }
			inline void setOpacityMult(float mAlphaMult) noexcept		{ alphaMult = mAlphaMult; }
			inline void setSize(float mSize) noexcept					{ size = mSize; }

			inline float getLife() const noexcept						{ return life; }
			inline float getSize() const noexcept						{ return size; }
			inline const Vec2f& getPosition() const noexcept			{ return position; }
			inline const sf::Color& getColor() const noexcept			{ return color; }
	};
}

#endif
