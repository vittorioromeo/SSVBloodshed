// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/OBConfig.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Weapons/OBWpnTypes.hpp"
#include "SSVBloodshed/LevelEditor/OBLEEditor.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"

using namespace ob;
using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvms;

int main()
{
	SSVUT_RUN();

	OBConfig::setSoundEnabled(false);

	unsigned int width{VideoMode::getDesktopMode().width}, height{VideoMode::getDesktopMode().height};
	width = 640; height = 480;


	GameWindow gameWindow;
	gameWindow.setTitle("operation bloodshed");
	gameWindow.setTimer<TimerStatic>(0.5f, 0.5f);
	gameWindow.setSize(width, height);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);
	gameWindow.setPixelMult(1);


	auto assets(mkUPtr<OBAssets>());
	auto game(mkUPtr<OBGame>(gameWindow, *assets));
	auto editor(mkUPtr<OBLEEditor>(gameWindow, *assets));
	auto database(mkUPtr<OBLEDatabase>(*assets));

	game->setEditor(*editor);
	game->setDatabase(*database);
	editor->setGame(*game);
	editor->setDatabase(*database);

	editor->newPack();

	gameWindow.setGameState(editor->getGameState());
	gameWindow.run();

	return 0;
}

// TODO: generic macro recursion dispatcher
// static tokenizer macro generator with tokeninterface ctrp base class
// rooms with scrolling
// TODO: aspect ratio resizing tests
// TODO: make explosions pierce destructible walls
