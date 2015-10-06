// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_FORM
#define SSVOB_GUI_CONTROLS_FORM

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/FormBar.hpp"

namespace ob
{
    namespace GUI
    {
        class Form : public Widget
        {
        private:
            enum class Action
            {
                None,
                Move,
                Resize
            };

            FormBar& fbBar;
            Widget& fbResizer;
            bool draggable{true}, resizable{true}, collapsed{false};
            Action action;
            float oldHeight;
            Scaling oldScalingX, oldScalingY;
            Vec2f dragOrigin;

            inline void update(FT) override
            {
                setFillColor(isFocused() ? getStyle().colorBaseFocused
                                         : getStyle().colorBaseUnfocused);

                if(action == Action::Move)
                    setPosition(getMousePos() - dragOrigin);
                else if(action == Action::Resize)
                {
                    auto oldNW(getVertexNW());
                    auto newWidth(getMousePos().x - dragOrigin.x);
                    auto newHeight(getMousePos().y - dragOrigin.y);

                    newWidth = std::max(20.f, newWidth);
                    newHeight = std::max(20.f, newHeight);

                    setWidth(newWidth);
                    setHeight(newHeight);

                    setPosition(oldNW + getHalfSize());
                }
            }

        public:
            Form(Context& mContext, std::string mTitle, const Vec2f& mPosition,
                const Vec2f& mSize)
                : Widget{mContext, mPosition, mSize / 2.f},
                  fbBar(create<FormBar>(ssvu::mv(mTitle))),
                  fbResizer(create<Widget>(Vec2f{4.f, 4.f}))
            {
                setOutlineThickness(getStyle().outlineThickness);
                setOutlineColor(getStyle().colorOutline);

                fbBar.getBtnClose().onLeftClick += [this]
                {
                    hide();
                };
                fbBar.getBtnMinimize().onLeftClick += [this]
                { /* TODO */ };
                fbBar.getBtnCollapse().onLeftClick += [this]
                {
                    toggleCollapsed();
                };
                fbBar.setScalingX(Scaling::FitToNeighbor);
                fbBar.setScalingY(Scaling::FitToChildren);
                fbBar.setPadding(getStyle().padding);

                fbResizer.setFillColor(sf::Color::Transparent);
                fbResizer.setOutlineThickness(getStyle().outlineThickness);
                fbResizer.setOutlineColor(getStyle().colorOutline);

                fbBar.attach(At::Bottom, *this, At::Top);
                fbResizer.attach(At::SE, *this, At::SE);

                fbBar.onLeftClickDown += [this]
                {
                    if(draggable)
                    {
                        action = Action::Move;
                        dragOrigin = getMousePos() - getPosition();
                    }
                };
                fbBar.onLeftRelease += [this]
                {
                    if(action == Action::Move) action = Action::None;
                };
                fbResizer.onLeftClickDown += [this]
                {
                    if(resizable)
                    {
                        action = Action::Resize;
                        dragOrigin = getMousePos() - getSize();
                    }
                };
                fbResizer.onLeftRelease += [this]
                {
                    if(action == Action::Resize) action = Action::None;
                };
            }

            inline void toggleCollapsed()
            {
                action = Action::None;

                if(!collapsed)
                {
                    oldHeight = getHeight();
                    oldScalingX = getScalingX();
                    oldScalingY = getScalingY();
                    setScaling(Scaling::Manual);
                    fbBar.getBtnCollapse().getLabel().setString("v");
                    resizeFromBottom(1.f);
                }
                else
                {
                    setScalingX(oldScalingX);
                    setScalingY(oldScalingY);
                    fbBar.getBtnCollapse().getLabel().setString("^");
                    resizeFromBottom(oldHeight);
                }

                collapsed = !collapsed;
                recurseChildrenIf<false>(
                    [this](Widget& mW)
                    {
                        return &mW != &fbBar;
                    },
                    [this](Widget& mW)
                    {
                        mW.setCollapsedRecursive(collapsed);
                    });
            }

            inline void setDraggable(bool mValue) noexcept
            {
                draggable = mValue;
            }
            inline void setResizable(bool mValue)
            {
                resizable = mValue;
                fbBar.getBtnMinimize().setExcludedRecursive(!mValue);
                fbResizer.setExcludedRecursive(!mValue);
            }
            inline void setTitle(std::string mTitle)
            {
                fbBar.getTitle().setString(ssvu::mv(mTitle));
            }
            inline const std::string& getTitle() noexcept
            {
                return fbBar.getTitle().getString();
            }
            inline bool isCollapsed() const noexcept { return collapsed; }
        };
    }
}

#endif
