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
			TGame& editor;

		public:
			bool painting{false}, deleting{false};

			inline OBLEGInput(TGame& mGame) : editor(mGame)
			{
				using k = sf::Keyboard::Key;
				using b = sf::Mouse::Button;
				using t = ssvs::Input::Trigger::Type;

				auto& gs(editor.getGameState());

				ssvs::add2StateInput(gs, {{b::Left}}, painting);
				ssvs::add2StateInput(gs, {{b::Right}}, deleting);

				gs.addInput({{k::Escape}}, [this](float){ editor.assets.musicPlayer.stop(); std::terminate(); });

				gs.addInput({{k::A}}, [=](float){ editor.gameCamera.pan(-4, 0); });
				gs.addInput({{k::D}}, [=](float){ editor.gameCamera.pan(4, 0); });
				gs.addInput({{k::W}}, [=](float){ editor.gameCamera.pan(0, -4); });
				gs.addInput({{k::S}}, [=](float){ editor.gameCamera.pan(0, 4); });
				gs.addInput({{k::Q}}, [=](float){ editor.gameCamera.zoomOut(1.1f); });
				gs.addInput({{k::E}}, [=](float){ editor.gameCamera.zoomIn(1.1f); });

				gs.addInput({{k::R}}, [this](float){ editor.newGame(); }, t::Once);

				gs.addInput({{k::Z}}, [this](float){ editor.cycleBrush(-1); }, t::Once);
				gs.addInput({{k::X}}, [this](float){ editor.cycleBrush(1); }, t::Once);

				gs.addInput({{k::C}}, [this](float){ editor.level.saveToFile("./level.txt"); }, t::Once);
				gs.addInput({{k::V}}, [this](float){ editor.level.loadFromFile("./level.txt", &editor.tileMap); }, t::Once);

				gs.addInput({{k::B}}, [this](float){ editor.rotate(-45); }, t::Once);
				gs.addInput({{k::N}}, [this](float){ editor.rotate(45); }, t::Once);

				gs.addInput({{k::I}}, [this](float){ editor.cycleZ(-1); }, t::Once);
				gs.addInput({{k::K}}, [this](float){ editor.cycleZ(1); }, t::Once);

				gs.onEvent(sf::Event::EventType::MouseWheelMoved) += [this](const sf::Event& mEvent)
				{
					if(mEvent.mouseWheel.y < 440) editor.cycleZ(ssvu::getSign(mEvent.mouseWheel.delta));
					else editor.cycleBrush(ssvu::getSign(mEvent.mouseWheel.delta));
				};

				gs.addInput({{k::F1}}, [this](float){ editor.getGameWindow().setGameState(editor.game->getGameState()); }, t::Once);
			}
	};
}

#endif
