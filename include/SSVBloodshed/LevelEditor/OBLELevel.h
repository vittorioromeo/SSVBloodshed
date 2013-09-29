// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_LEVEL
#define SSVOB_LEVELEDITOR_LEVEL

#include <unordered_map>
#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/LevelEditor/OBLETile.h"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.h"

namespace ob
{
	class OBLELevel
	{
		private:
			int columns{0}, rows{0}, depth{5};
			std::unordered_map<int, OBLETile> tiles;

		public:
			inline OBLELevel() = default;
			inline OBLELevel(int mColumns, int mRows) : columns{mColumns}, rows{mRows} { }
			inline OBLELevel(int mColumns, int mRows, const OBLEDatabaseEntry& mDefaultEntry) : OBLELevel{mColumns, mRows}
			{
				for(int iY{0}; iY < mRows; ++iY) for(int iX{0}; iX < mColumns; ++iX) getTile(iX, iY, 0).initFromEntry(mDefaultEntry);
			}

			inline void del(int mX, int mY, int mZ) { tiles.erase(ssvu::get1DIndexFrom3D(mX, mY, mZ, columns, rows)); }
			inline void del(OBLETile& mTile)		{ del(mTile.getX(), mTile.getY(), mTile.getZ()); }

			inline void update() { for(auto& t : tiles) t.second.update(); }
			inline void draw(sf::RenderTarget& mRenderTarget, bool mOnion = false, int mCurrentZ = 0)
			{
				std::vector<std::pair<int, sf::Sprite>> toDraw;
				for(auto& t : tiles)
				{
					sf::Sprite s{t.second.getSprite()};
					if(mOnion) s.setColor({255, 255, 255, ssvu::getClamped<unsigned char>(255 - std::abs(t.second.getZ() - mCurrentZ) * 50, 0, 255)});
					toDraw.emplace_back(t.second.getZ(), s);
				}

				ssvu::sortStable(toDraw, [](const std::pair<int, sf::Sprite>& mA, const std::pair<int, sf::Sprite>& mB){ return mA.first > mB.first; });
				for(auto& td : toDraw) mRenderTarget.draw(td.second);
			}

			inline void saveToFile(const ssvu::FileSystem::Path& mPath)
			{
				unsigned int idx{0}; ssvuj::Obj root;
				for(auto& p : tiles)
				{
					auto& t(p.second); if(t.isNull()) continue;
					ssvuj::set(root, idx++, ssvuj::getArchArray(t.getX(), t.getY(), t.getZ(), t.getType(), t.getParams()));
				}
				ssvuj::writeToFile(root, mPath);
			}
			inline void loadFromFile(const ssvu::FileSystem::Path& mPath, OBLEDatabase* mDatabase = nullptr)
			{
				tiles.clear();

				ssvuj::Obj root{ssvuj::readFromFile(mPath)};
				for(auto itr(ssvuj::begin(root)); itr != ssvuj::end(root); ++itr)
				{
					int x, y, z, type; std::map<std::string, ssvuj::Obj> params;
					ssvuj::extrArray(*itr, x, y, z, type, params);

					auto& t(getTile(x, y, z));
					if(mDatabase != nullptr) t.initFromEntry(mDatabase->get(OBLETType(type)));
					t.setX(x); t.setY(y); t.setZ(z); t.setType(OBLETType(type)); t.setParams(params);
				}
			}

			inline bool isValid(int mX, int mY, int mZ) const noexcept	{ return mX >= 0 && mY >= 0 && mZ >= -depth && mX < columns && mY < rows && mZ < depth; }
			inline const decltype(tiles)& getTiles() const noexcept		{ return tiles; }
			inline decltype(tiles)& getTiles() noexcept					{ return tiles; }
			inline OBLETile& getTile(int mX, int mY, int mZ) noexcept	{ auto& t(tiles[ssvu::get1DIndexFrom3D(mX, mY, mZ, columns, rows)]); t.setX(mX); t.setY(mY); t.setZ(mZ); return t; }
	};
}

#endif
