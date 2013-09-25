// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME_INPUT
#define SSVOB_GAME_INPUT

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	template<typename TGame> class OBGInput
	{
		private:
			TGame& game;
			bool iShoot{false}, iSwitch{false}, iBomb{false};
			int iX{0}, iY{0};

		public:
			inline OBGInput(TGame& mGame) : game(mGame)
			{
				using k = sf::Keyboard::Key;
				using b = sf::Mouse::Button;
				using t = ssvs::Input::Trigger::Type;

				auto& gs(game.getGameState());

				ssvs::add2StateInput(gs, {{k::Z}}, iShoot);
				ssvs::add2StateInput(gs, {{k::X}}, iSwitch, t::Once);
				ssvs::add2StateInput(gs, {{k::Space}}, iBomb, t::Once);
				ssvs::add3StateInput(gs, {{k::Left}}, {{k::Right}}, iX);
				ssvs::add3StateInput(gs, {{k::Up}}, {{k::Down}}, iY);

				gs.addInput({{k::Escape}}, [this](float){ game.assets.musicPlayer.stop(); std::terminate(); });

				gs.addInput({{k::A}}, [=](float){ game.camera.pan(-4, 0); });
				gs.addInput({{k::D}}, [=](float){ game.camera.pan(4, 0); });
				gs.addInput({{k::W}}, [=](float){ game.camera.pan(0, -4); });
				gs.addInput({{k::S}}, [=](float){ game.camera.pan(0, 4); });
				gs.addInput({{k::Q}}, [=](float){ game.camera.zoomOut(1.1f); });
				gs.addInput({{k::E}}, [=](float){ game.camera.zoomIn(1.1f); });

				gs.addInput({{k::R}}, [this](float){ game.newGame(); }, t::Once);

				gs.addInput({{k::Num0}}, [this](float){ game.factory.createPit(game.getMousePosition()); }, t::Once);
				gs.addInput({{k::Num1}}, [this](float){ game.factory.createWall(game.getMousePosition()); }, t::Once);
				gs.addInput({{k::Num2}}, [this](float){ game.factory.createERunner(game.getMousePosition(), false); });
				gs.addInput({{k::Num3}}, [this](float){ game.factory.createECharger(game.getMousePosition(), false); }, t::Once);
				gs.addInput({{k::Num4}}, [this](float){ game.factory.createEJuggernaut(game.getMousePosition(), false); }, t::Once);
				gs.addInput({{k::Num5}}, [this](float){ game.factory.createEGiant(game.getMousePosition()); }, t::Once);
				gs.addInput({{k::Num6}}, [this](float){ game.factory.createEBall(game.getMousePosition(), false); }, t::Once);
				gs.addInput({{k::Num7}}, [this](float){ game.factory.createERunner(game.getMousePosition(), true); });
				gs.addInput({{k::Num8}}, [this](float){ game.factory.createECharger(game.getMousePosition(), true); }, t::Once);
				gs.addInput({{k::Num9}}, [this](float){ game.factory.createEJuggernaut(game.getMousePosition(), true); }, t::Once);
				gs.addInput({{k::P}}, [this](float){ game.factory.createEBall(game.getMousePosition(), true); }, t::Once);
			}

			inline bool getIShoot() const noexcept		{ return iShoot; }
			inline bool getISwitch() const noexcept		{ return iSwitch; }
			inline bool getIBomb() const noexcept		{ return iBomb; }
			inline int getIX() const noexcept			{ return iX; }
			inline int getIY() const noexcept			{ return iY; }
	};
}

#endif
