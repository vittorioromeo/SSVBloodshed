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
				std::vector<Widget*> children;
				int depth{0};
				void render(const sf::Drawable& mDrawable);

			private:
				int zOrder{0};

				// Settings
				bool hidden{false}; // Controlled by hide/show: if true, it makes the widget implicitly invisible and inactive
				bool excluded{false}; // Controlled by setExcluded: if true, it makes the widget implicitly invisible and inactive
				bool active{true}, visible{true};

				// Status
				bool focused{false}, hovered{false}, pressed{false}, pressedPreviously{false};

				// Positioning
				Widget* neighbor{nullptr};
				At from{At::Center}, to{At::Center};
				Vec2f offset;

				inline virtual void update(float) { }
				inline virtual void postUpdate() { }
				inline virtual void draw() { }

				void updateWithChildren(float mFT);
				inline void drawWithChildren()
				{
					if(!isVisible()) return;
					recalculatePosition(); draw(); render(*this);
					for(auto& w : children) w->drawWithChildren();
				}

				inline void recalculatePosition() { if(neighbor != nullptr) setPosition(getVecPos(to, *neighbor) + offset + (this->getPosition() - getVecPos(from, *this))); }

				void checkHover();
				inline void checkUse()
				{
					pressedPreviously = pressed;
					pressed = isMBtnLeftDown() && hovered;
					for(const auto& c : children) c->checkUse();
				}
				inline void setFocused(bool mValue) { focused = mValue; for(auto& w : children) w->setFocused(w->depth >= depth && mValue); }

				inline void recalculateDepth(int mParentDepth = 0, int mParentDepthOffset = 0)
				{
					depth = mParentDepth + mParentDepthOffset;
					for(auto& w : children) w->recalculateDepth(depth, depthOffset);
				}

			public:
				int depthOffset{0};

				using AABBShape::AABBShape;

				Widget(Context& mContext) : context(mContext) { }
				Widget(Context& mContext, const Vec2f& mHalfSize) : AABBShape(Vec2f{0.f, 0.f}, mHalfSize), context(mContext) { }
				Widget(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize) : AABBShape(mPosition, mHalfSize), context(mContext) { }

				template<typename T, typename... TArgs> T& create(TArgs&&... mArgs);
				void destroy();

				void gainExclusiveFocus();

				inline void attach(At mFrom, Widget &mNeigh, At mTo, const Vec2f& mOffset = Vec2f{0.f, 0.f}) { from = mFrom; neighbor = &mNeigh; to = mTo; offset = mOffset; }
				inline void show() { setHidden(false); }
				inline void hide() { setHidden(true); }

				// An hidden widget is both invisible and inactive (should be controlled by collapsing windows)
				inline void setHidden(bool mValue)		{ hidden = mValue; for(auto& w : children) w->setHidden(mValue); }

				// An excluded widget is both invisible and inactive (should be used to completely disable a widget)
				inline void setExcluded(bool mValue)	{ excluded = mValue; for(auto& w : children) w->setExcluded(mValue); }

				inline void setActive(bool mValue) { active = mValue; for(auto& w : children) w->setActive(mValue); }
				inline void setVisible(bool mValue) { visible = mValue; for(auto& w : children) w->setVisible(mValue); }

				inline bool isFocused() const noexcept	{ return focused; }
				inline bool isHovered() const noexcept	{ return isActive() && hovered; }
				inline bool isVisible() const noexcept	{ return visible && !isHidden() && !isExcluded(); }
				inline bool isActive() const noexcept	{ return active && !isHidden() && !isExcluded(); }
				inline bool isPressed() const noexcept	{ return isHovered() && pressed; }
				inline bool isHidden() const noexcept	{ return hidden; }
				inline bool isExcluded() const noexcept	{ return excluded; }

				// Only these two should be used in widget code
				inline bool isClickedAlways() const noexcept 	{ return isFocused() && isPressed(); }
				inline bool isClickedOnce() const noexcept 		{ return isClickedAlways() && !wasPressed(); }

				inline bool isAnyChildPressed() const noexcept	{ for(const auto& w : children) if(w->isPressed()) return true; return isPressed(); }
				inline bool isAnyChildExclusiveFocusPressed(int mParentDepth = 0) const noexcept
				{
					for(const auto& w : children) if(w->isAnyChildExclusiveFocusPressed(depth)) return true;
					return depth > mParentDepth && isPressed();
				}

				bool wasPressed() const noexcept;
				const Vec2f& getMousePos() const noexcept;
				const Vec2f& getMousePosOld() const noexcept;
				bool isMBtnLeftDown() const noexcept;
		};
	}
}

#endif
