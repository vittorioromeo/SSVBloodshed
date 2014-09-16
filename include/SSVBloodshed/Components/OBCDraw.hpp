// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_RENDER
#define SSVOB_COMPONENTS_RENDER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"

namespace ob
{
	class OBCDraw : public sses::Component
	{
		private:
			OBGame& game;
			Body& body;
			std::vector<sf::Sprite> sprites;
			std::vector<Vec2f> offsets;
			int flippedX{1}, flippedY{1};
			bool scaleWithBody{false};
			Vec2f globalOffset, globalScale{1.f, 1.f};
			sf::BlendMode blendMode{sf::BlendMode::BlendAlpha};

		public:
			inline OBCDraw(OBGame& mGame, Body& mBody) noexcept : game(mGame), body(mBody) { }

			inline void update(FT) override
			{
				const auto& position(toPixels(body.getPosition()));
				const auto& size(body.getSize());

				for(auto i(0u); i < sprites.size(); ++i)
				{
					auto& s(sprites[i]);

					const auto& rect(s.getTextureRect());
					s.setOrigin({rect.width / 2.f, rect.height / 2.f});
					s.setPosition(position + globalOffset + offsets[i]);
					s.setScale(globalScale.x * flippedX, globalScale.y * flippedY);

					if(scaleWithBody) s.setScale(toPixels(size.x) / rect.width, toPixels(size.y) / rect.height);
				}
			}
			inline void draw() override { for(const auto& s : sprites) game.render(s, blendMode); }

			template<typename... TArgs> inline void emplaceSprite(TArgs&&... mArgs)	{ sprites.emplace_back(ssvu::fwd<TArgs>(mArgs)...); offsets.emplace_back(); }

			inline void rotate(float mDeg) noexcept						{ for(auto& s : sprites) s.rotate(mDeg); }

			inline void setRotation(float mDeg)	noexcept				{ for(auto& s : sprites) s.setRotation(mDeg); }
			inline void setFlippedX(bool mFlippedX)	noexcept			{ flippedX = mFlippedX ? -1 : 1; }
			inline void setFlippedY(bool mFlippedY)	noexcept			{ flippedY = mFlippedY ? -1 : 1; }
			inline void setScaleWithBody(bool mScale) noexcept			{ scaleWithBody = mScale; }
			inline void setGlobalOffset(const Vec2f& mOffset) noexcept	{ globalOffset = mOffset; }
			inline void setGlobalScale(float mFactor) noexcept			{ globalScale.x = globalScale.y = mFactor; }
			inline void setGlobalScale(float mX, float mY) noexcept		{ globalScale.x = mX; globalScale.y = mY; }
			inline void setGlobalScale(const Vec2f& mScale) noexcept	{ globalScale = mScale; }
			inline void setBlendMode(sf::BlendMode mMode) noexcept		{ blendMode = mMode; }

			inline OBGame& getGame() const noexcept							{ return game; }
			inline const decltype(sprites)& getSprites() const noexcept		{ return sprites; }
			inline const decltype(offsets)& getOffsets() const noexcept		{ return offsets; }
			inline decltype(sprites)& getSprites() noexcept					{ return sprites; }
			inline decltype(offsets)& getOffsets() noexcept					{ return offsets; }
			inline sf::Sprite& operator[](unsigned int mIdx)				{ return sprites[mIdx]; }
			inline const sf::Sprite& operator[](unsigned int mIdx) const	{ return sprites[mIdx]; }
			inline bool isFlippedX() const noexcept							{ return flippedX == -1; }
			inline bool isFlippedY() const noexcept							{ return flippedY == -1; }
			inline sf::BlendMode getBlendMode() const noexcept				{ return blendMode; }
	};
}

#endif
