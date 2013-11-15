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
			AABBShape(const Vec2f& mPosition, const Vec2f& mHalfSize) { AABBShape::setSize(mHalfSize * 2.f); AABBShape::setPosition(mPosition); }

			inline void setPosition(const Vec2f& mPosition)		{ sf::RectangleShape::setPosition(mPosition); }
			inline void setPosition(float mX, float mY)			{ AABBShape::setPosition(Vec2f{mX, mY}); }
			inline void setSize(const Vec2f& mSize)				{ sf::RectangleShape::setSize(mSize); sf::RectangleShape::setOrigin(getHalfSize()); }
			inline void setSize(float mX, float mY)				{ AABBShape::setSize(Vec2f{mX, mY}); }
			inline void setWidth(float mWidth) noexcept			{ AABBShape::setSize(mWidth, getHeight()); }
			inline void setHeight(float mHeight) noexcept		{ AABBShape::setSize(getWidth(), mHeight); }
			inline void setX(float mX) noexcept					{ AABBShape::setPosition(mX, getY()); }
			inline void setY(float mY) noexcept					{ AABBShape::setPosition(getX(), mY); }

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

			inline Vec2f getVertexNW() const noexcept { return Vec2f(getLeft(), getTop()); }
			inline Vec2f getVertexNE() const noexcept { return Vec2f(getRight(), getTop()); }
			inline Vec2f getVertexSW() const noexcept { return Vec2f(getLeft(), getBottom()); }
			inline Vec2f getVertexSE() const noexcept { return Vec2f(getRight(), getBottom()); }

			inline bool isOverlapping(const Vec2f& mPoint, float mPadding) const noexcept
			{
				return mPoint.x >= getLeft() - mPadding && mPoint.x < getRight() + mPadding
						&& mPoint.y >= getTop() - mPadding && mPoint.y < getBottom() + mPadding;
			}
		};
	}
}

#endif
