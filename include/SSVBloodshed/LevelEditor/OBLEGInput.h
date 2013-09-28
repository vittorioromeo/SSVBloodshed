// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME_INPUT
#define SSVOB_LEVELEDITOR_GAME_INPUT

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"

namespace ob
{
	template<typename TGame> class OBLEGInput
	{
		private:
			TGame& game;

		public:
			bool painting{false};

			inline OBLEGInput(TGame& mGame) : game(mGame)
			{
				using k = sf::Keyboard::Key;
				using b = sf::Mouse::Button;
				using t = ssvs::Input::Trigger::Type;

				auto& gs(game.getGameState());

				ssvs::add2StateInput(gs, {{b::Left}}, painting);

				gs.addInput({{k::Escape}}, [this](float){ game.assets.musicPlayer.stop(); std::terminate(); });

				gs.addInput({{k::A}}, [=](float){ game.gameCamera.pan(-4, 0); });
				gs.addInput({{k::D}}, [=](float){ game.gameCamera.pan(4, 0); });
				gs.addInput({{k::W}}, [=](float){ game.gameCamera.pan(0, -4); });
				gs.addInput({{k::S}}, [=](float){ game.gameCamera.pan(0, 4); });
				gs.addInput({{k::Q}}, [=](float){ game.gameCamera.zoomOut(1.1f); });
				gs.addInput({{k::E}}, [=](float){ game.gameCamera.zoomIn(1.1f); });

				gs.addInput({{k::R}}, [this](float){ game.newGame(); }, t::Once);

				gs.addInput({{k::Z}}, [this](float){ game.cycle(-1); }, t::Once);
				gs.addInput({{k::X}}, [this](float){ game.cycle(1); }, t::Once);

				gs.addInput({{k::C}}, [this](float){ game.level.saveToFile("./level.txt"); }, t::Once);
				gs.addInput({{k::V}}, [this](float){ game.level.loadFromFile("./level.txt", game.tileMap); }, t::Once);

				gs.addInput({{k::B}}, [this](float){ game.rotate(-45); }, t::Once);
				gs.addInput({{k::N}}, [this](float){ game.rotate(45); }, t::Once);
			}
	};
}

#endif
