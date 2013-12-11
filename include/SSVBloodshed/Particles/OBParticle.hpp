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
			sf::Color color; Vec2f size;
			float life{100.f}, lifeMax{100.f}, alphaMult{1.f}, curveSpeed{0.f};
			float fuzziness{1.f};

		public:
			Vec2f nw, ne, sw, se;

			inline OBParticle(const Vec2f& mPosition, const Vec2f& mVelocity, float mAcceleration, const sf::Color& mColor,
							  float mSize, float mLife, float mAlphaMult = 1.f, float mCurveSpeed = 0.f, float mFuzziness = 0.f) noexcept
				: position{mPosition}, velocity{mVelocity}, acceleration{mAcceleration}, color{mColor}, size{mSize, mSize},
				  life{mLife}, lifeMax{mLife}, alphaMult{mAlphaMult}, curveSpeed{ssvu::toRad(mCurveSpeed)}, fuzziness{mFuzziness} { }

			inline void update(FT mFT) noexcept
			{
				life -= mFT;
				color.a = (ssvu::getClamped(life * (255.f / lifeMax) * alphaMult, 0.f, 255.f));
				if(curveSpeed != 0) ssvs::rotateRadAroundCenter(velocity, ssvs::zeroVec2f, curveSpeed);
				velocity *= acceleration;
				position += velocity * mFT;

				nw.x = position.x - size.x + ssvu::getRndR(-fuzziness, fuzziness);
				nw.y = position.y - size.y + ssvu::getRndR(-fuzziness, fuzziness);

				ne.x = position.x + size.x + ssvu::getRndR(-fuzziness, fuzziness);
				ne.y = position.y - size.y + ssvu::getRndR(-fuzziness, fuzziness);

				sw.x = position.x - size.x + ssvu::getRndR(-fuzziness, fuzziness);
				sw.y = position.y + size.y + ssvu::getRndR(-fuzziness, fuzziness);

				se.x = position.x + size.x + ssvu::getRndR(-fuzziness, fuzziness);
				se.y = position.y + size.y + ssvu::getRndR(-fuzziness, fuzziness);
			}

			inline void setPosition(const Vec2f& mPosition) noexcept	{ position = mPosition; }
			inline void setColor(sf::Color mColor) noexcept				{ color = std::move(mColor); }
			inline void setVelocity(const Vec2f& mVelocity) noexcept	{ velocity = mVelocity; }
			inline void setAcceleration(float mAcceleration) noexcept	{ acceleration = mAcceleration; }
			inline void setCurveSpeed(float mCurveSpeed) noexcept		{ curveSpeed = mCurveSpeed; }
			inline void setLife(float mLife) noexcept					{ life = lifeMax = mLife; }
			inline void setOpacityMult(float mAlphaMult) noexcept		{ alphaMult = mAlphaMult; }
			inline void setSize(float mSize) noexcept					{ size.x = size.y = mSize; }
			inline void setFuzziness(float mFuzziness) noexcept			{ fuzziness = mFuzziness; }

			inline float getLife() const noexcept						{ return life; }
			inline const Vec2f& getSize() const noexcept				{ return size; }
			inline const Vec2f& getPosition() const noexcept			{ return position; }
			inline const sf::Color& getColor() const noexcept			{ return color; }
	};
}

#endif
