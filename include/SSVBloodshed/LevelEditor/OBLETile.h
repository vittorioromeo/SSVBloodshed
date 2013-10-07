// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_TILE
#define SSVOB_LEVELEDITOR_TILE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.h"

namespace ob
{
	class OBLETile
	{
		template<typename> friend struct ssvuj::Internal::Converter;

		private:
			bool null{true};
			int x, y, z;
			OBLETType type;
			std::map<std::string, ssvuj::Obj> params;

			sf::Sprite sprite;
			ssvu::Uptr<ssvs::BitmapText> idText{nullptr};

		public:
			inline OBLETile() = default;
			inline OBLETile(const OBLETile& mTile) noexcept
			{
				x = mTile.x; y = mTile.y; z = mTile.z;
				type = mTile.type; params = mTile.params;
			}

			inline void setRot(int mDeg) noexcept { if(hasParam("rot")) params["rot"] = mDeg; }
			inline void setId(OBAssets& mAssets, int mId) noexcept
			{
				if(hasParam("id"))
				{
					if(idText == nullptr) idText.reset(new ssvs::BitmapText(*mAssets.obStroked));
					params["id"] = mId;
				}
			}

			inline void update()
			{
				sprite.setOrigin(sprite.getTextureRect().width / 2.f, sprite.getTextureRect().height / 2.f);
				sprite.setRotation(params.count("rot") > 0 ? ssvuj::as<int>(params["rot"]) : 0);
				sprite.setPosition(x * 10.f, y * 10.f);

				if(idText != nullptr)
				{
					idText->setPosition(x * 10.f - 4, y * 10.f - 5);
					idText->setString(ssvu::toStr(getParam<int>("id")));
				}
			}

			inline void initFromEntry(const OBLEDatabaseEntry& mEntry) noexcept
			{
				idText.reset(nullptr);
				null = false; type = mEntry.type; params = mEntry.defaultParams;
				sprite.setTexture(*mEntry.texture);
				sprite.setTextureRect(mEntry.intRect);
			}

			inline OBLETile& operator=(const OBLETile& mTile) noexcept
			{
				x = mTile.x; y = mTile.y; z = mTile.z;
				type = mTile.type; params = mTile.params;
				return *this;
			}

			inline bool isNull() const noexcept								{ return null; }
			inline void setX(int mX) noexcept								{ x = mX; }
			inline void setY(int mY) noexcept								{ y = mY; }
			inline void setZ(int mZ) noexcept								{ z = mZ; }
			inline void setType(OBLETType mType) noexcept					{ type = mType; null = false; }
			inline void setParams(const decltype(params)& mParams)			{ params = mParams; }
			template<typename T> inline T getParam(const std::string& mKey)	{ return ssvuj::as<T>(params[mKey]); }
			inline bool hasParam(const std::string& mKey) const noexcept	{ return params.count(mKey) > 0; }

			inline OBLETType getType() const noexcept					{ return type; }
			inline int getX() const noexcept							{ return x; }
			inline int getY() const noexcept							{ return y; }
			inline int getZ() const noexcept							{ return z; }
			inline const sf::Sprite& getSprite() const noexcept			{ return sprite; }
			inline sf::Sprite& getSprite() noexcept						{ return sprite; }
			inline const decltype(params)& getParams() const noexcept	{ return params; }
			inline decltype(params)& getParams() noexcept				{ return params; }
			inline ssvs::BitmapText* getIdText() noexcept				{ return idText.get(); }
	};
}

#endif

