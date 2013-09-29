// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DIRECTION8
#define SSVOB_COMPONENTS_DIRECTION8

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCDir8 : public sses::Component
	{
		private:
			Dir8 direction{Dir8::E};

		public:
			inline OBCDir8& operator=(Dir8 mValue) noexcept	{ setDir(mValue); return *this; }
			inline void setDir(Dir8 mValue) noexcept		{ direction = mValue; }

			inline operator Dir8() const noexcept						{ return direction; }
			inline int getInt() const noexcept							{ return int(direction); }
			inline float getDeg() const noexcept						{ return getDegFromDir8(direction); }
			inline Vec2f getVec(float mMagnitude = 1.f) const noexcept	{ return ssvs::getVecFromDegrees(getDeg(), mMagnitude); }
	};
}

#endif
