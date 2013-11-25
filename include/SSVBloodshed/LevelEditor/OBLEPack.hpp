// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_PACK
#define SSVOB_LEVELEDITOR_PACK

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/LevelEditor/OBLESector.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"

namespace ob
{
	class OBLEPack
	{
		template<typename> friend struct ssvuj::Converter;

		private:
			std::string name{"unnamed pack"};
			std::unordered_map<int, OBLESector> sectors;

		public:
			OBLEPack() = default;

			inline void setName(std::string mName) { name = std::move(mName); }

			inline const std::string& getName() const noexcept			{ return name; }
			inline const decltype(sectors)& getSectors() const noexcept	{ return sectors; }
			inline decltype(sectors)& getSectors() noexcept				{ return sectors; }
			inline OBLESector& getSector(int mIdx) noexcept				{ return sectors[mIdx]; }
			inline bool isValid(int mIdx) noexcept						{ return sectors.count(mIdx) > 0; }
	};
}

#endif
