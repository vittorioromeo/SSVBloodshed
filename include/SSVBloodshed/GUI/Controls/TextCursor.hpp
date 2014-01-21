// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_TEXTCURSOR
#define SSVOB_GUI_CONTROLS_TEXTCURSOR

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"

namespace ob
{
	namespace GUI
	{
		namespace Internal
		{
			class TextCursor
			{
				private:
					AABBShape shape;
					int idxStart{0}, idxEnd{0}, idxMin, idxMax;
					float blinkValue{0.f};

					inline void delSelection(std::string& mStr)
					{
						mStr.erase(std::begin(mStr) + idxMin, std::begin(mStr) + idxMax);
						setBoth(idxMin);
					}
					inline void moveBoth(int mDir) noexcept	{ idxStart += mDir; idxEnd += mDir; }
					inline bool isSingle() const noexcept { return idxStart == idxEnd; }
					inline int getBreakGroup(char mChar) { if(ssvu::isSpace(mChar)) return 0; if(ssvu::isPunctuation(mChar)) return 1; return -1; }
					inline int findNextWordBeginEnd(std::string& mStr, int mPos, int mDir)
					{
						do
						{
							mPos += mDir;
						} while(getBreakGroup(mStr[mPos]) == getBreakGroup(mStr[mPos - std::abs(mDir)]));

						return mPos;
					}

				public:
					inline TextCursor(float mFontHeight) : shape{Vec2f{0.f, 0.f}, Vec2f{1.f, mFontHeight / 2.f}} { }

					inline void update(FT mFT, std::size_t mStrSize) noexcept
					{
						blinkValue += mFT * 0.08f;

						ssvu::clamp(idxStart, 0, mStrSize);
						ssvu::clamp(idxEnd, 0, mStrSize);
						idxMin = std::min(idxStart, idxEnd);
						idxMax = std::max(idxStart, idxEnd);
					}

					inline const AABBShape& getShape() { return shape; }

					inline void refreshShape(float mLeft, float mY, float mSpacing)
					{
						auto effect(std::abs(std::sin(blinkValue)));
						shape.setPosition(Vec2f{mLeft + ssvs::getGlobalWidth(shape) / 2.f, mY} + Vec2f(mSpacing * idxMin, 0.f));

						if(isSingle())
						{
							shape.setFillColor(sf::Color(0, 0, 0, 155 + effect * 100));
							shape.setWidth(2.f);
						}
						else
						{
							shape.setFillColor(sf::Color(0, 55, 200, 75 + effect * 75));
							shape.setWidth(mSpacing * (idxMax - idxMin) + 3.f);
						}
					}

					inline void setEnd(int mIdx) noexcept	{ idxEnd = mIdx; }
					inline void setBoth(int mIdx) noexcept	{ idxStart = idxEnd = mIdx; }

					inline void delStrBack(std::string& mStr)
					{
						if(!isSingle()) { delSelection(mStr); return; }

						auto itrStart(std::begin(mStr) + idxStart);
						if(itrStart > std::begin(mStr)) mStr.erase(itrStart - 1, itrStart);
						moveBoth(-1);
					}

					inline void delStrFwd(std::string& mStr)
					{
						if(!isSingle()) { delSelection(mStr); return; }

						auto itrStart(std::begin(mStr) + idxStart);
						if(itrStart < std::end(mStr)) mStr.erase(itrStart, itrStart + 1);
					}

					inline void insertStr(std::string& mStr, char mChar)
					{
						if(!isSingle()) delSelection(mStr);
						mStr.insert(std::begin(mStr) + idxMin, mChar);
						moveBoth(1);
					}

					inline void move(std::string& mStr, bool mShift, bool mCtrl, int mDir)
					{
						if(mShift)
						{
							if(mCtrl) idxEnd = findNextWordBeginEnd(mStr, idxEnd, mDir);
							else idxEnd += mDir;
						}
						else if(!isSingle())
						{
							if(mCtrl)
							{
								int& idxVar(mDir > 0 ? idxMax : idxMin);
								idxVar = findNextWordBeginEnd(mStr, idxVar, mDir);
								setBoth(idxVar);
							}
							else setBoth(mDir > 0 ? idxMax + 1 : idxMin - 1);
						}
						else
						{
							if(mCtrl)
							{
								idxStart = findNextWordBeginEnd(mStr, idxStart, mDir);
								setBoth(idxStart);
							}
							else moveBoth(mDir);
						}
					}
			};
		}
	}
}

#endif
