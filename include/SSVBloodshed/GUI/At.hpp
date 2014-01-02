// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_AT
#define SSVOB_GUI_AT

namespace ob
{
	namespace GUI
	{
		enum class At : int{Left = 0, Right = 1, Top = 2, Bottom = 3, NW = 4, NE = 5, SW = 6, SE = 7, Center = 8};
		enum class Scaling{Manual, FitToParent, FitToNeighbor, FitToChildren};

		inline At getAtOpposite(At mAt) noexcept
		{
			static At opposites[9]{At::Right, At::Left, At::Bottom, At::Top, At::SE, At::SW, At::NE, At::NW, At::Center};
			return opposites[int(mAt)];
		}
		inline Vec2f getAtVec(At mAt, float mMag) noexcept
		{
			static Vec2f vecs[9]
			{
				{-1.f, 0.f},	// Left
				{1.f, 0.f},		// Right
				{0.f, -1.f},	// Top
				{0.f, 1.f},		// Bottom
				{-1.f, -1.f},	// NW
				{1.f, -1.f},	// NE
				{-1.f, 1.f},	// SW
				{1.f, 1.f},		// SE
				{0.f, 0.f}		// Center
			};

			return vecs[int(mAt)] * mMag;
		}
		template<typename T> inline Vec2f getVecPos(At mAt, T& mWidget) noexcept
		{
			switch(mAt)
			{
				case At::Left:		return {mWidget.getLeft(), mWidget.getY()};
				case At::Right:		return {mWidget.getRight(), mWidget.getY()};
				case At::Top:		return {mWidget.getX(), mWidget.getTop()};
				case At::Bottom:	return {mWidget.getX(), mWidget.getBottom()};
				case At::NW:		return mWidget.getVertexNW();
				case At::NE:		return mWidget.getVertexNE();
				case At::SW:		return mWidget.getVertexSW();
				case At::SE:		return mWidget.getVertexSE();
				case At::Center:	return mWidget.getPosition();
			}

			return mWidget.getPosition();
		}
	}
}

#endif
