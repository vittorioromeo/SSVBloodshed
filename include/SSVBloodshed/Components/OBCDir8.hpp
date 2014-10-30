// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DIRECTION8
#define SSVOB_COMPONENTS_DIRECTION8

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"

namespace ob
{
	class OBCDir8 : public Component
	{
		private:
			Dir8 dir{Dir8::E};

		public:
			inline OBCDir8(Entity& mE) noexcept : Component{mE} { }

			inline OBCDir8& operator=(Dir8 mValue) noexcept	{ setDir(mValue); return *this; }
			inline void setDir(Dir8 mValue) noexcept		{ dir = mValue; }

			inline operator Dir8() const noexcept					{ return dir; }
			inline int getInt() const noexcept						{ return int(dir); }
			inline float getDeg() const noexcept					{ return getDegFromDir8(dir); }
			inline Vec2f getVec(float mMag = 1.f) const noexcept	{ return ssvs::getVecFromDeg(getDeg(), mMag); }
	};
}

#endif
