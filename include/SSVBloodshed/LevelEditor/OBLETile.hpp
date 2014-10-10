// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_TILE
#define SSVOB_LEVELEDITOR_TILE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"

namespace ob
{
	struct OBLETileData
	{
		SSVJ_CNV_FRIEND();

		std::map<std::string, ssvj::Val> params;
		OBLETType type{OBLETType::LETNull};
		int x{-1}, y{-1}, z{-1};
	};

	class OBLETile
	{
		SSVJ_CNV_FRIEND();

		private:
			OBLETileData data;
			sf::Sprite sprite;
			ssvs::BitmapText idText;

		public:
			inline OBLETile() { }
			//inline OBLETile(const OBLETile& mTile) noexcept : data{mTile.data}, params{mTile.params} { }

			inline void refreshIdText(OBAssets& mAssets)
			{
				idText = ssvs::BitmapText{*mAssets.obStroked};

				idText.setScale(0.5f, 0.5f);
				idText.setTracking(-3);

				auto id(getParam<int>("id"));
				idText.setPosition(data.x * 10.f - 4, data.y * 10.f - 5);
				idText.setString(id == 0 ? "" : ssvu::toStr(id));
			}

			inline void setRot(int mDeg) noexcept { if(hasParam("rot")) data.params["rot"] = mDeg; }
			inline void setId(OBAssets& mAssets, int mId) noexcept
			{
				if(!hasParam("id")) return;
				data.params["id"] = mId;
				refreshIdText(mAssets);
			}

			inline void setParam(const std::string& mKey, const std::string& mValue)
			{
				auto& p(data.params[mKey]);
				try
				{
					if(p.isNum<ssvj::IntS>()) p = std::stoi(mValue);
					else if(p.isNum<ssvj::Real>()) p = std::stof(mValue);
					else if(p.is<bool>()) p = mValue == "true" ? true : false;
				}
				catch(const std::exception& mError)
				{
					ssvu::lo("ob::OBLETile::setParam") << "Error setting parameter <" << mKey << ">: <" << mValue << ">!\n";
					ssvu::lo("ob::OBLETile::setParam") << mError.what() << std::endl;
				}
			}

			inline void update()
			{
				sprite.setOrigin(sprite.getTextureRect().width / 2.f, sprite.getTextureRect().height / 2.f);
				sprite.setRotation(data.params.count("rot") > 0 ? data.params["rot"].as<int>() : 0);
				sprite.setPosition(data.x * 10.f, data.y * 10.f);
			}

			inline void initFromEntry(const OBLEDatabaseEntry& mEntry) noexcept
			{
				data.type = mEntry.type;
				data.params = mEntry.defaultParams;
			}
			inline void initGfxFromEntry(const OBLEDatabaseEntry& mEntry) noexcept
			{
				sprite.setTexture(*mEntry.texture);
				sprite.setTextureRect(mEntry.intRect);
			}

			inline bool isNull() const noexcept								{ return data.type == OBLETType::LETNull; }
			inline void setX(int mX) noexcept								{ data.x = mX; }
			inline void setY(int mY) noexcept								{ data.y = mY; }
			inline void setZ(int mZ) noexcept								{ data.z = mZ; }
			inline void setType(OBLETType mType) noexcept					{ data.type = mType; }
			inline void setParams(decltype(data.params) mParams)			{ data.params = std::move(mParams); }
			template<typename T> inline T getParam(const std::string& mKey)	{ return data.params[mKey].as<T>(); }
			inline bool hasParam(const std::string& mKey) const noexcept	{ return data.params.count(mKey) > 0; }

			inline auto getType() const noexcept			{ return data.type; }
			inline auto getX() const noexcept				{ return data.x; }
			inline auto getY() const noexcept				{ return data.y; }
			inline auto getZ() const noexcept				{ return data.z; }
			inline const auto& getSprite() const noexcept	{ return sprite; }
			inline auto& getSprite() noexcept				{ return sprite; }
			inline const auto& getParams() const noexcept	{ return data.params; }
			inline auto& getParams() noexcept				{ return data.params; }
			inline auto& getIdText() noexcept				{ return idText; }
			inline const auto& getData() const noexcept		{ return data; }
	};
}

#endif

