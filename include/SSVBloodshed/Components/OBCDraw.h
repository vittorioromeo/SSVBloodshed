// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_RENDER
#define SSVOB_COMPONENTS_RENDER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCDraw : public sses::Component
	{
		private:
			OBGame& game;
			ssvsc::Body& body;
			std::vector<sf::Sprite> sprites;
			std::vector<Vec2f> offsets;
			bool flippedX{false}, flippedY{false}, scaleWithBody{false};
			Vec2f globalOffset;

		public:
			inline OBCDraw(OBGame& mGame, ssvsc::Body& mBody) : game(mGame), body(mBody) { }

			inline void update(float) override
			{
				const auto& position(toPixels(body.getPosition()));
				const auto& size(toPixels(body.getSize()));

				for(auto i(0u); i < sprites.size(); ++i)
				{
					auto& s(sprites[i]);

					const auto& rect(s.getTextureRect());
					s.setOrigin({rect.width / 2.f, rect.height / 2.f});
					s.setPosition(position + globalOffset + offsets[i]);
					s.setScale(flippedX ? -1 : 1, flippedY ? -1 : 1);

					if(scaleWithBody) s.setScale(size.x / rect.width, size.y / rect.height);
				}
			}
			inline void draw() override { for(const auto& s : sprites) game.render(s); }

			inline void addSprite(const sf::Sprite& mSprite) { sprites.push_back(mSprite); offsets.emplace_back(); }
			template<typename... TArgs> inline void emplaceSprite(TArgs&&... mArgs) { sprites.emplace_back(std::forward<TArgs>(mArgs)...); offsets.emplace_back(); }

			inline void setRotation(float mDegrees)	noexcept			{ for(auto& s : sprites) s.setRotation(mDegrees); }
			inline void setFlippedX(bool mFlippedX)	noexcept			{ flippedX = mFlippedX; }
			inline void setFlippedY(bool mFlippedY)	noexcept			{ flippedY = mFlippedY; }
			inline void setScaleWithBody(bool mScale) noexcept			{ scaleWithBody = mScale; }
			inline void setGlobalOffset(const Vec2f& mOffset) noexcept	{ globalOffset = mOffset; }

			inline bool isFlippedX() const noexcept							{ return flippedX; }
			inline bool isFlippedY() const noexcept							{ return flippedY; }
			inline const decltype(sprites)& getSprites() const noexcept		{ return sprites; }
			inline const decltype(offsets)& getOffsets() const noexcept		{ return offsets; }
			inline decltype(sprites)& getSprites() noexcept					{ return sprites; }
			inline decltype(offsets)& getOffsets() noexcept					{ return offsets; }
			inline sf::Sprite& operator[](unsigned int mIdx)				{ return sprites[mIdx]; }
			inline const sf::Sprite& operator[](unsigned int mIdx) const	{ return sprites[mIdx]; }
	};
}

#endif
