// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME_DEBUGTEXT
#define SSVOB_LEVELEDITOR_GAME_DEBUGTEXT

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"

namespace ob
{
	template<typename TGame> class OBLEGDebugText
	{
		private:
			TGame& editor;
			ssvs::BitmapText debugText;

		public:
			OBLEGDebugText(TGame& mGame) noexcept : editor(mGame), debugText{*editor.getAssets().obStroked} { debugText.setTracking(-3); }

			inline void update(float)
			{
				std::string copiedParamsStr;
				for(const auto& p : editor.copiedParams.second) copiedParamsStr += p.first + "(" + ssvu::getReplacedAll(ssvu::toStr(p.second), "\n", "") + ")" + "\n";

				std::ostringstream s;
				s	<< "Z: " << editor.currentZ << "\n"
					<< "Level XY: "	<< editor.currentLevelX << ";" << editor.currentLevelY << "\n"
					<< "C/V: save/load" << "\t" << "Z/X: cycle Z" << "\n"
					<< "A/S: cycle id" << "\t" << "Q/W: cycle param" << "\n"
					<< "LShift: pick" << "\n" << "N/M: copy/paste params" << "\n\n"
					<< copiedParamsStr << std::endl;

				debugText.setString(s.str());
			}
			inline void draw() const { editor.render(debugText); }
	};
}

#endif
