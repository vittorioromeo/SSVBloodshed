// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_SHUTTER
#define SSVOB_GUI_CONTROLS_SHUTTER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/Label.hpp"
#include "SSVBloodshed/GUI/Controls/Button.hpp"
#include "SSVBloodshed/GUI/Controls/Strip.hpp"

namespace ob
{
	namespace GUI
	{
		class Shutter : public Widget
		{
			private:
				Strip& wsBar;
				Strip& wsShutter;
				Button& btnOpen;
				Label& lblLabel;

				inline void update(FT) override
				{
					setSize(wsBar.getSize());
					if(!wsShutter.isAnyChildRecursive([](const Widget& mW){ return mW.isFocused(); })) wsShutter.setExcludedRecursive(true);
					wsShutter.setFillColor(wsShutter.isFocused() ? getStyle().colorBaseFocused : getStyle().colorBaseUnfocused);
				}

			public:
				Shutter(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext},
					wsBar(create<Strip>(At::Right, At::Left, At::Left)),
					wsShutter(create<Strip>(At::Top, At::Bottom, At::Bottom)),
					btnOpen(wsBar.create<Button>("v", getStyle().getBtnSquareSize())),
					lblLabel(wsBar.create<Label>(std::move(mLabel)))
				{
					setFillColor(sf::Color::Transparent);

					wsBar.setFillColor(sf::Color::Red);
					wsBar.setOutlineColor(getStyle().colorOutline);
					wsBar.setOutlineThickness(getStyle().outlineThickness);
					wsBar.setScaling(Scaling::Manual);
					wsBar.setSize(mSize);

					wsShutter.setOutlineColor(getStyle().colorOutline);
					wsShutter.setOutlineThickness(getStyle().outlineThickness);
					wsShutter.setExcludedRecursive(true);
					wsShutter.setContainer(true);
					wsShutter.setPadding(getStyle().padding * 2.f);
					wsShutter.setExternal(true);

					wsBar.attach(At::Right, *this, At::Right);
					wsShutter.attach(At::Top, wsBar, At::Bottom, Vec2f{0.f, getStyle().padding});

					btnOpen.onLeftClick += [this]
					{
						wsShutter.setExcludedSameDepth(false);
						wsShutter.gainExclusiveFocus();
					};
				}

				inline Shutter& operator+=(Widget& mWidget) { wsShutter += mWidget; return *this; }
				inline Shutter& operator+=(const std::initializer_list<Widget*> mWidgets) { wsShutter += mWidgets; return *this; }

				inline Strip& getBar() const noexcept		{ return wsBar; }
				inline Strip& getShutter() const noexcept	{ return wsShutter; }
				inline Label& getLabel() noexcept			{ return lblLabel; }
		};
	}
}

#endif
