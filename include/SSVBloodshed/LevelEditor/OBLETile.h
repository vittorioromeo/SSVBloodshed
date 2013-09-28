// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_TILE
#define SSVOB_LEVELEDITOR_TILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"

namespace ob
{
	struct OBLETileData
	{
		OBLETType type;
		sf::Texture* texture;
		sf::IntRect intRect;
		std::map<std::string, ssvuj::Obj> defaultParams;

		inline OBLETileData() : type{OBLETType::LETFloor} { }
		inline OBLETileData(OBLETType mType, sf::Texture* mTexture, const sf::IntRect& mIntRect, const std::map<std::string, ssvuj::Obj>& mParams = {}) : type{mType}, texture{mTexture}, intRect{mIntRect}, defaultParams{mParams} { }
	};

	struct OBLETile
	{
		private:
			int x, y;
			OBLETType type;
			sf::Sprite sprite;
			std::map<std::string, ssvuj::Obj> params;

		public:
			inline OBLETile(int mX, int mY, const OBLETileData& mData) : x{mX}, y{mY} { initFromData(mData); }

			inline void rotate(int mDeg)
			{
				if(params.count("rot") == 0) return;
				int currentRot(ssvuj::as<int>(params["rot"]));
				currentRot += mDeg;
				params["rot"] = currentRot;
			}

			inline void update()
			{
				if(params.count("rot") > 0) getSprite().setRotation(ssvuj::as<int>(params["rot"]));
				getSprite().setColor(sf::Color::White);
			}

			inline void initFromData(const OBLETileData& mData)
			{
				type = mData.type;
				sprite.setTexture(*mData.texture);
				sprite.setTextureRect(mData.intRect);
				params = mData.defaultParams;

				sprite.setOrigin(5, 5);
				sprite.setPosition(x * 10.f, y * 10.f);
			}

			inline void setParams(const decltype(params)& mParams)		{ params = mParams; }

			inline OBLETType getType() const noexcept					{ return type; }
			inline int getX() const noexcept							{ return x; }
			inline int getY() const noexcept							{ return y; }
			inline const sf::Sprite& getSprite() const noexcept			{ return sprite; }
			inline sf::Sprite& getSprite() noexcept						{ return sprite; }
			inline const decltype(params)& getParams() const noexcept	{ return params; }
			inline decltype(params)& getParams()noexcept				{ return params; }
	};
}

#endif
