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
			bool painting{false}, deleting{false}, modCtrl{false}, modShift{false};

			inline OBLEGInput(TGame& mGame) : editor(mGame)
			{
				using k = sf::Keyboard::Key;
				using b = sf::Mouse::Button;
				using t = ssvs::Input::Trigger::Type;
				auto& gs(editor.getGameState());

				ssvs::add2StateInput(gs, {{b::Left}}, painting);
				ssvs::add2StateInput(gs, {{b::Right}}, deleting);
				ssvs::add2StateInput(gs, {{k::LControl}}, modCtrl);
				ssvs::add2StateInput(gs, {{k::LShift}}, modShift);

				gs.addInput({{k::Escape}},	[this](float){ editor.assets.musicPlayer.stop(); std::terminate(); });
				gs.addInput({{k::R}},		[this](float){ editor.clearCurrentLevel(); }, t::Once);
				gs.addInput({{k::C}},		[this](float){ editor.saveToFile("./level.txt"); }, t::Once);
				gs.addInput({{k::V}},		[this](float){ editor.loadFromFile("./level.txt"); }, t::Once);
				gs.addInput({{k::Z}},		[this](float){ editor.cycleZ(-1); }, t::Once);
				gs.addInput({{k::X}},		[this](float){ editor.cycleZ(1); }, t::Once);
				gs.addInput({{k::A}},		[this](float){ editor.cycleId(-1); }, t::Once);
				gs.addInput({{k::S}},		[this](float){ editor.cycleId(1); }, t::Once);
				gs.addInput({{k::Q}},		[this](float){ editor.cycleParam(-1); }, t::Once);
				gs.addInput({{k::W}},		[this](float){ editor.cycleParam(1); }, t::Once);
				gs.addInput({{k::N}},		[this](float){ editor.copyParams(); });
				gs.addInput({{k::M}},		[this](float){ editor.pasteParams(); });
				gs.addInput({{k::LShift}},	[this](float){ editor.pick(); }, t::Once);
				gs.addInput({{k::F1}},		[this](float){ editor.getGameWindow().setGameState(editor.game->getGameState()); }, t::Once);

				gs.addInput({{k::Numpad4}},	[this](float){ editor.cycleLevel(-1, 0); }, t::Once);
				gs.addInput({{k::Numpad6}},	[this](float){ editor.cycleLevel(1, 0); }, t::Once);
				gs.addInput({{k::Numpad8}},	[this](float){ editor.cycleLevel(0, -1); }, t::Once);
				gs.addInput({{k::Numpad2}},	[this](float){ editor.cycleLevel(0, 1); }, t::Once);

				gs.onEvent(sf::Event::EventType::MouseWheelMoved) += [this](const sf::Event& mEvent)
				{
					int dir{ssvu::getSign(mEvent.mouseWheel.delta)};

					if(modShift && modCtrl)	editor.cycleCurrentParam(dir);
					else if(modShift && !modCtrl) editor.cycleBrush(dir);
					else if(modCtrl && !modShift) editor.cycleBrushSize(dir);
					else editor.cycleRot(dir * 45);
				};

			}
	};
}

#endif
