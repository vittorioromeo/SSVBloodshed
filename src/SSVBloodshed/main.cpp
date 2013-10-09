#ifndef HGJSIOPHSJH

#include "SSVBloodshed/CESystem/CES.h"
#include "SSVBloodshed/OBCommon.h"

using namespace ssvces;

struct CPosition : Component		{ float x, y; CPosition(float mX, float mY) : x{mX}, y{mY} { } };
struct CVelocity : Component		{ float x, y; CVelocity(float mX, float mY) : x{mX}, y{mY} { } };
struct CAcceleration : Component	{ float x, y; CAcceleration(float mX, float mY) : x{mX}, y{mY} { } };
struct CLife : Component			{ float life; CLife(float mLife) : life{mLife} { } };
struct CSprite : Component			{ sf::RectangleShape sprite; CSprite() : sprite{ssvs::Vec2f(1, 1)} { } };

struct SMovement : System<CPosition, CVelocity, CAcceleration>
{
	inline void update(float mFT)
	{
		SYSTEM_LOOP_NOENTITY(cPosition, cVelocity, cAcceleration)
		{
			cVelocity.x += cAcceleration.x * mFT;
			cVelocity.y += cAcceleration.y * mFT;
			cPosition.x += cVelocity.x * mFT;
			cPosition.y += cVelocity.y * mFT;
		}}
	}
};

struct SDeath : System<CLife>
{
	inline void update(float mFT)
	{
		SYSTEM_LOOP(entity, cLife)
		{
			cLife.life -= mFT;
			if(cLife.life < 0) entity.destroy();
		}}
	}
};

struct SDraw : System<CPosition, CSprite>
{
	sf::RenderTarget& renderTarget;
	inline SDraw(sf::RenderTarget& mRenderTarget) : renderTarget(mRenderTarget) { }

	inline void draw()
	{
		SYSTEM_LOOP_NOENTITY(cPosition, cSprite)
		{
			cSprite.sprite.setPosition(cPosition.x, cPosition.y);
			renderTarget.draw(cSprite.sprite);
		}}
	}
};

int main()
{
	using namespace std;
	using namespace ssvu;

	ssvs::GameWindow gameWindow;
	gameWindow.setTitle("component tests");
	gameWindow.setTimer<ssvs::StaticTimer>(0.5f, 0.5f);
	gameWindow.setSize(1024, 768);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);

	Manager manager;
	SMovement sMovement;
	SDeath sDeath;
	SDraw sDraw{gameWindow};

	manager.registerSystem(sMovement);
	manager.registerSystem(sDeath);
	manager.registerSystem(sDraw);

	for(int i = 0; i < 20000; ++i)
	{
		auto e = manager.createEntity();
		e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
		e.createComponent<CVelocity>(ssvu::getRnd(-10, 10), ssvu::getRnd(-10, 10));
		e.createComponent<CAcceleration>(ssvu::getRnd(-1, 1), ssvu::getRnd(-1, 1));
		e.createComponent<CSprite>();
		e.createComponent<CLife>(ssvu::getRnd(50, 60));
	}

	float counter{0};
	ssvs::GameState gameState;
	gameState.onUpdate += [&, counter](float mFT) mutable
	{
		counter += mFT;
		if(counter > 100.f)
		{
			counter = 0;
			for(int i = 0; i < 20000; ++i)
			{
				auto e = manager.createEntity();
				e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
				e.createComponent<CVelocity>(ssvu::getRnd(-10, 10), ssvu::getRnd(-10, 10));
				e.createComponent<CAcceleration>(ssvu::getRnd(-1, 1), ssvu::getRnd(-1, 1));
				e.createComponent<CSprite>();
				e.createComponent<CLife>(ssvu::getRnd(50, 100));
			}
		}
		manager.refresh();
		sMovement.update(mFT);
		sDeath.update(mFT);

		if(gameWindow.getFPS() < 60) ssvu::lo<<gameWindow.getFPS()<<std::endl;
	};
	gameState.onDraw += [&]{ sDraw.draw(); };

	gameWindow.setGameState(gameState);
	gameWindow.run();

	return 0;
}

#else

// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/OBConfig.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Weapons/OBWpnTypes.h"
#include "SSVBloodshed/LevelEditor/OBLEEditor.h"

using namespace ob;
using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvms;

int main()
{
	unsigned int width{VideoMode::getDesktopMode().width}, height{VideoMode::getDesktopMode().height};
	width = 640; height = 480;

	OBAssets assets;

	GameWindow gameWindow;
	gameWindow.setTitle("operation bloodshed");
	gameWindow.setTimer<StaticTimer>(0.5f, 0.5f);
	gameWindow.setSize(width, height);
	gameWindow.setFullscreen(false);
	gameWindow.setFPSLimited(true);
	gameWindow.setMaxFPS(200);

	OBGame game{gameWindow, assets};
	OBLEEditor editor{gameWindow, assets};

	game.setEditor(editor);
	editor.setGame(game);

	gameWindow.setGameState(editor.getGameState());
	gameWindow.run();

	return 0;
}

#endif
