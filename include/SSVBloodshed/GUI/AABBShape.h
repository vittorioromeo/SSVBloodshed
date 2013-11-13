// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_AABBSHAPE
#define SSVOB_GUI_AABBSHAPE

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	namespace GUI
	{
		struct AABBShape : public sf::RectangleShape
		{
			using sf::RectangleShape::RectangleShape;

			AABBShape() = default;
			AABBShape(const Vec2f& mPosition, const Vec2f& mHalfSize) { AABBShape::setPosition(mPosition); AABBShape::setSize(mHalfSize * 2.f); }

			inline void setPosition(const Vec2f& mPosition)		{ sf::RectangleShape::setPosition(mPosition); }
			inline void setPosition(float mX, float mY)			{ sf::RectangleShape::setPosition(Vec2f{mX, mY}); }
			inline void setSize(const Vec2f& mSize)				{ sf::RectangleShape::setSize(mSize); sf::RectangleShape::setOrigin(getHalfSize()); }
			inline void setSize(float mX, float mY)				{ sf::RectangleShape::setSize(Vec2f{mX, mY}); sf::RectangleShape::setOrigin(getHalfSize()); }
			inline void setWidth(float mWidth) noexcept			{ setSize(mWidth, getHeight()); }
			inline void setHeight(float mHeight) noexcept		{ setSize(getWidth(), mHeight); }
			inline void setX(float mX) noexcept					{ setPosition(mX, getY()); }
			inline void setY(float mY) noexcept					{ setPosition(getX(), mY); }

			inline float getX() const noexcept					{ return sf::RectangleShape::getPosition().x; }
			inline float getY() const noexcept					{ return sf::RectangleShape::getPosition().y; }
			inline float getLeft() const noexcept				{ return sf::RectangleShape::getPosition().x - getHalfSize().x; }
			inline float getRight() const noexcept				{ return sf::RectangleShape::getPosition().x + getHalfSize().x; }
			inline float getTop() const noexcept				{ return sf::RectangleShape::getPosition().y - getHalfSize().y; }
			inline float getBottom() const noexcept				{ return sf::RectangleShape::getPosition().y + getHalfSize().y; }
			inline Vec2f getHalfSize() const noexcept			{ return sf::RectangleShape::getSize() / 2.f; }
			inline float getHalfWidth() const noexcept			{ return getHalfSize().x; }
			inline float getHalfHeight() const noexcept			{ return getHalfSize().y; }
			inline float getWidth() const noexcept				{ return sf::RectangleShape::getSize().x; }
			inline float getHeight() const noexcept				{ return sf::RectangleShape::getSize().y; }

			inline void resizeFromLeft(float mWidth) 			{ setX(getX() - (mWidth - getWidth()) / 2.f); setWidth(mWidth); }
			inline void resizeFromRight(float mWidth) 			{ setX(getX() + (mWidth - getWidth()) / 2.f); setWidth(mWidth); }
			inline void resizeFromTop(float mHeight) 			{ setY(getY() - (mHeight - getHeight()) / 2.f); setHeight(mHeight); }
			inline void resizeFromBottom(float mHeight) 		{ setY(getY() + (mHeight - getHeight()) / 2.f); setHeight(mHeight); }
			inline void resizeFromNW(const Vec2f& mSize) 		{ resizeFrom(Vec2f{-1.f, -1.f}, mSize); }
			inline void resizeFromNE(const Vec2f& mSize) 		{ resizeFrom(Vec2f{1.f, -1.f}, mSize); }
			inline void resizeFromSW(const Vec2f& mSize) 		{ resizeFrom(Vec2f{-1.f, 1.f}, mSize); }
			inline void resizeFromSE(const Vec2f& mSize) 		{ resizeFrom(Vec2f{1.f, 1.f}, mSize); }

			inline void resizeFrom(const Vec2f& mOrigin, const Vec2f& mSize)
			{
				assert(mOrigin.x >= 0.f && mOrigin.x <= 1.f);
				assert(mOrigin.y >= 0.f && mOrigin.y <= 1.f);

				setX(getX() - mOrigin.x * ((mSize.x - getWidth()) / 2.f));
				setY(getY() - mOrigin.y * ((mSize.y - getHeight()) / 2.f));
				setSize(mSize);
			}

			template<typename T = float> inline Vec2<T> getVertexNW() const noexcept { return Vec2<T>(getLeft(), getTop()); }
			template<typename T = float> inline Vec2<T> getVertexNE() const noexcept { return Vec2<T>(getRight(), getTop()); }
			template<typename T = float> inline Vec2<T> getVertexSW() const noexcept { return Vec2<T>(getLeft(), getBottom()); }
			template<typename T = float> inline Vec2<T> getVertexSE() const noexcept { return Vec2<T>(getRight(), getBottom()); }

			inline bool isOverlapping(const Vec2f& mPoint) const noexcept	{ return mPoint.x >= getLeft() && mPoint.x < getRight() && mPoint.y >= getTop() && mPoint.y < getBottom(); }
			inline bool contains(const Vec2f& mPoint) const noexcept		{ return isOverlapping(mPoint); }
		};
	}
}

#endif
