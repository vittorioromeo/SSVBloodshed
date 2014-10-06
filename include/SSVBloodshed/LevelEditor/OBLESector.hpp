// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_SECTOR
#define SSVOB_LEVELEDITOR_SECTOR

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"

namespace ob
{
	class OBLESector
	{
		SSVJ_CNV_FRIEND();

		private:
			int cols, rows;
			std::unordered_map<int, OBLELevel> levels;

		public:
			inline OBLESector(int mCols = 100, int mRows = 100) noexcept : cols{mCols}, rows{mRows} { }
			inline void clear() noexcept { levels.clear(); }

			inline int getColumns() const noexcept						{ return cols; }
			inline int getRows() const noexcept							{ return rows; }
			inline const decltype(levels)& getLevels() const noexcept	{ return levels; }
			inline decltype(levels)& getLevels() noexcept				{ return levels; }
			inline OBLELevel& getLevel(int mX, int mY) noexcept			{ auto& l(levels[ssvu::get1DIdxFrom2D(mX, mY, cols)]); l.x = mX; l.y = mY; return l; }
			inline bool isValid(int mX, int mY) const noexcept			{ return levels.count(ssvu::get1DIdxFrom2D(mX, mY, cols)) > 0; }
	};
}

#endif
