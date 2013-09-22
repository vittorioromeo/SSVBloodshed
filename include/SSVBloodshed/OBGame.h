// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME
#define SSVOB_GAME

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/OBConfig.h"
#include "SSVBloodshed/OBFactory.h"

namespace ob
{
	class OBGame
	{
		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::Camera camera{gameWindow, 2.f};
			OBFactory factory;
			ssvs::GameState gameState;
			ssvsc::World world;
			sses::Manager manager;
			ssvs::BitmapText debugText;
			//ssvu::TimelineManager timelineManager;

			bool inputShoot{false}, inputSwitch{false}, inputBomb{false};
			int inputX{0}, inputY{0};

			sf::RenderTexture psPermTexture, psTempTexture;
			ParticleSystem* psPerm{nullptr};
			ParticleSystem* psTemp{nullptr};

			sf::Sprite hudSprite{assets.get<sf::Texture>("tempHud.png")};

		public:
			OBGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets);

			void newGame();

			void update(float mFrameTime);
			void updateDebugText(float mFrameTime);
			void draw();

			inline void render(const sf::Drawable& mDrawable) { gameWindow.draw(mDrawable); }

			inline Vec2i getMousePosition() const		{ return toCoords(camera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow()	{ return gameWindow; }
			inline OBAssets& getAssets()				{ return assets; }
			inline OBFactory& getFactory()				{ return factory; }
			inline ssvs::GameState& getGameState()		{ return gameState; }
			inline ssvsc::World& getWorld()				{ return world; }
			inline sses::Manager& getManager()			{ return manager; }

			inline bool getIShoot() const noexcept		{ return inputShoot; }
			inline bool getISwitch() const noexcept		{ return inputSwitch; }
			inline bool getIBomb() const noexcept		{ return inputBomb; }
			inline int getIX() const noexcept			{ return inputX; }
			inline int getIY() const noexcept			{ return inputY; }

			void createPBlood(unsigned int mCount, const Vec2f& mPosition, float mMult = 1.f);
			void createPGib(unsigned int mCount, const Vec2f& mPosition);
			void createPDebris(unsigned int mCount, const Vec2f& mPosition);
			void createPDebrisFloor(unsigned int mCount, const Vec2f& mPosition);
			void createPMuzzle(unsigned int mCount, const Vec2f& mPosition);
			void createPPlasma(unsigned int mCount, const Vec2f& mPosition);
			void createPSmoke(unsigned int mCount, const Vec2f& mPosition);
	};

	// TODO: join walls, hud mockup, color fixes, etc etc etc
}

#endif
