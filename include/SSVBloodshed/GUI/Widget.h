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

			protected:
				Context& context;
				bool external{false}; // If true, this widget will not be taken into account for scaling
				void render(const sf::Drawable& mDrawable);
				const std::vector<sf::Event>& getEventsToPoll() const noexcept;

			private:
				std::vector<Widget*> children;
				Widget* parent{nullptr};
				int depth{0};
				bool container{false}; // If true, children have a deeper depth
				sf::View view;
				Vec2f childBoundsMin, childBoundsMax, viewBoundsMin, viewBoundsMax;

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
				Scaling nextTempScaling{Scaling::Manual};
				float padding{0.f};

				inline virtual void update(float) { }
				inline virtual void draw() { }

				inline void drawHierarchy()
				{
					auto hierarchy(getAllRecursive());
					ssvu::sortStable(hierarchy, [](const Widget* mA, const Widget* mB){ return mA->depth < mB->depth; });
					for(auto& w : hierarchy) if(w->isVisible()) { w->draw(); render(*w); }
				}

				template<typename TS, typename TG> inline void fitToParentImpl(TS mSetter, TG mGetterMin, TG mGetterMax)
				{
					if(parent != nullptr) (this->*mSetter)(((parent->*mGetterMax)() - (parent->*mGetterMin)()) - padding * 2.f);
				}
				template<typename TS, typename TG> inline void fitToNeighborImpl(TS mSetter, TG mGetterMin, TG mGetterMax)
				{
					if(neighbor != nullptr) (this->*mSetter)(((neighbor->*mGetterMax)() - (neighbor->*mGetterMin)()) - padding * 2.f);
				}
				template<typename TS> inline void fitToChildrenImpl(TS mSetter, float mMin, float mMax)
				{
					if(!children.empty()) (this->*mSetter)(mMax - mMin + padding * 2.f);
				}

				inline void recalculatePosition()
				{
					if(neighbor != nullptr) setPosition(getVecPos(to, *neighbor) + offset + (getPosition() - getVecPos(from, *this)));
				}
				inline void recalculateChildBounds()
				{
					childBoundsMin = childBoundsMax = getPosition();

					for(const auto& w : children)
					{
						if(w->isHidden() || w->isExcluded() || w->external) continue;

						childBoundsMin.x = std::min(childBoundsMin.x, w->getLeft());
						childBoundsMax.x = std::max(childBoundsMax.x, w->getRight());
						childBoundsMin.y = std::min(childBoundsMin.y, w->getTop());
						childBoundsMax.y = std::max(childBoundsMax.y, w->getBottom());
					}
				}
				inline void recalculateViewBoundsImpl(Vec2f& mMin, Vec2f& mMax)
				{
					mMin.x = std::min(mMin.x, getLeft());
					mMax.x = std::max(mMax.x, getRight());
					mMin.y = std::min(mMin.y, getTop());
					mMax.y = std::max(mMax.y, getBottom());

					for(auto& w : children) if(w->isVisible()) w->recalculateViewBoundsImpl(mMin, mMax);
				}
				inline void recalculateViewBounds()
				{
					viewBoundsMin = getVertexNW(); viewBoundsMax = getVertexSE();
					recalculateViewBoundsImpl(viewBoundsMin, viewBoundsMax);
				}
				template<typename TS, typename TG> inline void recalculateSize(Scaling mScaling, TS mSetter, TG mGetterMin, TG mGetterMax)
				{
					if(mScaling == Scaling::FitToParent) fitToParentImpl(mSetter, mGetterMin, mGetterMax);
					else if(mScaling == Scaling::FitToNeighbor) fitToNeighborImpl(mSetter, mGetterMin, mGetterMax);
				}
				inline void recalculateFitToChildren(Scaling mScalingX, Scaling mScalingY)
				{
					if(mScalingX == Scaling::FitToChildren) fitToChildrenImpl(&Widget::setWidth, childBoundsMin.x, childBoundsMax.x);
					if(mScalingY == Scaling::FitToChildren) fitToChildrenImpl(&Widget::setHeight, childBoundsMin.y, childBoundsMax.y);
				}

				inline void setFocusedSameDepth(bool mValue) { focused = mValue; for(auto& w : children) if(w->depth == depth) w->setFocusedSameDepth(mValue); }
				inline void recalculateDepth() noexcept { depth = parent == nullptr ? 0 : parent->depth + static_cast<int>(container); }
				inline void cleanUpMemoryRecursive()
				{
					ssvu::eraseRemoveIf(children, &ssvu::MemoryManager<Widget>::isDead<Widget*>);
					for(auto& w : children) w->cleanUpMemoryRecursive();
				}

				void checkMouse();
				void updateRecursive(float mFT);
				void recalculateView();

			public:
				using AABBShape::AABBShape;

				Widget(Context& mContext) : context(mContext) { }
				Widget(Context& mContext, const Vec2f& mHalfSize) : AABBShape(Vec2f{0.f, 0.f}, mHalfSize), context(mContext) { }
				Widget(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize) : AABBShape(mPosition, mHalfSize), context(mContext) { }
				virtual ~Widget() { }

				inline void attach(At mFrom, Widget &mNeigh, At mTo, const Vec2f& mOffset = Vec2f{0.f, 0.f}) { from = mFrom; neighbor = &mNeigh; to = mTo; offset = mOffset; }
				inline void show() { setHiddenRecursive(false); }
				inline void hide() { setHiddenRecursive(true); }

				inline void fitToParent()	{ nextTempScaling = Scaling::FitToParent; }
				inline void fitToNeighbor()	{ nextTempScaling = Scaling::FitToNeighbor; }
				inline void fitToChildren()	{ nextTempScaling = Scaling::FitToChildren; }

				// An hidden widget is both invisible and inactive (should be controlled by collapsing windows)
				inline void setHiddenRecursive(bool mValue)		{ hidden = mValue; for(auto& w : children) w->setHiddenRecursive(mValue); }

				// An excluded widget is both invisible and inactive (should be used to completely disable a widget)
				inline void setExcludedRecursive(bool mValue)	{ excluded = mValue; for(auto& w : children) w->setExcludedRecursive(mValue); }
				inline void setExcludedSameDepth(bool mValue)	{ excluded = mValue; for(auto& w : children) if(w->depth == depth) w->setExcludedSameDepth(mValue); }

				inline void setActiveRecursive(bool mValue)		{ active = mValue; for(auto& w : children) w->setActiveRecursive(mValue); }
				inline void setVisibleRecursive(bool mValue)	{ visible = mValue; for(auto& w : children) w->setVisibleRecursive(mValue); }
				inline void setContainer(bool mValue)			{ container = mValue; }
				inline void setParent(Widget& mWidget)
				{
					if(parent != nullptr) ssvu::eraseRemove(parent->children, this);

					parent = &mWidget;
					mWidget.children.push_back(this);

					setHiddenRecursive(mWidget.isHidden());
					setExcludedRecursive(mWidget.isExcluded());
					setActiveRecursive(mWidget.isActive());
					setVisibleRecursive(mWidget.isVisible());
				}
				inline void setScalingX(Scaling mValue) noexcept	{ scalingX = mValue; }
				inline void setScalingY(Scaling mValue) noexcept	{ scalingY = mValue; }
				inline void setScaling(Scaling mValue) noexcept		{ scalingX = scalingY = mValue; }
				inline void setPadding(float mValue) noexcept		{ padding = mValue; }

				inline bool isFocused() const noexcept		{ return focused; }
				inline bool isHovered() const noexcept		{ return isActive() && hovered; }
				inline bool isVisible() const noexcept		{ return visible && !isHidden() && !isExcluded(); }
				inline bool isActive() const noexcept		{ return active && !isHidden() && !isExcluded(); }
				inline bool isPressed() const noexcept		{ return isHovered() && pressed; }
				inline bool isHidden() const noexcept		{ return hidden; }
				inline bool isExcluded() const noexcept		{ return excluded; }
				inline bool isContainer() const noexcept	{ return container; }

				inline decltype(children)& getChildren() noexcept { return children; }

				// Only these two should be used in widget code
				inline bool isClickedAlways() const noexcept 	{ return isFocused() && isPressed(); }
				inline bool isClickedOnce() const noexcept 		{ return isClickedAlways() && !wasPressed(); }

				inline bool isAnyChildFocused() const noexcept	{ for(auto& w : children) if(w->isAnyChildFocused()) return true; if(isFocused()) return true; return false; }
				inline bool isAnyChildPressed() const noexcept	{ for(auto& w : children) if(w->isAnyChildPressed()) return true; if(isPressed()) return true; return false; }
				inline float getPadding() const noexcept		{ return padding; }
				inline Scaling getScalingX() const noexcept		{ return scalingX; }
				inline Scaling getScalingY() const noexcept		{ return scalingY; }

				inline void fillAllRecursive(std::vector<Widget*>& mTarget)		{ mTarget.push_back(this); for(const auto& w : children) w->fillAllRecursive(mTarget); }
				inline std::vector<Widget*> getAllRecursive() 					{ std::vector<Widget*> result; fillAllRecursive(result); return result; }

				template<typename T, typename... TArgs> T& create(TArgs&&... mArgs);
				void destroyRecursive();
				void gainExclusiveFocus();

				bool wasPressed() const noexcept;
				bool isMBtnLeftDown() const noexcept;
				const Vec2f& getMousePos() const noexcept;
				const Vec2f& getMousePosOld() const noexcept;
		};
	}
}

#endif

