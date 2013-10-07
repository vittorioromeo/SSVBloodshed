// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_WIELDER
#define SSVOB_COMPONENTS_WIELDER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCDir8.h"

namespace ob
{
	class OBCWielder : public OBCActorBase
	{
		private:
			OBCDir8& cDir8;
			bool shooting{false};
			float wieldDist{1000.f};

		public:
			OBCWielder(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCDir8& mCDir8) noexcept : OBCActorBase{mCPhys, mCDraw}, cDir8(mCDir8) { }

			inline void draw() override
			{
				if(!shooting) return;
				cDraw[1].setRotation(cDir8.getDeg());
				cDraw.getOffsets()[1] = cDir8.getVec(toPixels(wieldDist));
			}

			inline void setShooting(bool mValue) noexcept		{ shooting = mValue; cDraw[1].setColor(sf::Color(255, 255, 255, shooting ? 255 : 0)); }
			inline void setWieldDist(float mValue) noexcept		{ wieldDist = mValue; }

			inline OBCDir8& getCDir8() const noexcept			{ return cDir8; }
			inline bool isShooting() const noexcept				{ return shooting; }
			inline Vec2i getShootingPos() const noexcept		{ return cPhys.getPosI() + Vec2i(cDir8.getVec(wieldDist)); }
	};
}

#endif
