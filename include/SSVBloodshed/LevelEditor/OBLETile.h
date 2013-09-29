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
		std::map<std::string, ssvuj::Obj> defaultParams;
		inline OBLETileData() : type{OBLETType::LETFloor} { }
		inline OBLETileData(OBLETType mType, const std::map<std::string, ssvuj::Obj>& mParams = {}) : type{mType}, defaultParams{mParams} { }
	};

	struct OBLETileDataDrawable : public OBLETileData
	{
		sf::Texture* texture;
		sf::IntRect intRect;
		inline OBLETileDataDrawable() = default;
		inline OBLETileDataDrawable(OBLETType mType, sf::Texture* mTexture, const sf::IntRect& mIntRect, const std::map<std::string, ssvuj::Obj>& mParams = {}) : OBLETileData{mType, mParams}, texture{mTexture}, intRect{mIntRect} { }
	};

	struct OBLETile
	{
		private:
			bool null{true};
			int x, y, z;
			OBLETType type;
			sf::Sprite sprite;
			std::map<std::string, ssvuj::Obj> params;

		public:
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
				sprite.setPosition(x * 10.f, y * 10.f);
			}

			inline void initFromData(const OBLETileData& mData)
			{
				null = false;
				type = mData.type;
				params = mData.defaultParams;
			}
			inline void initFromDataDrawable(const OBLETileDataDrawable& mData)
			{
				initFromData(mData);
				sprite.setTexture(*mData.texture);
				sprite.setTextureRect(mData.intRect);
				sprite.setOrigin(5, 5);
			}

			inline bool isNull() const noexcept							{ return null; }
			inline void setX(int mX) noexcept							{ x = mX; }
			inline void setY(int mY) noexcept							{ y = mY; }
			inline void setZ(int mZ) noexcept							{ z = mZ; }
			inline void setType(OBLETType mType) noexcept				{ type = mType; null = false; }
			inline void setParams(const decltype(params)& mParams)		{ params = mParams; }

			inline OBLETType getType() const noexcept					{ return type; }
			inline int getX() const noexcept							{ return x; }
			inline int getY() const noexcept							{ return y; }
			inline int getZ() const noexcept							{ return z; }
			inline const sf::Sprite& getSprite() const noexcept			{ return sprite; }
			inline sf::Sprite& getSprite() noexcept						{ return sprite; }
			inline const decltype(params)& getParams() const noexcept	{ return params; }
			inline decltype(params)& getParams() noexcept				{ return params; }
	};
}

#endif
