#ifndef HGJSIOPHSJH

#include <chrono>
#include "SSVBloodshed/CESystem/CES.h"
#include "SSVBloodshed/OBCommon.h"

using namespace ssvces;

struct CPosition : Component		{ float x, y; CPosition(float mX, float mY) : x{mX}, y{mY} { } };
struct CVelocity : Component		{ float x, y; CVelocity(float mX, float mY) : x{mX}, y{mY} { } };
struct CAcceleration : Component	{ float x, y; CAcceleration(float mX, float mY) : x{mX}, y{mY} { } };
struct CLife : Component			{ float life; CLife(float mLife) : life{mLife} { } };
struct CSprite : Component			{ sf::RectangleShape sprite; CSprite() : sprite{ssvs::Vec2f(1, 1)} { } };
struct CColorInhibitor : Component	{ float life; CColorInhibitor(float mLife) : life{mLife} { } };

struct SMovement : System<Req<CPosition, CVelocity, CAcceleration>>
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

struct SDeath : System<Req<CLife>>
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

struct SNonColorInhibitor : System<Req<CSprite>, Not<CColorInhibitor>>
{
	inline void draw()
	{
		SYSTEM_LOOP(e, cSprite)
		{
			cSprite.sprite.setFillColor(sf::Color::Red);
		}}
	}
};

struct SDraw : System<Req<CPosition, CSprite>>
{
	sf::RenderTarget& renderTarget;
	inline SDraw(sf::RenderTarget& mRenderTarget) : renderTarget(mRenderTarget) { }

	inline void draw()
	{
		SYSTEM_LOOP_NOENTITY(cPosition, cSprite)
		{
			cSprite.sprite.setPosition(cPosition.x, cPosition.y);
			//renderTarget.draw(cSprite.sprite);
		}}
	}
};

struct SColorInhibitor : System<Req<CSprite, CColorInhibitor>>
{
	inline void update(float mFT)
	{
		SYSTEM_LOOP(entity, cSprite, cColorInhibitor)
		{
			cColorInhibitor.life -= mFT;
			if(cColorInhibitor.life < 0) entity.removeComponent<CColorInhibitor>();
		}}
	}

	inline void draw()
	{
		SYSTEM_LOOP(e, cSprite)
		{
			cSprite.sprite.setFillColor(sf::Color::Blue);
		}}
	}
};

int main()
{
	using namespace std;
	using namespace ssvu;
	using namespace sf;

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
	SColorInhibitor sColorInhibitor;
	SNonColorInhibitor sNonColorInhibitor;

	manager.registerSystem(sMovement);
	manager.registerSystem(sDeath);
	manager.registerSystem(sNonColorInhibitor);
	manager.registerSystem(sDraw);
	manager.registerSystem(sColorInhibitor);

	/*ssvu::startBenchmark();
	{
		for(int k = 0; k < 5; ++k)
		{
			for(int i = 0; i < 20000; ++i)
			{
				auto e = manager.createEntity();
				e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
				e.createComponent<CVelocity>(ssvu::getRndR(-1.f, 2.f), ssvu::getRndR(-1.f, 2.f));
				e.createComponent<CAcceleration>(ssvu::getRndR(-1.f, 2.f), ssvu::getRndR(-1.f, 2.f));
				e.createComponent<CLife>(1);
			}

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);
		}

		for(int k = 0; k < 5; ++k)
		{
			for(int i = 0; i < 20000; ++i)
			{
				auto e = manager.createEntity();
				e.createComponent<CPosition>(ssvu::getRnd(512 - 100, 512 + 100), ssvu::getRnd(384 - 100, 384 + 100));
				e.createComponent<CVelocity>(ssvu::getRndR(-1.f, 2.f), ssvu::getRndR(-1.f, 2.f));
				e.createComponent<CAcceleration>(ssvu::getRndR(-1.f, 2.f), ssvu::getRndR(-1.f, 2.f));
				e.createComponent<CLife>(25);
			}

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);

			manager.refresh();
			sMovement.update(5);
			sDeath.update(5);
		}
	} ssvu::lo("benchmark") << ssvu::endBenchmark();*/

	string dms = "";

	float counter{99};
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
				e.createComponent<CVelocity>(ssvu::getRndR(-1.f, 2.f), ssvu::getRndR(-1.f, 2.f));
				e.createComponent<CAcceleration>(ssvu::getRndR(-0.5f, 1.5f), ssvu::getRndR(-0.5f, 1.5f));
				e.createComponent<CSprite>();
				e.createComponent<CLife>(ssvu::getRnd(50, 100));
				e.createComponent<CColorInhibitor>(ssvu::getRnd(5, 85));
				e.addGroups(0);
			}
		}

		ssvu::startBenchmark();
		{
			manager.refresh();
			sMovement.update(mFT);
			sDeath.update(mFT);
			sColorInhibitor.update(mFT);
		}
		auto ums = ssvu::endBenchmark();

		gameWindow.setTitle("up: " + ums + "\t dw: " + dms);

		//if(gameWindow.getFPS() < 60) ssvu::lo<<gameWindow.getFPS()<<std::endl;
		//ssvu::lo<<manager.getEntityCount(0)<<std::endl;
	};
	gameState.onDraw += [&]
	{
		ssvu::startBenchmark();
		{
			sNonColorInhibitor.draw();
			sDraw.draw();
			sColorInhibitor.draw();
		}
		dms = ssvu::endBenchmark();
	};


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
