// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_LEVEL
#define SSVOB_LEVELEDITOR_LEVEL

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/LevelEditor/OBLETile.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"

namespace ob
{
	class OBLESector;

	class OBLELevel
	{
		friend OBLESector;
		SSVJ_CNV_FRIEND();

		private:
			int cols{levelCols}, rows{levelRows}, depth{5};
			int x{0}, y{0};
			std::unordered_map<int, OBLETile> tiles;

		public:
			inline OBLELevel() = default;

			inline void clear(const OBLEDatabaseEntry& mDefaultEntry)
			{
				tiles.clear();
				for(int iX{0}; iX < cols; ++iX) for(int iY{0}; iY < rows; ++iY) getTile(iX, iY, 0).initFromEntry(mDefaultEntry);
			}

			inline void del(int mX, int mY, int mZ) { tiles.erase(ssvu::get1DIdxFrom3D(mX, mY, mZ, cols, rows)); }
			inline void del(OBLETile& mTile)		{ del(mTile.getX(), mTile.getY(), mTile.getZ()); }

			inline void update() { for(auto& t : tiles) t.second.update(); }
			inline void draw(sf::RenderTarget& mRenderTarget, bool mOnion, bool mShowId, int mCurrentZ = 0)
			{
				std::vector<std::pair<int, sf::Sprite>> toDraw;
				for(auto& t : tiles)
				{
					sf::Sprite s{t.second.getSprite()};
					if(mOnion) s.setColor(sf::Color(255, 255, 255, ssvu::getClamped(255 - std::abs(t.second.getZ() - mCurrentZ) * 50, 0, 255)));
					toDraw.emplace_back(t.second.getZ(), s);
				}

				ssvu::sortStable(toDraw, [](const auto& mA, const auto& mB){ return mA.first > mB.first; });
				for(auto& td : toDraw) mRenderTarget.draw(td.second);
				if(mShowId) for(auto& t : tiles) if(t.second.hasParam("id")) mRenderTarget.draw(t.second.getIdText());
			}

			inline int getColumns() const noexcept						{ return cols; }
			inline int getRows() const noexcept							{ return rows; }
			inline int getDepth() const noexcept						{ return depth; }
			inline int getWidth() const noexcept						{ return cols * 10; }
			inline int getHeight() const noexcept						{ return rows * 10; }
			inline int getX() const noexcept							{ return x; }
			inline int getY() const noexcept							{ return y; }
			inline bool isValid(int mX, int mY, int mZ) const noexcept	{ return mX >= 0 && mY >= 0 && mZ >= -depth && mX < cols && mY < rows && mZ < depth; }
			inline const decltype(tiles)& getTiles() const noexcept		{ return tiles; }
			inline decltype(tiles)& getTiles() noexcept					{ return tiles; }
			inline OBLETile& getTile(int mX, int mY, int mZ) noexcept	{ auto& t(tiles[ssvu::get1DIdxFrom3D(mX, mY, mZ, cols, rows)]); t.setX(mX); t.setY(mY); t.setZ(mZ); return t; }
	};
}

#endif
