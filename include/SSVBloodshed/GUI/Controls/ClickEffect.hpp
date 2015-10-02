// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_CLICKEFFECT
#define SSVOB_GUI_CONTROLS_CLICKEFFECT

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"

namespace ob
{
namespace GUI
{
    namespace Impl
    {
        class ClickEffect
        {
        private:
            sf::Color color;
            AABBShape& target;
            float value{0.f};

        public:
            inline ClickEffect(const sf::Color& mColor,
            AABBShape& mTarget) noexcept : color{mColor},
                                           target(mTarget)
            {
            }
            inline void update(FT mFT) noexcept
            {
                auto colorNew(color);
                colorNew.g = value;
                target.setFillColor(colorNew);
                value = ssvu::getClampedMin(value - mFT * 15.f, 0.f);
            }
            inline void click() noexcept { value = 255; }
        };
    }
}
}

#endif
