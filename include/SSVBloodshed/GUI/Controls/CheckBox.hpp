// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_CHECKBOX
#define SSVOB_GUI_CONTROLS_CHECKBOX

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/ClickEffect.hpp"
#include "SSVBloodshed/GUI/Controls/Label.hpp"

namespace ob
{
namespace GUI
{
    namespace Impl
    {
        class CheckBoxStateBox : public Widget
        {
        private:
            Label& lblState;
            Impl::ClickEffect clickEffect{getStyle().colorBtnUnpressed, *this};

            inline void update(FT mFT) override { clickEffect.update(mFT); }

        public:
            CheckBoxStateBox(Context& mContext)
                : Widget{mContext}, lblState(create<Label>(""))
            {
                setOutlineThickness(getStyle().outlineThickness);
                setOutlineColor(getStyle().colorOutline);
                setSize(getStyle().getBtnSquareSize());
                lblState.attach(At::Center, *this, At::Center);
            }

            inline void setState(bool mValue)
            {
                clickEffect.click();
                lblState.setString(mValue ? "x" : "");
            }
            inline Label& getLabel() noexcept { return lblState; }
        };
    }

    class CheckBox : public Widget
    {
    private:
        Impl::CheckBoxStateBox& cbsbBox;
        Label& lblLabel;
        bool state{false};

    public:
        ssvu::Delegate<void()> onStateChanged;

        CheckBox(Context& mContext, std::string mLabel, bool mState = false)
            : Widget{mContext}, cbsbBox(create<Impl::CheckBoxStateBox>()),
              lblLabel(create<Label>(ssvu::mv(mLabel)))
        {
            setScaling(Scaling::FitToChildren);
            setFillColor(sf::Color::Transparent);
            setState(mState);

            cbsbBox.attach(At::NW, *this, At::NW);
            lblLabel.attach(At::Left, cbsbBox, At::Right,
            Vec2f{getStyle().outlineThickness, 0.f});

            onLeftClick += [this]
            {
                setState(!state);
            };
        }

        inline void setState(bool mValue)
        {
            state = mValue;
            cbsbBox.setState(mValue);
            onStateChanged();
        }
        inline bool getState() const noexcept { return state; }
        inline Label& getLabel() noexcept { return lblLabel; }
    };
}
}

#endif
