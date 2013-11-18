// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLE
#define SSVOB_PARTICLES_PARTICLE

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
	class OBParticle
	{
		private:
			Vec2f position, velocity;
			float acceleration{0.f};
			sf::Color color;
			float size{1.f}, life{100.f}, lifeMax{100.f}, alphaMult{1.f}, curveSpeed{0.f};

		public:
			inline OBParticle(const Vec2f& mPosition, const Vec2f& mVelocity, float mAcceleration, const sf::Color& mColor, float mSize, float mLife, float mAlphaMult = 1.f, float mCurveSpeed = 0.f) noexcept
				: position{mPosition}, velocity{mVelocity}, acceleration{mAcceleration}, color{mColor}, size{mSize}, life{mLife}, lifeMax{mLife}, alphaMult{mAlphaMult}, curveSpeed{mCurveSpeed} { }

			inline void update(float mFT) noexcept
			{
				life -= mFT;
				color.a = static_cast<unsigned char>(ssvu::getClamped(life * (255.f / lifeMax) * alphaMult, 0.f, 255.f));
				if(curveSpeed != 0) ssvs::rotateDegAroundCenter(velocity, ssvs::zeroVec2f, curveSpeed);
				velocity *= acceleration;
				position += velocity * mFT;
			}

			inline void setPosition(const Vec2f& mPosition) noexcept	{ position = mPosition; }
			inline void setColor(sf::Color mColor) noexcept				{ color = std::move(mColor); }
			inline void setVelocity(const Vec2f& mVelocity) noexcept	{ velocity = mVelocity; }
			inline void setAcceleration(float mAcceleration) noexcept	{ acceleration = mAcceleration; }
			inline void setCurveSpeed(float mCurveSpeed) noexcept		{ curveSpeed = mCurveSpeed; }
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
