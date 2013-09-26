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
#include "SSVBloodshed/OBBarCounter.h"

namespace ob
{
	class OBGame
	{
		template<typename> friend class OBGInput;

		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::Camera gameCamera{gameWindow, 2.f}, overlayCamera{gameWindow, 2.f};
			OBFactory factory{assets, *this, manager};
			ssvs::GameState gameState;
			ssvsc::World world{ssvsc::createResolver<ssvsc::Impulse>(), ssvsc::createSpatial<ssvsc::HashGrid>(1000, 1000, 1000, 500)};
			sses::Manager manager;

			OBGInput<OBGame> input{*this};
			OBGParticles particles;

			OBGDebugText<OBGame> debugText{*this};
			sf::Sprite hudSprite{assets.get<sf::Texture>("tempHud.png")};

			ssvs::BitmapText testAmmoTxt{assets.get<ssvs::BitmapFont>("fontObStroked")};

		public:
			OBBarCounter testhp{2, 6, 13};

			inline OBGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets)
			{
				gameState.onUpdate += [this](float mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				// Testing hud
				hudSprite.setPosition(0, 240 - getGlobalHeight(hudSprite));

				testhp.setColor(sf::Color{184, 37, 53, 255}); testhp.setTracking(1);
				testhp.setPosition(13, (240 - getGlobalHeight(hudSprite) / 2.f) - 1.f);

				testAmmoTxt.setColor(sf::Color{136, 199, 234, 255}); testAmmoTxt.setTracking(-3);
				testAmmoTxt.setPosition(86, (240 - getGlobalHeight(hudSprite) / 2.f) - 3.f);

				newGame();
			}

			inline void newGame()
			{
				// Cleanup managers
				manager.clear(); world.clear(); particles.clear(factory);

				// Debug test level
				auto getTilePos = [](int mX, int mY) -> Vec2i { return toCoords(Vec2i{mX * 10 + 5, mY * 10 + 5}); };
				constexpr int maxX{320 / 10}, maxY{240 / 10 - 2};

				std::string level = "################################"
									"#..............................#"
									"#..P...#.......................#"
									"#......#....###................#"
									"#..#####...#.#.#....##.........#"
									"#..........#####....W#.....#...#"
									"#..........#.#.#....#E.....##..#"
									"#...........###.....W#....#.#..#"
									"#...gggggg..........##....##...#"
									"#...goooog.................#...#"
									"#...goooog.....................#"
									"##..goooog.....gggggggggg......#"
									"#...goooog.....goooooooog......#"
									"#..#goooog..##.goooooooog......#"
									"##..gggggg..##.goooooooog......#"
									"#..............goooooooog......#"
									"#...........##.gggggggggg......#"
									"#....##......#.................#"
									"#....##...................#....#"
									"###..##..................#.#...#"
									"###.............#N#.......#....#"
									"################################";


				auto tileIs = [&](int mX, int mY, char mChar){ if(mX < 0 || mY < 0 || mX >= maxX || mY >= maxY) return false; return level[ssvu::get1DIndexFrom2D(mX, mY, maxX)] == mChar; };


				unsigned int idx{0};
				for(int iY{0}; iY < maxY; ++iY)
					for(int iX{0}; iX < maxX; ++iX)
					{
						const auto& tp(getTilePos(iX, iY));

						switch(level[idx++])
						{
							case '#':
							{
								int mask{0};
								mask += tileIs(iX, iY - 1, '#') << 0;
								mask += tileIs(iX + 1, iY, '#') << 1;
								mask += tileIs(iX, iY + 1, '#') << 2;
								mask += tileIs(iX - 1, iY, '#') << 3;
								factory.createWall(tp, *assets.wallBitMask[mask]);
								break;
							}
							case '.': factory.createFloor(tp); break;
							case 'o': factory.createPit(tp); break;
							case 'g': factory.createFloor(tp, true); break;
							case 'P': factory.createFloor(tp); factory.createPlayer(tp); break;
							case 'N': factory.createFloor(tp); factory.createETurret(tp, Direction8::N); break;
							case 'S': factory.createFloor(tp); factory.createETurret(tp, Direction8::S); break;
							case 'W': factory.createFloor(tp); factory.createETurret(tp, Direction8::W); break;
							case 'E': factory.createFloor(tp); factory.createETurret(tp, Direction8::E); break;
						}
					}
			}

			inline void update(float mFT)
			{
				manager.update(mFT);
				world.update(mFT);
				debugText.update(mFT);
				gameCamera.update<int>(mFT);

				testAmmoTxt.setString(ssvu::toStr(testhp.getValue()));
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				manager.draw();
				gameCamera.unapply();

				overlayCamera.apply<int>();
				render(hudSprite);
				render(testhp);
				render(testAmmoTxt);
				overlayCamera.unapply();

				debugText.draw();

			}

			inline void render(const sf::Drawable& mDrawable) { gameWindow.draw(mDrawable); }

			inline Vec2i getMousePosition() const					{ return toCoords(gameCamera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow() noexcept		{ return gameWindow; }
			inline OBAssets& getAssets() noexcept					{ return assets; }
			inline OBFactory& getFactory() noexcept					{ return factory; }
			inline ssvs::GameState& getGameState() noexcept			{ return gameState; }
			inline ssvsc::World& getWorld() noexcept				{ return world; }
			inline sses::Manager& getManager() noexcept				{ return manager; }
			inline const decltype(input)& getInput() const noexcept	{ return input; }

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
			inline void createPElectric(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPElectric(particles.getPSTemp(), mPos); }
			inline void createPCharge(unsigned int mCount, const Vec2f& mPos, float mDist)		{ for(auto i(0u); i < mCount; ++i) ob::createPCharge(particles.getPSTemp(), mPos, mDist); }
	};
}

#endif
