// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME_DEBUGTEXT
#define SSVOB_GAME_DEBUGTEXT

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"

namespace ob
{
	template<typename TGame> class OBGDebugText
	{
		private:
			TGame& game;
			ssvs::BitmapText debugText;

		public:
			inline OBGDebugText(TGame& mGame) : game(mGame), debugText{*game.getAssets().obStroked} { debugText.setTracking(-3); }

			inline void update(FT)
			{
				std::ostringstream s;
				const auto& entities(game.getManager().getEntities());
				const auto& bodies(game.getWorld().getBodies());
				const auto& sensors(game.getWorld().getSensors());
				std::size_t componentCount{0}, dynamicBodiesCount{0};
				for(const auto& e : entities) componentCount += e->getComponents().size();
				for(const auto& b : bodies) if(!b->isStatic()) ++dynamicBodiesCount;

				s	<< "FPS: "				<< static_cast<int>(game.getGameWindow().getFPS()) << "\n"
					<< "U: "				<< game.gameWindow.getMsUpdate() << "\t" << "D: " << game.gameWindow.getMsDraw() << "\n"
					<< "Bodies(all): "		<< bodies.size() << "\n"
					<< "Bodies(static): "	<< bodies.size() - dynamicBodiesCount << "\n"
					<< "Bodies(dynamic): "	<< dynamicBodiesCount << "\n"
					<< "Sensors: "			<< sensors.size() << "\n"
					<< "Entities: "			<< entities.size() << "\n"
					<< "Components: "		<< componentCount << std::endl;

				debugText.setString(s.str());
			}
			inline void draw() const { game.render(debugText); }
	};
}

#endif
