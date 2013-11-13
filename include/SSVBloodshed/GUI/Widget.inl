// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_WIDGET_INL
#define SSVOB_GUI_WIDGET_INL

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/GUI/Widget.h"

namespace ob
{
	namespace GUI
	{
		inline void Widget::gainExclusiveFocus() { context.unFocusAll(); setFocused(true); }
		inline void Widget::render(const sf::Drawable& mDrawable) { context.render(mDrawable); }
		inline void Widget::destroy() { context.del(*this); for(const auto& c : children) c->destroy(); }
		inline void Widget::checkHover()
		{
			hovered = isOverlapping(getMousePos());
			if(hovered) context.hovered = true;
			for(auto& w : children) w->checkHover();
		}

		inline void Widget::updateWithChildren(float mFT)
		{
			ssvu::eraseRemoveIf(children, [](const Widget* mW){ return ssvu::MemoryManager<Widget>::isDead(mW); });
			update(mFT);
			if(isPressed()) context.busy = true;
			for(auto& w : children) w->updateWithChildren(mFT);
			postUpdate();
		}
		inline const Vec2f& Widget::getMousePos() const noexcept	{ return context.mousePos; }
		inline const Vec2f& Widget::getMousePosOld() const noexcept	{ return context.mousePosOld; }
		inline bool Widget::isMBtnLeftDown() const noexcept			{ return isActive() && context.mouseDown; }
		inline bool Widget::wasPressed() const noexcept				{ return context.mouseDownOld || (isHovered() && pressedPreviously); }

		template<typename T, typename... TArgs> inline T& Widget::create(TArgs&&... mArgs)
		{
			auto& result(context.allocateWidget<T>(std::forward<TArgs>(mArgs)...));
			result.setHidden(isHidden());
			result.setExcluded(isExcluded());
			result.setActive(isActive());
			result.setVisible(isVisible());
			//result.depth = depth + depthOffset;
			children.push_back(&result); return result;
		}
	}
}

#endif

