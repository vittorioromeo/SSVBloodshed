// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME
#define SSVOB_GAME

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/OBConfig.h"
#include "SSVBloodshed/OBFactory.h"
#include "SSVBloodshed/OBGDebugText.h"
#include "SSVBloodshed/OBGParticles.h"
#include "SSVBloodshed/OBGInput.h"
#include "SSVBloodshed/Particles/OBParticleTypes.h"

namespace ob
{
	class OBGame
	{
		template<typename> friend class OBGInput;

		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::Camera camera{gameWindow, 2.f};
			OBFactory factory{assets, *this, manager};
			ssvs::GameState gameState;
			ssvsc::World world{ssvsc::Utils::createResolver<ssvsc::Impulse>(), ssvsc::Utils::createSpatial<ssvsc::HashGrid>(1000, 1000, 1000, 500)};
			sses::Manager manager;

			OBGInput<OBGame> input{*this};
			OBGParticles particles;
			OBGDebugText<OBGame> debugText{*this};

			sf::Sprite hudSprite{assets.get<sf::Texture>("tempHud.png")};

		public:
			inline OBGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets)
			{
				gameState.onUpdate += [this](float mFrameTime){ update(mFrameTime); };
				gameState.onDraw += [this]{ draw(); };

				// Temp hud sprite
				hudSprite.setScale(2.f, 2.f);
				hudSprite.setPosition(0, 480 - getGlobalHeight(hudSprite));

				newGame();
			}

			inline void newGame()
			{
				manager.clear();
				world.clear();
				particles.clear(factory);

				auto getTilePos = [](int mX, int mY) -> Vec2i { return toCoords(Vec2i{mX * 10 + 5, mY * 10 + 5}); };
				constexpr int maxX{320 / 10}, maxY{240 / 10 - 2};

				for(int iX{0}; iX < maxX; ++iX)
					for(int iY{0}; iY < maxY; ++iY)
					{
						if(iX == 0 || iX == maxX - 1 || iY == 0 || iY == maxY - 1) factory.createWall(getTilePos(iX, iY));
						else factory.createFloor(getTilePos(iX, iY));
					}

				factory.createPlayer(getTilePos(5, 5));
			}

			inline void update(float mFrameTime)
			{
				manager.update(mFrameTime);
				world.update(mFrameTime);
				debugText.update(mFrameTime);
				camera.update<int>(mFrameTime);
			}
			inline void draw()
			{
				camera.apply<int>();
				manager.draw();
				camera.unapply();
				render(hudSprite);
				debugText.draw();
			}

			inline void render(const sf::Drawable& mDrawable) { gameWindow.draw(mDrawable); }

			inline Vec2i getMousePosition() const				{ return toCoords(camera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow() noexcept	{ return gameWindow; }
			inline OBAssets& getAssets() noexcept				{ return assets; }
			inline OBFactory& getFactory() noexcept				{ return factory; }
			inline ssvs::GameState& getGameState() noexcept		{ return gameState; }
			inline ssvsc::World& getWorld() noexcept			{ return world; }
			inline sses::Manager& getManager() noexcept			{ return manager; }

			inline const decltype(input)& getInput() const noexcept { return input; }

			inline void createPBlood(unsigned int mCount, const Vec2f& mPos, float mMult = 1.f)
			{
				for(auto i(0u); i < mCount; ++i) ob::createPBlood(particles.getPSPerm(), mPos, mMult);
				for(auto i(0u); i < mCount / 2; ++i) ob::createPGib(particles.getPSTemp(), mPos);
			}
			inline void createPGib(unsigned int mCount, const Vec2f& mPos)						{ for(auto i(0u); i < mCount; ++i) ob::createPGib(particles.getPSTemp(), mPos); }
			inline void createPDebris(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPDebris(particles.getPSTemp(), mPos); }
			inline void createPDebrisFloor(unsigned int mCount, const Vec2f& mPos)				{ for(auto i(0u); i < mCount; ++i) ob::createPDebrisFloor(particles.getPSTemp(), mPos); }
			inline void createPMuzzle(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPMuzzle(particles.getPSTemp(), mPos); }
			inline void createPPlasma(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPPlasma(particles.getPSTemp(), mPos); }
			inline void createPSmoke(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPSmoke(particles.getPSTemp(), mPos); }
	};

	// TODO: join walls, hud mockup, color fixes, etc etc etc
}

#endif
