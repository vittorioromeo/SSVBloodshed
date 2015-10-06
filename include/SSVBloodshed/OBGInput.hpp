// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME_INPUT
#define SSVOB_GAME_INPUT

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBConfig.hpp"

namespace ob
{
    template <typename TGame>
    class OBGInput
    {
    private:
        TGame& game;
        bool iShoot{false}, iSwitch{false}, iBomb{false};
        int iX{0}, iY{0};

    public:
        inline OBGInput(TGame& mGame) : game(mGame)
        {
            using k = ssvs::KKey;
            // using b = ssvs::MBtn;
            using t = ssvs::Input::Type;

            auto& gs(game.getGameState());

            ssvs::add2StateInput(gs, OBConfig::getTShoot(), iShoot);
            ssvs::add2StateInput(gs, OBConfig::getTSwitch(), iSwitch, t::Once);
            ssvs::add2StateInput(gs, OBConfig::getTBomb(), iBomb, t::Once);
            ssvs::add3StateInput(
                gs, OBConfig::getTLeft(), OBConfig::getTRight(), iX);
            ssvs::add3StateInput(
                gs, OBConfig::getTUp(), OBConfig::getTDown(), iY);

            gs.addInput({{k::Escape}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.assets.musicPlayer.stop();
                    game.getGameWindow().stop();
                });

            gs.addInput({{k::A}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.gameCamera.pan(-4, 0);
                });
            gs.addInput({{k::D}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.gameCamera.pan(4, 0);
                });
            gs.addInput({{k::W}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.gameCamera.pan(0, -4);
                });
            gs.addInput({{k::S}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.gameCamera.pan(0, 4);
                });
            gs.addInput({{k::Q}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.gameCamera.zoomOut(1.1f);
                });
            gs.addInput({{k::E}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.gameCamera.zoomIn(1.1f);
                });

            gs.addInput({{k::R}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.newGame();
                },
                t::Once);
            gs.addInput({{k::Pause}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.paused = !game.paused;
                },
                t::Once);

            gs.addInput({{k::Num0}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createPit(game.getMousePosition());
                },
                t::Once);
            gs.addInput({{k::Num1}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createWall(
                        game.getMousePosition(), game.getAssets().wallSingle);
                },
                t::Once);
            gs.addInput({{k::Num2}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createERunner(
                        game.getMousePosition(), RunnerType::Unarmed);
                });
            gs.addInput({{k::Num3}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createECharger(
                        game.getMousePosition(), ChargerType::Unarmed);
                },
                t::Once);
            gs.addInput({{k::Num4}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createEJuggernaut(
                        game.getMousePosition(), JuggernautType::Unarmed);
                },
                t::Once);
            gs.addInput({{k::Num5}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createEGiant(game.getMousePosition());
                },
                t::Once);
            gs.addInput({{k::Num6}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createEBall(
                        game.getMousePosition(), BallType::Normal, false);
                },
                t::Once);
            gs.addInput({{k::Num7}}, [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createERunner(
                        game.getMousePosition(), RunnerType::PlasmaBolter);
                });
            gs.addInput({{k::Num8}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createECharger(
                        game.getMousePosition(), ChargerType::GrenadeLauncher);
                },
                t::Once);
            gs.addInput({{k::Num9}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createEJuggernaut(game.getMousePosition(),
                        JuggernautType::RocketLauncher);
                },
                t::Once);
            gs.addInput({{k::P}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createEBall(
                        game.getMousePosition(), BallType::Flying, false);
                },
                t::Once);
            gs.addInput({{k::O}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.factory.createEEnforcer(game.getMousePosition());
                },
                t::Once);

            gs.addInput({{k::F1}},
                [this](FT)
                {
                    if(isUsingGUI()) return;
                    game.getGameWindow().setGameState(
                        game.editor->getGameState());
                },
                t::Once);
        }

        inline bool isUsingGUI() const noexcept
        {
            return game.guiCtx.isInUse();
        }

        inline bool getIShoot() const noexcept { return iShoot; }
        inline bool getISwitch() const noexcept { return iSwitch; }
        inline bool getIBomb() const noexcept { return iBomb; }
        inline int getIX() const noexcept { return iX; }
        inline int getIY() const noexcept { return iY; }
    };
}

#endif
