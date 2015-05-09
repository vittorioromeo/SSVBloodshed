// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_FORMBAR
#define SSVOB_GUI_CONTROLS_FORMBAR

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/Label.hpp"
#include "SSVBloodshed/GUI/Controls/Button.hpp"
#include "SSVBloodshed/GUI/Controls/Strip.hpp"

namespace ob
{
	namespace GUI
	{
		class FormBar : public Widget
		{
			private:
				Strip& wsBtns;
				Button& btnClose;
				Button& btnMinimize;
				Button& btnCollapse;
				Label& lblTitle;

			public:
				FormBar(Context& mContext, std::string mTitle) : Widget{mContext},
					wsBtns(create<Strip>(At::Right, At::Left, At::Left)),
					btnClose(wsBtns.create<Button>("x", getStyle().getBtnSquareSize())),
					btnMinimize(wsBtns.create<Button>("_", getStyle().getBtnSquareSize())),
					btnCollapse(wsBtns.create<Button>("^", getStyle().getBtnSquareSize())),
					lblTitle(create<Label>(ssvu::mv(mTitle)))
				{
					external = true;
					setFillColor(sf::Color::Black);

					wsBtns.setWidgetOffset(getStyle().outlineThickness);

					wsBtns.attach(At::Right, *this, At::Right, Vec2f{-getStyle().padding, 0.f});
					lblTitle.attach(At::NW, *this, At::NW, Vec2f{0.f, getStyle().padding});
				}

				inline Button& getBtnClose() const noexcept		{ return btnClose; }
				inline Button& getBtnMinimize() const noexcept	{ return btnMinimize; }
				inline Button& getBtnCollapse() const noexcept	{ return btnCollapse; }
				inline Label& getTitle() noexcept				{ return lblTitle; }
		};
	}
}

#endif
