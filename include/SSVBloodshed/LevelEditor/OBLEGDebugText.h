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
			OBLEGDebugText(TGame& mGame) : editor(mGame), debugText{*editor.getAssets().obStroked}
			{
				debugText.setTracking(-3);
			}

			inline void update(float mFT)
			{
				std::ostringstream s;
				const auto& entities(editor.getManager().getEntities());
				std::size_t componentCount{0};
				for(const auto& e : entities) componentCount += e->getComponents().size();

				s	<< "Z: "				<< editor.currentZ << "\t"
					<< "Has tile: "			<< std::boolalpha << (editor.currentTile != nullptr) << "\n"
					<< "FPS: "				<< static_cast<int>(editor.getGameWindow().getFPS()) << "\t"
					<< "FT: "				<< mFT << "\n"
					<< "Entities: "			<< entities.size() << "\n"
					<< "Components: "		<< componentCount << std::endl;

				debugText.setString(s.str());
			}
			inline void draw() const { editor.render(debugText); }
	};
}

#endif
