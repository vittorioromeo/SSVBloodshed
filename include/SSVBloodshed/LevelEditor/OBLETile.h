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
			inline void setRot(int mDeg)
			{
				if(params.count("rot") == 0) return;
				params["rot"] = mDeg;
			}

			inline void update()
			{
				if(params.count("rot") > 0) getSprite().setRotation(ssvuj::as<int>(params["rot"]));
				else getSprite().setRotation(0);

				Vec2f origin{sprite.getTextureRect().width / 2.f, sprite.getTextureRect().height / 2.f};
				sprite.setOrigin(origin);
				sprite.setPosition(x * 10.f, y * 10.f);
			}

			inline void initFromEntry(const OBLEDatabaseEntry& mEntry)
			{
				null = false;
				type = mEntry.type;
				params = mEntry.defaultParams;
				sprite.setTexture(*mEntry.texture);
				sprite.setTextureRect(mEntry.intRect);
			}

			inline bool isNull() const noexcept								{ return null; }
			inline void setX(int mX) noexcept								{ x = mX; }
			inline void setY(int mY) noexcept								{ y = mY; }
			inline void setZ(int mZ) noexcept								{ z = mZ; }
			inline void setType(OBLETType mType) noexcept					{ type = mType; null = false; }
			inline void setParams(const decltype(params)& mParams)			{ params = mParams; }
			template<typename T> inline T getParam(const std::string& mKey)	{ return ssvuj::as<T>(params[mKey]); }

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
