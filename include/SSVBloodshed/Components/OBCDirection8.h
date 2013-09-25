// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DIRECTION8
#define SSVOB_COMPONENTS_DIRECTION8

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCDirection8 : public sses::Component
	{
		private:
			Direction8 direction{Direction8::E};

		public:
			inline OBCDirection8& operator=(Direction8 mValue) noexcept	{ setDirection(mValue); return *this; }
			inline void setDirection(Direction8 mValue) noexcept		{ direction = mValue; }

			inline operator Direction8() const noexcept					{ return direction; }
			inline Direction8 getDirection() const noexcept				{ return direction; }
			inline int getInt() const noexcept							{ return static_cast<int>(direction); }
			inline float getDegrees() const noexcept					{ return getDegreesFromDirection8(direction); }
			inline Vec2f getVec(float mMagnitude = 1.f) const noexcept	{ return ssvs::getVecFromDegrees(getDegrees(), mMagnitude); }
	};
}

#endif
