// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_LABEL
#define SSVOB_GUI_CONTROLS_LABEL

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"

namespace ob
{
	namespace GUI
	{
		class Label : public Widget
		{
			private:
				ssvs::BitmapText text;
				bool scaleWithText{true};

				inline void drawWidget() override { text.setPosition(getX(), getY() - 1.f); render(text); }

			public:
				Label(Context& mContext, std::string mText = "") : Widget{mContext}, text{getStyle().font}
				{
					text.setColor(getStyle().colorText); text.setTracking(-3);
					setFillColor(getStyle().colorLabelBG);
					setString(std::move(mText));
				}

				inline void setScaleWithText(bool mValue) noexcept { scaleWithText = mValue; }
				inline void setString(std::string mLabel)
				{
					text.setString(std::move(mLabel));
					text.setOrigin(ssvs::getGlobalHalfSize(text));
					if(scaleWithText) setSize(ssvs::getGlobalSize(text));
				}
				inline const std::string& getString() const noexcept { return text.getString(); }
				inline decltype(text)& getText() noexcept { return text; }
		};
	}
}

#endif
