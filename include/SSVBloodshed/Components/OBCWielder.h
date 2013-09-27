// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_WIELDER
#define SSVOB_COMPONENTS_WIELDER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCDirection8.h"

namespace ob
{
	class OBCWielder : public OBCActorBase
	{
		private:
			OBCDirection8& cDirection8;
			bool shooting{false};
			float wieldDistance{1000.f};

		public:
			OBCWielder(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCDirection8& mCDirection8) : OBCActorBase{mCPhys, mCDraw}, cDirection8(mCDirection8) { }

			inline void draw() override
			{
				if(shooting)
				{
					cDraw[1].setRotation(cDirection8.getDegrees());
					cDraw.getOffsets()[1] = cDirection8.getVec(toPixels(wieldDistance));
					cDraw[1].setColor({255, 255, 255, 255});
				}
				else cDraw[1].setColor({255, 255, 255, 0});
			}

			inline void setShooting(bool mValue) noexcept			{ shooting = mValue; }
			inline void setWieldDistance(float mValue) noexcept		{ wieldDistance = mValue; }

			inline OBCDirection8& getCDirection() const noexcept	{ return cDirection8; }
			inline bool isShooting() const noexcept					{ return shooting; }
			inline Vec2i getShootingPos() const noexcept			{ return cPhys.getPosI() + Vec2i(cDirection8.getVec(wieldDistance)); }
	};
}

#endif
