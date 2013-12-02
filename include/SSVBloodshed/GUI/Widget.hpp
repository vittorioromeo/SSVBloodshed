// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_WIDGET
#define SSVOB_GUI_WIDGET

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/At.hpp"
#include "SSVBloodshed/GUI/AABBShape.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"

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
				const Vec2f& getMousePos() const noexcept;

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
				bool focused{false}, hovered{false}, pressedLeft{false}, pressedRight{false};

				// Positioning
				Widget* neighbor{nullptr};
				At from{At::Center}, to{At::Center};
				Vec2f offset;

				// Scaling
				Scaling scalingX{Scaling::Manual}, scalingY{Scaling::Manual};
				float padding{0.f}, scalePercent{100.f};

				inline virtual void update(FT) { }
				inline virtual void draw() { }

				inline void drawHierarchy()
				{
					recurseChildrenBF([this](Widget& mW){ if(mW.isVisible()) { mW.draw(); render(mW); } });
				}

				template<typename TS, typename TG> inline void fitToParentImpl(TS mSetter, TG mGetterMin, TG mGetterMax)
				{
					if(parent != nullptr) (this->*mSetter)((((parent->*mGetterMax)() - (parent->*mGetterMin)()) * (scalePercent / 100.f)) - padding * 2.f);
				}
				template<typename TS, typename TG> inline void fitToNeighborImpl(TS mSetter, TG mGetterMin, TG mGetterMax)
				{
					if(neighbor != nullptr) (this->*mSetter)((((neighbor->*mGetterMax)() - (neighbor->*mGetterMin)()) * (scalePercent / 100.f)) - padding * 2.f);
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

				inline void recalculateDepth() noexcept { depth = parent == nullptr ? 0 : parent->depth + static_cast<int>(container); }

				inline void setFocused(bool mValue)
				{
					if(focused != mValue)
					{
						onFocusChanged(mValue);
						recurseParents([mValue](Widget& mW){ mW.onAnyChildFocusChanged(mValue); });
					}
					focused = mValue;
				}

				inline void updateRecursive(FT mFT)
				{
					update(mFT);

					// Recalculate sizing
					recalculateSize(scalingX, &Widget::setWidth, &Widget::getLeft, &Widget::getRight);
					recalculateSize(scalingY, &Widget::setHeight, &Widget::getTop, &Widget::getBottom);
					recalculateFitToChildren(scalingX, scalingY);

					recalculatePosition();

					for(auto& w : children) w->updateRecursive(mFT);

					recalculateChildBounds();
					recalculateView();

					onPostUpdate();

					recurseChildrenBF<true, true>([this](Widget& mW){ mW.updateInput(); });
				}
				void recalculateView();
				void updateInput();

			public:
				using AABBShape::AABBShape;

				ssvu::Delegate<void(bool)> onFocusChanged, onAnyChildFocusChanged;
				ssvu::Delegate<void()> onPostUpdate;
				ssvu::Delegate<void()> onLeftClick, onLeftClickDown, onLeftRelease;
				ssvu::Delegate<void()> onRightClick, onRightClickDown, onRightRelease;

				Widget(Context& mContext) : context(mContext) { }
				Widget(Context& mContext, const Vec2f& mHalfSize) : AABBShape(Vec2f{0.f, 0.f}, mHalfSize), context(mContext) { }
				Widget(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize) : AABBShape(mPosition, mHalfSize), context(mContext) { }
				virtual ~Widget() { }

				template<bool TIncludeCaller = true, typename T> inline void recurseChildren(const T& mFunc)
				{
					if(TIncludeCaller) mFunc(*this);
					for(const auto& w : children) w->recurseChildren<true, T>(mFunc);
				}
				template<bool TIncludeCaller = true, bool TReverse = false, typename T> inline void recurseChildrenBF(const T& mFunc)
				{
					std::vector<Widget*> hierarchy; hierarchy.reserve(25);
					recurseChildren<TIncludeCaller>([&hierarchy](Widget& mW){ hierarchy.push_back(&mW); });
					ssvu::sortStable(hierarchy, [](const Widget* mA, const Widget* mB){ return mA->depth < mB->depth == !TReverse; });
					for(const auto& w : hierarchy) mFunc(*w);
				}
				template<bool TIncludeCaller = true, typename T1, typename T2> inline void recurseChildrenIf(const T1& mPred, const T2& mFunc)
				{
					if(TIncludeCaller)
					{
						if(!mPred(*this)) return;
						mFunc(*this);
					}
					for(const auto& w : children) w->recurseChildrenIf<true, T1, T2>(mPred, mFunc);
				}
				template<bool TIncludeCaller = true, typename T> inline void recurseParents(const T& mFunc)
				{
					if(TIncludeCaller) mFunc(*this);
					if(parent != nullptr) parent->recurseParents<true, T>(mFunc);
				}
				template<bool TIncludeCaller = true, typename T> inline bool isAnyChildRecursive(const T& mFunc) const
				{
					if(TIncludeCaller) if(mFunc(*this)) return true;
					for(const auto& w : children) if(w->isAnyChildRecursive<true, T>(mFunc)) return true;
					return false;
				}

				inline void attach(At mFrom, Widget &mNeigh, At mTo, const Vec2f& mOffset = ssvs::zeroVec2f) { from = mFrom; neighbor = &mNeigh; to = mTo; offset = mOffset; }
				inline void show() { setHiddenRecursive(false); }
				inline void hide() { setHiddenRecursive(true); }

				// An hidden widget is both invisible and inactive (should be controlled by collapsing windows)
				inline void setHidden(bool mValue) noexcept		{ hidden = mValue; }
				inline void setHiddenRecursive(bool mValue)		{ recurseChildren([mValue](Widget& mW){ mW.setHidden(mValue); }); }

				// An excluded widget is both invisible and inactive (should be used to completely disable a widget)
				inline void setExcluded(bool mValue) noexcept	{ excluded = mValue; }
				inline void setExcludedRecursive(bool mValue)	{ recurseChildren([mValue](Widget& mW){ mW.setExcluded(mValue); });  }
				inline void setExcludedSameDepth(bool mValue)	{ recurseChildrenIf([this](Widget& mW){ return mW.depth == depth; }, [mValue](Widget& mW){ mW.setExcluded(mValue); }); }

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
				inline void setScalePercent(float mValue) noexcept	{ scalePercent = mValue; }

				inline bool isFocused() const noexcept		{ return focused; }
				inline bool isHovered() const noexcept		{ return isActive() && hovered; }
				inline bool isVisible() const noexcept		{ return visible && !isHidden() && !isExcluded(); }
				inline bool isActive() const noexcept		{ return active && !isHidden() && !isExcluded(); }
				inline bool isPressedAny() const noexcept	{ return pressedLeft || pressedRight; }
				inline bool isHidden() const noexcept		{ return hidden; }
				inline bool isExcluded() const noexcept		{ return excluded; }
				inline bool isContainer() const noexcept	{ return container; }

				inline decltype(children)& getChildren() noexcept { return children; }

				inline float getPadding() const noexcept		{ return padding; }
				inline Scaling getScalingX() const noexcept		{ return scalingX; }
				inline Scaling getScalingY() const noexcept		{ return scalingY; }

				template<typename T, typename... TArgs> T& create(TArgs&&... mArgs);
				void destroyRecursive();
				void gainExclusiveFocus();
		};
	}
}

#endif

