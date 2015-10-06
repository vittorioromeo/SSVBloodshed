// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_BRUSH
#define SSVOB_LEVELEDITOR_BRUSH

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
    class OBLEBrush : public sf::Drawable
    {
    private:
        int idx{0}, size{1};
        Vec2i position, center, min, max;
        sf::IntRect bounds;
        sf::RectangleShape single, full;

        inline void refresh()
        {
            center.x = ssvu::getClamped(
                position.x, bounds.left, (bounds.left + bounds.width - 1));
            center.y = ssvu::getClamped(
                position.y, bounds.top, (bounds.top + bounds.height - 1));

            min.x = ssvu::getClamped(position.x - size / 2, bounds.left,
                bounds.left + bounds.width - size);
            min.y = ssvu::getClamped(position.y - size / 2, bounds.top,
                bounds.top + bounds.height - size);

            max.x = ssvu::getClamped(position.x + std::ceil(size / 2.f),
                bounds.left + size, bounds.left + bounds.width);
            max.y = ssvu::getClamped(position.y + std::ceil(size / 2.f),
                bounds.top + size, bounds.top + bounds.height);

            single.setPosition(Vec2f(min * tileSize));
            full.setPosition(Vec2f(min * tileSize));
            full.setSize(Vec2f(size * tileSize, size * tileSize));
        }

    public:
        inline OBLEBrush(const sf::IntRect& mBounds)
            : bounds(mBounds), single{Vec2f(tileSize, tileSize)},
              full{Vec2f(tileSize, tileSize)}
        {
            single.setOrigin(tileSize / 2.f, tileSize / 2.f);
            single.setFillColor(sf::Color::Transparent);
            single.setOutlineColor({255, 255, 0, 125});
            single.setOutlineThickness(0.65f);

            full.setOrigin(tileSize / 2.f, tileSize / 2.f);
            full.setFillColor({255, 0, 0, 125});
            full.setOutlineColor({255, 255, 0, 125});
            full.setOutlineThickness(0.5f);
        }

        inline void draw(sf::RenderTarget& mRenderTarget,
            sf::RenderStates mRenderStates) const override
        {
            mRenderTarget.draw(single, mRenderStates);
            mRenderTarget.draw(full, mRenderStates);
        }

        inline void setPosition(const Vec2i& mPosition)
        {
            position = mPosition;
            refresh();
        }
        inline void setSize(int mSize)
        {
            size = mSize;
            refresh();
        }
        inline void setIdx(int mIdx) noexcept { idx = mIdx; }

        inline int getX() const noexcept { return center.x; }
        inline int getY() const noexcept { return center.y; }
        inline int getLeft() const noexcept { return min.x; }
        inline int getRight() const noexcept { return max.x; }
        inline int getTop() const noexcept { return min.y; }
        inline int getBottom() const noexcept { return max.y; }
        inline int getSize() const noexcept { return size; }
        inline int getIdx() const noexcept { return idx; }
    };
}

#endif
