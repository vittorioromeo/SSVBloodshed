// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_WIELDER
#define SSVOB_COMPONENTS_WIELDER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCDir8.hpp"

namespace ob
{
	class OBCWielder : public OBCActor
	{
		private:
			OBCDir8& cDir8;
			bool shooting{false};
			float holdDist{2.f}, wieldDist{8.f};
			sf::IntRect rectStand, rectShoot;

		public:
			OBCWielder(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, OBCDir8& mCDir8, const sf::IntRect& mRectStand, const sf::IntRect& mRectShoot) noexcept
				: OBCActor{mE, mCPhys, mCDraw}, cDir8(mCDir8), rectStand{mRectStand}, rectShoot{mRectShoot} { }

			inline void setShooting(bool mValue) noexcept
			{
				shooting = mValue;
				if(shooting)
				{
					cDraw[0].setTextureRect(rectShoot);
					cDraw[1].setRotation(cDir8.getDeg() - 90);
					cDraw.getOffsets()[1] = cDir8.getVec(wieldDist);
				}
				else
				{
					cDraw[0].setTextureRect(rectStand);
					cDraw[1].setRotation(cDir8.getDeg());
					cDraw.getOffsets()[1] = cDir8.getVec(holdDist);
				}
			}
			inline void setHoldDist(float mValue) noexcept				{ holdDist = mValue; }
			inline void setWieldDist(float mValue) noexcept				{ wieldDist = mValue; }
			inline void setRectStand(const sf::IntRect& mRect) noexcept	{ rectStand = mRect; }
			inline void setRectShoot(const sf::IntRect& mRect) noexcept	{ rectShoot = mRect; }

			inline OBCDir8& getCDir8() const noexcept		{ return cDir8; }
			inline bool isShooting() const noexcept			{ return shooting; }
			inline Vec2i getShootingPos() const noexcept	{ return cPhys.getPosI() + Vec2i(cDir8.getVec(toCoords(wieldDist * 0.6f))); }
			inline Vec2f getShootingPosPx() const noexcept	{ return toPixels(cPhys.getPosI() + Vec2i(cDir8.getVec(toCoords(wieldDist * 1.3f)))); }
			inline float getHoldDist() const noexcept		{ return holdDist; }
			inline float getWieldDist() const noexcept		{ return wieldDist; }
	};
}

#endif

