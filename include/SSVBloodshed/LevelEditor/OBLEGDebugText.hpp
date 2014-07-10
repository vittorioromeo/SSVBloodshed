// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME_DEBUGTEXT
#define SSVOB_LEVELEDITOR_GAME_DEBUGTEXT

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"

namespace ob
{
	template<typename TGame> class OBLEGDebugText
	{
		private:
			TGame& editor;
			std::string str;

		public:
			OBLEGDebugText(TGame& mGame) noexcept : editor(mGame) { }

			inline void update(FT)
			{
				std::string copiedParamsStr;
				for(const auto& p : editor.copiedParams.second) copiedParamsStr += p.first + "(" + ssvu::getReplacedAll(ssvu::toStr(p.second), "\n", "") + ")" + "\n";

				std::ostringstream s;
				s	<< "PACK NAME: " << editor.sharedData.getPack().getName() << "\n"
					<< "Sector IDX: " << editor.sharedData.getCurrentSectorIdx() << "\n"
					<< "Level XY: "	<< editor.sharedData.getCurrentLevelX() << ";" << editor.sharedData.getCurrentLevelY() << "\n"
					<< "Z: " << editor.currentZ << "\n"
					<< "C/V: save/load" << "\t" << "Z/X: cycle Z" << "\n"
					<< "A/S: cycle id" << "\t" << "Q/W: cycle param" << "\n"
					<< "LShift: pick" << "\n" << "N/M: copy/paste params" << "\n\n"
					<< copiedParamsStr << "\n";

				str = s.str();
			}
			inline const std::string& getStr() { return str; }
	};
}

#endif
