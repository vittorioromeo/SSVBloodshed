// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_LEVEL
#define SSVOB_LEVELEDITOR_LEVEL

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/LevelEditor/OBLETile.h"

namespace ob
{
	class OBLELevel
	{
		private:
			int columns, rows;
			std::vector<OBLETile> tiles;

		public:
			inline OBLELevel(int mColumns, int mRows, const OBLETileData& mDefaultTileData) : columns{mColumns}, rows{mRows}
			{
				for(int iY{0}; iY < rows; ++iY) for(int iX{0}; iX < columns; ++iX) tiles.emplace_back(iX, iY, mDefaultTileData);
			}

			inline OBLETile& getTile(int mX, int mY)	{ return tiles[ssvu::get1DIndexFrom2D(mX, mY, columns)]; }
			inline bool isValid(int mX, int mY)			{ return mX >= 0 && mY >= 0 && mX < columns && mY < rows; }

			inline void update()								{ for(auto& t : tiles) t.update(); }
			inline void draw(sf::RenderTarget& mRenderTarget)	{ for(auto& t : tiles) mRenderTarget.draw(t.getSprite()); }

			inline void saveToFile(const ssvu::FileSystem::Path& mPath)
			{
				unsigned int idx{0}; ssvuj::Obj root;

				for(auto& t : tiles)
				{
					ssvuj::Obj tObj;
					ssvuj::archArray(tObj, t.getX(), t.getY(), t.getType(), t.getParams());
					ssvuj::set(root, idx, tObj);
					++idx;
				}

				ssvuj::writeToFile(root, mPath);
			}

			inline void loadFromFile(const ssvu::FileSystem::Path& mPath, std::map<OBLETType, OBLETileData>& mTileMap)
			{
				unsigned int idx{0}; ssvuj::Obj root{ssvuj::readFromFile(mPath)};

				for(auto& t : tiles)
				{
					int x, y, dataType; std::map<std::string, ssvuj::Obj> params;
					ssvuj::extrArray(ssvuj::get(root, idx), x, y, dataType, params);

					t.initFromData(mTileMap[static_cast<OBLETType>(dataType)]);
					t.setParams(params);
					++idx;
				}
			}
	};
}

#endif
