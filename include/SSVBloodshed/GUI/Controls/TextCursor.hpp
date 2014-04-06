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
				friend class TextCursorShape;

				private:
					std::string& str;
					int idxStart{0}, idxEnd{0};

					inline int getIdxMin() const noexcept	{ return std::min(idxStart, idxEnd); }
					inline int getIdxMax() const noexcept	{ return std::max(idxStart, idxEnd); }

					inline void setIdxStart(int mIdx) noexcept	{ idxStart = ssvu::getClamped(mIdx, 0, getStrSize()); }
					inline void setIdxEnd(int mIdx) noexcept	{ idxEnd = ssvu::getClamped(mIdx, 0, getStrSize()); }

					inline std::size_t getStrSize() const noexcept	{ return str.size(); }
					inline std::string& getStr() noexcept			{ return str; }

					inline void delSelection()
					{
						str.erase(std::begin(str) + getIdxMin(), std::begin(str) + getIdxMax());
						setBoth(getIdxMin());
					}
					inline void moveBoth(int mDir) noexcept	{ setIdxStart(idxStart + mDir); setIdxEnd(idxEnd + mDir); }
					inline bool isSingle() const noexcept { return idxStart == idxEnd; }
					inline int getBreakGroup(char mChar) { if(ssvu::isSpace(mChar)) return 0; if(ssvu::isPunctuation(mChar)) return 1; return -1; }
					inline int findNextWordBeginEnd(int mPos, int mDir)
					{
						do
						{
							mPos += mDir;
						} while(getBreakGroup(str[mPos]) == getBreakGroup(str[mPos - std::abs(mDir)]));

						return mPos;
					}

					template<int TDir> inline void delImpl(bool mCtrl)
					{
						if(!isSingle()) { delSelection(); return; }
						if(mCtrl) { move(true, true, TDir); delSelection(); return; }

						auto itrStart(std::begin(str) + idxStart);

						if(TDir > 0)
						{
							if(itrStart < std::end(str)) str.erase(itrStart, itrStart + 1);
						}
						else if(TDir < 0)
						{
							if(itrStart > std::begin(str)) str.erase(itrStart - 1, itrStart);
							moveBoth(-1);
						}
					}

				public:
					inline TextCursor(std::string& mStr) : str(mStr) { }

					inline void setEnd(int mIdx) noexcept	{ setIdxEnd(mIdx); }
					inline void setBoth(int mIdx) noexcept	{ setIdxEnd(mIdx); setIdxStart(idxEnd); }

					inline void delStrBack(bool mCtrl) { delImpl<-1>(mCtrl); }
					inline void delStrFwd(bool mCtrl) { delImpl<1>(mCtrl); }

					inline void insertStr(char mChar)
					{
						if(!isSingle()) delSelection();
						str.insert(std::begin(str) + getIdxMin(), mChar);
						moveBoth(1);
					}

					inline void move(bool mShift, bool mCtrl, int mDir)
					{
						if(mShift)
						{
							if(mCtrl) setIdxEnd(findNextWordBeginEnd(idxEnd, mDir));
							else setIdxEnd(idxEnd + mDir);
						}
						else if(!isSingle())
						{
							if(mCtrl)
							{
								int idxVar(mDir > 0 ? getIdxMax() : getIdxMin());
								idxVar = findNextWordBeginEnd(idxVar, mDir);
								setBoth(idxVar);
							}
							else setBoth(mDir > 0 ? getIdxMax() + 1 : getIdxMin() - 1);
						}
						else
						{
							if(mCtrl)
							{
								setIdxStart(findNextWordBeginEnd(idxStart, mDir));
								setBoth(idxStart);
							}
							else moveBoth(mDir);
						}
					}
			};

			class TextCursorShape
			{
				private:
					AABBShape shape;
					float blinkValue{0.f};

				public:
					inline TextCursorShape(float mFontHeight) : shape{ssvs::zeroVec2f, Vec2f{1.f, mFontHeight / 2.f}} { }

					inline void update(FT mFT) noexcept { blinkValue += mFT * 0.08f; }
					inline void refreshShape(TextCursor& mTextCursor, float mLeft, float mY, float mSpacing)
					{
						auto effect(std::abs(std::sin(blinkValue)));
						shape.setPosition(Vec2f{mLeft + ssvs::getGlobalWidth(shape) / 2.f, mY} + Vec2f(mSpacing * mTextCursor.getIdxMin(), 0.f));

						if(mTextCursor.isSingle())
						{
							shape.setFillColor(sf::Color(0, 0, 0, 155 + effect * 100));
							shape.setWidth(2.f);
						}
						else
						{
							shape.setFillColor(sf::Color(0, 55, 200, 75 + effect * 75));
							shape.setWidth(mSpacing * (mTextCursor.getIdxMax() - mTextCursor.getIdxMin()) + 3.f);
						}
					}

					inline const AABBShape& getShape() { return shape; }
			};
		}
	}
}

#endif
