// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME_INPUT
#define SSVOB_LEVELEDITOR_GAME_INPUT

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"

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
				using k = ssvs::KKey;
				using b = ssvs::MBtn;
				using t = ssvs::Input::Type;
				using m = ssvs::Input::Mode;
				auto& gs(editor.getGameState());

				ssvs::add2StateInput(gs, {{b::Left}}, painting);
				ssvs::add2StateInput(gs, {{b::Right}}, deleting, t::Always, m::Exclusive);
				ssvs::add2StateInput(gs, {{k::LControl}}, modCtrl);
				ssvs::add2StateInput(gs, {{k::LShift}}, modShift);

				gs.addInput({{k::Escape}},	[this](FT){ if(editor.guiCtx.isInUse()) return; editor.assets.musicPlayer.stop(); editor.getGameWindow().stop(); });
				gs.addInput({{k::R}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.clearCurrentLevel(); }, t::Once);
				gs.addInput({{k::C}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.saveToFile("./level.txt"); }, t::Once, m::Exclusive);
				gs.addInput({{k::V}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.loadFromFile("./level.txt"); }, t::Once, m::Exclusive);
				gs.addInput({{k::Z}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleZ(-1); }, t::Once);
				gs.addInput({{k::X}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleZ(1); }, t::Once);
				gs.addInput({{k::A}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleId(-1); }, t::Once);
				gs.addInput({{k::S}},		[this](FT){ if(editor.guiCtx.isInUse()) return;  editor.cycleId(1); }, t::Once);
				gs.addInput({{k::Q}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleParam(-1); }, t::Once);
				gs.addInput({{k::W}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleParam(1); }, t::Once);
				gs.addInput({{k::T}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleCurrentParam(-100); }, t::Once);
				gs.addInput({{k::Y}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleCurrentParam(100); }, t::Once);

				gs.addInput({{b::Middle}},	[this](FT){ if(editor.guiCtx.isInUse()) return; editor.pick(); }, t::Once);
				gs.addInput({{k::F1}},		[this](FT){ if(editor.guiCtx.isInUse()) return; editor.getGameWindow().setGameState(editor.game->getGameState()); }, t::Once);

				gs.addInput({{{k::LShift}, {b::Right}}}, [this](FT){ if(editor.guiCtx.isInUse()) return; editor.openParams(); }, t::Once, m::Exclusive);

				gs.addInput({{k::LControl, k::C}}, [this](FT){ if(editor.guiCtx.isInUse()) return; editor.copyTiles(); }, t::Once, m::Exclusive);
				gs.addInput({{k::LControl, k::V}}, [this](FT){ if(editor.guiCtx.isInUse()) return; editor.pasteTiles(); }, t::Always, m::Exclusive);

				gs.addInput({{k::Numpad4}},	[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleLevel(-1, 0); }, t::Once);
				gs.addInput({{k::Numpad6}},	[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleLevel(1, 0); }, t::Once);
				gs.addInput({{k::Numpad8}},	[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleLevel(0, -1); }, t::Once);
				gs.addInput({{k::Numpad2}},	[this](FT){ if(editor.guiCtx.isInUse()) return; editor.cycleLevel(0, 1); }, t::Once);

				gs.onEvent(sf::Event::EventType::MouseWheelMoved) += [this](const sf::Event& mEvent)
				{
					int dir{ssvu::getSign(mEvent.mouseWheel.delta)};

					if(modShift && modCtrl)	editor.cycleCurrentParam(dir);
					else if(modShift && !modCtrl) editor.cycleRot(dir * 45);
					else if(modCtrl && !modShift) editor.cycleBrushSize(dir);
					else editor.cycleBrush(dir);
				};

			}
	};
}

#endif
