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
	class OBLESector;

	class OBLELevel
	{
		friend class OBLESector;
		template<typename> friend struct ssvuj::Internal::Converter;

		private:
			int columns{0}, rows{0}, depth{5}, x{0}, y{0};
			std::unordered_map<int, OBLETile> tiles;

		public:
			inline OBLELevel() = default;
			inline OBLELevel(int mColumns, int mRows) : columns{mColumns}, rows{mRows} { }
			inline OBLELevel(int mColumns, int mRows, const OBLEDatabaseEntry& mDefaultEntry) : OBLELevel{mColumns, mRows}
			{
				for(int iY{0}; iY < mRows; ++iY) for(int iX{0}; iX < mColumns; ++iX) getTile(iX, iY, 0).initFromEntry(mDefaultEntry);
			}

			inline void clear() { tiles.clear(); }
			inline void init(OBLEDatabase& mDatabase)
			{
				for(auto& p : tiles)
				{
					auto tempTile(p.second);
					auto& t(p.second);

					t.initFromEntry(mDatabase.get(OBLETType(t.getType())));
					t = tempTile;
				}
			}

			inline void del(int mX, int mY, int mZ) { tiles.erase(ssvu::get1DIdxFrom3D(mX, mY, mZ, columns, rows)); }
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
				for(auto& t : tiles) if(t.second.getIdText() != nullptr) mRenderTarget.draw(*t.second.getIdText());
			}

			inline int getColumns() const noexcept						{ return columns; }
			inline int getRows() const noexcept							{ return rows; }
			inline int getDepth() const noexcept						{ return depth; }
			inline int getX() const noexcept							{ return x; }
			inline int getY() const noexcept							{ return y; }
			inline bool isValid(int mX, int mY, int mZ) const noexcept	{ return mX >= 0 && mY >= 0 && mZ >= -depth && mX < columns && mY < rows && mZ < depth; }
			inline const decltype(tiles)& getTiles() const noexcept		{ return tiles; }
			inline decltype(tiles)& getTiles() noexcept					{ return tiles; }
			inline decltype(tiles) getTilesNonNull() const noexcept		{ decltype(tiles) result; for(const auto& p : tiles) if(!p.second.isNull()) result.insert(p); return result; }
			inline OBLETile& getTile(int mX, int mY, int mZ) noexcept	{ auto& t(tiles[ssvu::get1DIdxFrom3D(mX, mY, mZ, columns, rows)]); t.setX(mX); t.setY(mY); t.setZ(mZ); return t; }
	};

	class OBLESector
	{
		template<typename> friend struct ssvuj::Internal::Converter;

		private:
			int columns, rows;
			std::unordered_map<int, OBLELevel> levels;

		public:
			inline OBLESector(int mColumns = 100, int mRows = 100) : columns{mColumns}, rows{mRows} { }
			inline void clear() { levels.clear(); }
			inline void init(OBLEDatabase& mDatabase) { for(auto& p : levels) p.second.init(mDatabase); }

			inline int getColumns() const noexcept						{ return columns; }
			inline int getRows() const noexcept							{ return rows; }
			inline const decltype(levels)& getLevels() const noexcept	{ return levels; }
			inline decltype(levels)& getLevels() noexcept				{ return levels; }
			inline OBLELevel& getLevel(int mX, int mY) noexcept			{ auto& l(levels[ssvu::get1DIdxFrom2D(mX, mY, columns)]); l.x = mX; l.y = mY; return l; }
	};
}

namespace ssvuj
{
	namespace Internal
	{
		template<> struct Converter<ob::OBLETType>
		{
			using T = ob::OBLETType;
			inline static void fromObj(T& mValue, const Obj& mObj)	{ ssvuj::extrEnum<T, int>(mObj, mValue); }
			inline static void toObj(Obj& mObj, const T& mValue)	{ ssvuj::archEnum<T, int>(mObj, mValue); }
		};

		template<> struct Converter<ob::OBLETile>
		{
			using T = ob::OBLETile;
			inline static void fromObj(T& mValue, const Obj& mObj)	{ ssvuj::extrArray(mObj, mValue.x, mValue.y, mValue.z, mValue.type, mValue.params); }
			inline static void toObj(Obj& mObj, const T& mValue)	{ ssvuj::archArray(mObj, mValue.x, mValue.y, mValue.z, mValue.type, mValue.params); }
		};

		template<> struct Converter<ob::OBLELevel>
		{
			using T = ob::OBLELevel;
			inline static void fromObj(T& mValue, const Obj& mObj)	{ ssvuj::extrArray(mObj, mValue.x, mValue.y, mValue.columns, mValue.rows, mValue.depth, mValue.tiles); }
			inline static void toObj(Obj& mObj, const T& mValue)	{ ssvuj::archArray(mObj, mValue.x, mValue.y, mValue.columns, mValue.rows, mValue.depth, mValue.getTilesNonNull()); }
		};

		template<> struct Converter<ob::OBLESector>
		{
			using T = ob::OBLESector;
			inline static void fromObj(T& mValue, const Obj& mObj)	{ ssvuj::extr(mObj, mValue.levels); }
			inline static void toObj(Obj& mObj, const T& mValue)	{ ssvuj::arch(mObj, mValue.levels); }
		};
	}
}

#endif
