// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_WIDGET
#define SSVOB_GUI_WIDGET

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/GUI/At.h"
#include "SSVBloodshed/GUI/AABBShape.h"
#include "SSVBloodshed/GUI/Widget.h"

namespace ob
{
	namespace GUI
	{
		class Context;

		class Widget : public AABBShape, public ssvu::MemoryManageable
		{
			friend class Context;

			public:
				enum class Scaling{Manual, FitToParent, FitToNeighbor, FitToChildren};

			protected:
				Context& context;
				std::vector<Widget*> children;
				Widget* parent{nullptr};
				bool external{false}; // If true, this widget will not be taken into account for scaling
				void render(const sf::Drawable& mDrawable);

			private:
				int depth{0};
				bool container{false}; // If true, children have a deeper depth

				// Dirtyness
				bool dirty{true};
				Vec2f positionOld, sizeOld;

				// Settings
				bool hidden{false}; // Controlled by hide/show: if true, it makes the widget implicitly invisible and inactive
				bool excluded{false}; // Controlled by setExcludedRecursive: if true, it makes the widget implicitly invisible and inactive
				bool active{true}, visible{true};

				// Status
				bool focused{false}, hovered{false}, pressed{false}, pressedOld{false};

				// Positioning
				Widget* neighbor{nullptr};
				At from{At::Center}, to{At::Center};
				Vec2f offset;

				// Scaling
				Scaling scalingX{Scaling::Manual}, scalingY{Scaling::Manual};
				float padding{0.f};

				inline virtual void update(float) { }
				inline virtual void draw() { }
				inline virtual void refreshIfDirty() { }

				void updateRecursive(float mFT);
				inline void drawHierarchy()
				{
					auto hierarchy(getAllRecursive());
					ssvu::sortStable(hierarchy, [](const Widget* mA, const Widget* mB){ return mA->depth < mB->depth; });
					for(auto& w : hierarchy)
					{
						if(!w->isVisible()) continue;
						w->recalculateSize(w->scalingX, &Widget::setWidth, &Widget::getLeft, &Widget::getRight);
						w->recalculateSize(w->scalingY, &Widget::setHeight, &Widget::getTop, &Widget::getBottom);
						w->recalculatePosition();
						w->draw(); render(*w);
					}
				}
				inline void refreshDirtyRecursive()
				{
					for(auto& w : children) w->refreshDirtyRecursive();
					if(dirty) { refreshIfDirty(); dirty = false; }
				}

				inline void recalculatePosition()
				{
					if(neighbor == nullptr) return;

					positionOld = getPosition();
					setPosition(getVecPos(to, *neighbor) + offset + (this->getPosition() - getVecPos(from, *this)));
					if(positionOld != getPosition()) dirty = true;
				}
				template<typename TS, typename TG> inline void recalculateSize(Scaling mScaling, TS mSetter, TG mGetterMin, TG mGetterMax)
				{
					if(mScaling == Scaling::Manual) return;

					sizeOld = getSize();

					if(mScaling == Scaling::FitToParent)
					{
						if(parent == nullptr) return;
						(this->*mSetter)(((parent->*mGetterMax)() - (parent->*mGetterMin)()) - padding * 2.f);
					}
					else if(mScaling == Scaling::FitToNeighbor)
					{
						if(neighbor == nullptr) return;
						(this->*mSetter)(((neighbor->*mGetterMax)() - (neighbor->*mGetterMin)()) - padding * 2.f);
					}
					else if(mScaling == Scaling::FitToChildren)
					{
						if(children.empty()) return;

						bool found{false};
						float dMin{std::numeric_limits<float>::max()}, dMax{std::numeric_limits<float>::min()};

						for(const auto& w : children)
						{
							if(w->isHidden() || w->isExcluded() || w->external) continue;

							found = true;

							dMin = std::min(dMin, (w->*mGetterMin)());
							dMax = std::max(dMax, (w->*mGetterMax)());
						}

						if(found) (this->*mSetter)(dMax - dMin + padding * 2.f);
						else (this->*mSetter)(0.f);
					}

					if(sizeOld != getSize()) dirty = true;
				}

				void checkHover();
				inline void checkPressed()
				{
					pressedOld = pressed;
					pressed = isMBtnLeftDown() && hovered;
				}

				inline void setFocusedRecursive(bool mValue) { dirty = true; focused = mValue; for(auto& w : children) w->setFocusedRecursive(mValue); }
				inline void setFocusedSameDepth(bool mValue) { dirty = true; focused = mValue; for(auto& w : children) if(w->depth == depth) w->setFocusedSameDepth(mValue); }

				inline void recalculateDepth() { depth = parent == nullptr ? 0 : parent->depth + static_cast<int>(container); }

			public:
				using AABBShape::AABBShape;

				Widget(Context& mContext) : context(mContext) { }
				Widget(Context& mContext, const Vec2f& mHalfSize) : AABBShape(Vec2f{0.f, 0.f}, mHalfSize), context(mContext) { }
				Widget(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize) : AABBShape(mPosition, mHalfSize), context(mContext) { }

				template<typename T, typename... TArgs> T& create(TArgs&&... mArgs);
				void destroyRecursive();
				void gainExclusiveFocus();

				inline void attach(At mFrom, Widget &mNeigh, At mTo, const Vec2f& mOffset = Vec2f{0.f, 0.f}) { dirty = true; from = mFrom; neighbor = &mNeigh; to = mTo; offset = mOffset; }
				inline void show() { setHiddenRecursive(false); }
				inline void hide() { setHiddenRecursive(true); }

				// An hidden widget is both invisible and inactive (should be controlled by collapsing windows)
				inline void setHiddenRecursive(bool mValue)		{ dirty = true; hidden = mValue; for(auto& w : children) w->setHiddenRecursive(mValue); }

				// An excluded widget is both invisible and inactive (should be used to completely disable a widget)
				inline void setExcludedRecursive(bool mValue)	{ dirty = true; excluded = mValue; for(auto& w : children) w->setExcludedRecursive(mValue); }
				inline void setExcludedSameDepth(bool mValue)	{ dirty = true; excluded = mValue; for(auto& w : children) if(w->depth == depth) w->setExcludedSameDepth(mValue); }

				inline void setActiveRecursive(bool mValue)		{ dirty = true; active = mValue; for(auto& w : children) w->setActiveRecursive(mValue); }
				inline void setVisibleRecursive(bool mValue)	{ dirty = true; visible = mValue; for(auto& w : children) w->setVisibleRecursive(mValue); }
				inline void setContainer(bool mValue)			{ dirty = true; container = mValue; }
				inline void setParent(Widget& mWidget)
				{
					dirty = true;
					if(parent != nullptr) ssvu::eraseRemove(parent->children, this);
					parent = &mWidget;
					mWidget.children.push_back(this);
					setHiddenRecursive(mWidget.isHidden());
					setExcludedRecursive(mWidget.isExcluded());
					setActiveRecursive(mWidget.isActive());
					setVisibleRecursive(mWidget.isVisible());
				}
				inline void setScalingX(Scaling mValue) noexcept	{ dirty = true; scalingX = mValue; }
				inline void setScalingY(Scaling mValue) noexcept	{ dirty = true; scalingY = mValue; }
				inline void setScaling(Scaling mValue) noexcept		{ dirty = true; setScalingX(mValue); setScalingY(mValue); }
				inline void setPadding(float mValue) noexcept		{ dirty = true; padding = mValue; }

				inline bool isFocused() const noexcept		{ return focused; }
				inline bool isHovered() const noexcept		{ return isActive() && hovered; }
				inline bool isVisible() const noexcept		{ return visible && !isHidden() && !isExcluded(); }
				inline bool isActive() const noexcept		{ return active && !isHidden() && !isExcluded(); }
				inline bool isPressed() const noexcept		{ return isHovered() && pressed; }
				inline bool isHidden() const noexcept		{ return hidden; }
				inline bool isExcluded() const noexcept		{ return excluded; }
				inline bool isContainer() const noexcept	{ return container; }

				// Only these two should be used in widget code
				inline bool isClickedAlways() const noexcept 	{ return isFocused() && isPressed(); }
				inline bool isClickedOnce() const noexcept 		{ return isClickedAlways() && !wasPressed(); }

				inline bool isAnyChildFocused() const noexcept	{ for(auto& w : children) if(w->isAnyChildFocused()) return true; if(isFocused()) return true; return false; }
				inline bool isAnyChildPressed() const noexcept	{ for(auto& w : children) if(w->isAnyChildPressed()) return true; if(isPressed()) return true; return false; }
				inline float getPadding() const noexcept		{ return padding; }
				inline decltype(children)& getChildren() noexcept { return children; }

				inline void fillAllRecursive(std::vector<Widget*>& mTarget)		{ mTarget.push_back(this); for(const auto& w : children) w->fillAllRecursive(mTarget); }
				inline std::vector<Widget*> getAllRecursive() 					{ std::vector<Widget*> result; fillAllRecursive(result); return result; }

				bool wasPressed() const noexcept;
				const Vec2f& getMousePos() const noexcept;
				const Vec2f& getMousePosOld() const noexcept;
				bool isMBtnLeftDown() const noexcept;
		};
	}
}

#endif
