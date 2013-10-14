// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_SECTOR
#define SSVOB_LEVELEDITOR_SECTOR

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/LevelEditor/OBLELevel.h"

namespace ob
{
	class OBLESector
	{
		template<typename> friend struct ssvuj::Converter;

		private:
			int columns, rows;
			std::unordered_map<int, OBLELevel> levels;

		public:
			inline OBLESector(int mColumns = 100, int mRows = 100) noexcept : columns{mColumns}, rows{mRows} { }
			inline void clear() { levels.clear(); }
			inline void init(OBLEDatabase& mDatabase) { for(auto& p : levels) p.second.init(mDatabase); }

			inline int getColumns() const noexcept						{ return columns; }
			inline int getRows() const noexcept							{ return rows; }
			inline const decltype(levels)& getLevels() const noexcept	{ return levels; }
			inline decltype(levels)& getLevels() noexcept				{ return levels; }
			inline OBLELevel& getLevel(int mX, int mY) noexcept			{ auto& l(levels[ssvu::get1DIdxFrom2D(mX, mY, columns)]); l.x = mX; l.y = mY; return l; }
			inline bool isValid(int mX, int mY) noexcept				{ return levels.count(ssvu::get1DIdxFrom2D(mX, mY, columns)) > 0; }
	};
}

#endif
