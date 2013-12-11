// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLE
#define SSVOB_PARTICLES_PARTICLE

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
	struct OBParticle
	{
		Vec2f pos, vel, nw, ne, sw, se;
		float size, life, lifeMax, curveSpd, fuzziness, accel, alphaMult;
		sf::Color color;

		inline OBParticle(const Vec2f& mPos, const Vec2f& mVel, float mSize, float mLife, float mCurveSpd, float mFuzziness, float mAccel, float mAlphaMult, sf::Color mColor) noexcept
			: pos{mPos}, vel{mVel}, size{mSize}, life{mLife}, lifeMax{mLife}, curveSpd{ssvu::toRad(mCurveSpd)}, fuzziness{mFuzziness}, accel{mAccel}, alphaMult{mAlphaMult}, color{std::move(mColor)} { }

		inline void update(FT mFT) noexcept
		{
			life -= mFT;
			color.a = (ssvu::getClamped(life * (255.f / lifeMax) * alphaMult, 0.f, 255.f));
			ssvs::rotateRadAroundCenter(vel, ssvs::zeroVec2f, curveSpd);
			vel *= accel;
			pos += vel * mFT;

			float fz0{ssvu::getRndR(-fuzziness, fuzziness)};
			float fz1{ssvu::getRndR(-fuzziness, fuzziness)};
			float fz2{ssvu::getRndR(-fuzziness, fuzziness)};

			nw.x = pos.x - size + fz0;
			nw.y = pos.y - size + fz1;

			ne.x = pos.x + size + fz2;
			ne.y = pos.y - size + fz0;

			sw.x = pos.x - size + fz1;
			sw.y = pos.y + size + fz2;

			se.x = pos.x + size + fz0;
			se.y = pos.y + size + fz1;
		}
	};
}

#endif
