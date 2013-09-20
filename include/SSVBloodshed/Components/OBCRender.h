// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_RENDER
#define SSVOB_COMPONENTS_RENDER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCRender : public sses::Component
	{
		private:
			OBGame& game;
			ssvsc::Body& body;
			std::vector<sf::Sprite> sprites;
			std::vector<Vec2f> spriteOffsets;
			bool flippedX{false}, flippedY{false}, scaleWithBody{false};
			Vec2f offset;

		public:
			OBCRender(OBGame& mGame, ssvsc::Body& mBody) : game(mGame), body(mBody) { }

			void update(float) override
			{
				const auto& position(toPixels(body.getPosition()));
				for(auto i(0u); i < sprites.size(); ++i)
				{
					auto& s(sprites[i]);

					const auto& rect(s.getTextureRect());
					s.setOrigin({rect.width / 2.f, rect.height / 2.f});
					s.setPosition(position + offset + spriteOffsets[i]);
					s.setScale(flippedX ? -1 : 1, flippedY ? -1 : 1);

					if(!scaleWithBody) continue;
					const auto& size(toPixels(body.getSize()));
					s.setScale(size.x / rect.width, size.y / rect.height);
				}
			}
			inline void draw() override { for(const auto& s : sprites) game.render(s); }

			inline void addSprite(const sf::Sprite& mSprite) { sprites.push_back(mSprite); spriteOffsets.emplace_back(0.f, 0.f); }

			inline bool isFlippedX() const						{ return flippedX; }
			inline bool isFlippedY() const						{ return flippedY; }
			inline std::vector<sf::Sprite>& getSprites()		{ return sprites; }
			inline std::vector<Vec2f>& getSpriteOffsets()		{ return spriteOffsets; }
			inline sf::Sprite& operator[](unsigned int mIdx)	{ return sprites[mIdx]; }

			inline void setRotation(float mDegrees)				{ for(auto& s : sprites) s.setRotation(mDegrees); }
			inline void setFlippedX(bool mFlippedX)				{ flippedX = mFlippedX; }
			inline void setFlippedY(bool mFlippedY)				{ flippedY = mFlippedY; }
			inline void setScaleWithBody(bool mScaleWithBody)	{ scaleWithBody = mScaleWithBody; }
			inline void setOffset(const Vec2f& mOffset)			{ offset = mOffset; }
	};
}

#endif
