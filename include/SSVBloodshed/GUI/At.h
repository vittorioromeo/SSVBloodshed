// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_AT
#define SSVOB_GUI_AT

namespace ob
{
	namespace GUI
	{
		enum class At{Left, Right, Top, Bottom, NW, NE, SW, SE, Center};

		inline At getAtOpposite(At mAt) noexcept
		{
			switch(mAt)
			{
				case At::Left:		return At::Right;
				case At::Right:		return At::Left;
				case At::Top:		return At::Bottom;
				case At::Bottom:	return At::Top;
				case At::NW:		return At::SE;
				case At::NE:		return At::SW;
				case At::SW:		return At::NE;
				case At::SE:		return At::NW;
				case At::Center:	return At::Center;
			}

			return At::Center;
		}
		inline Vec2f getAtVec(At mAt, float mMag) noexcept
		{
			switch(mAt)
			{
				case At::Left:		return Vec2f{-mMag, 0.f};
				case At::Right:		return Vec2f{mMag, 0.f};
				case At::Top:		return Vec2f{0.f, -mMag};
				case At::Bottom:	return Vec2f{0.f, mMag};
				case At::NW:		return Vec2f{-mMag, -mMag};
				case At::NE:		return Vec2f{-mMag, mMag};
				case At::SW:		return Vec2f{mMag, -mMag};
				case At::SE:		return Vec2f{mMag, mMag};
				case At::Center:	return Vec2f{0.f, 0.f};
			}

			return Vec2f{0.f, 0.f};
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
