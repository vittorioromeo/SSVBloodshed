// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_BUTTON
#define SSVOB_GUI_CONTROLS_BUTTON

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/ClickEffect.hpp"
#include "SSVBloodshed/GUI/Controls/Label.hpp"

namespace ob
{
    namespace GUI
    {
        class Button : public Widget
        {
        private:
            Label& lblLabel;
            Impl::ClickEffect clickEffect{getStyle().colorBtnUnpressed, *this};

            inline void update(FT mFT) override { clickEffect.update(mFT); }

        public:
            Button(Context& mContext, std::string mLabel, const Vec2f& mSize)
                : Widget{mContext, mSize / 2.f},
                  lblLabel(create<Label>(ssvu::mv(mLabel)))
            {
                setOutlineThickness(getStyle().outlineThickness);
                setOutlineColor(getStyle().colorOutline);
                lblLabel.attach(At::Center, *this, At::Center);
                onLeftClick += [this]
                {
                    clickEffect.click();
                };
            }

            inline Label& getLabel() noexcept { return lblLabel; }
        };
    }
}

#endif
