// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME
#define SSVOB_GAME

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/OBConfig.hpp"
#include "SSVBloodshed/OBFactory.hpp"
#include "SSVBloodshed/OBGDebugText.hpp"
#include "SSVBloodshed/OBGParticles.hpp"
#include "SSVBloodshed/OBGInput.hpp"
#include "SSVBloodshed/Particles/OBParticleTypes.hpp"
#include "SSVBloodshed/OBBarCounter.hpp"
#include "SSVBloodshed/OBSharedData.hpp"

#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"
#include "SSVBloodshed/LevelEditor/OBLEEditor.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"

#include "SSVBloodshed/GUI/GUI.hpp"
#include "SSVBloodshed/GUIOB/FormIO.hpp"

namespace ob
{
	class OBCVMachine;

	struct OBGLevelStat
	{
		bool clear{false};
	};

	class OBGame
	{
		template<typename> friend class OBGDebugText;
		template<typename> friend class OBGInput;

		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::GameState gameState;
			ssvs::Camera gameCamera{gameWindow, 2.f}, overlayCamera{gameWindow, 2.f};
			OBFactory factory{assets, *this, manager};
			World world{1000, 1000, 1000, 500};
			sses::Manager manager;

			OBGInput<OBGame> input{*this};
			OBGParticles particles;

			OBGDebugText<OBGame> debugText{*this};
			sf::Sprite hudSprite{assets.get<sf::Texture>("tempHud.png")};

			ssvs::BitmapText testAmmoTxt{*assets.obStroked};

			OBLEEditor* editor{nullptr};
			OBSharedData sharedData;
			std::unordered_map<OBLELevel*, OBGLevelStat> levelStats;

			bool paused{true};
			sf::RectangleShape pauseRect{Vec2f(gameWindow.getWidth(), gameWindow.getHeight())};
			ssvs::BitmapText pauseTxt{*assets.obBigStroked, "PAUSED"};
			GUI::Context guiCtx{assets, gameWindow};
			FormIO* formIO{nullptr};

			template<typename T, typename... TArgs> inline void createParticles(const T& mFunc, OBParticleSystem& mPS, unsigned int mCount, const Vec2f& mPos, TArgs&&... mArgs)
			{
				unsigned int total(mCount * OBConfig::getParticleMult());
				for(auto i(0u); i < total; ++i) mFunc(mPS, mPos, std::forward<TArgs>(mArgs)...);
			}

		public:
			ssvu::Delegate<void()> onPostUpdate;

			OBBarCounter testhp{2, 6, 13};
			ssvs::BitmapText txtShards{*assets.obStroked}, txtVM{*assets.obStroked}, txtInfo{*assets.obStroked};

			inline OBGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets)
			{
				gameState.onUpdate += [this](FT mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				// Testing hud
				hudSprite.setPosition(0, 240 - ssvs::getGlobalHeight(hudSprite));

				testhp.setColor(sf::Color{184, 37, 53, 255}); testhp.setTracking(1);
				testhp.setPosition(13, (240 - ssvs::getGlobalHeight(hudSprite) / 2.f) - 1.f);

				testAmmoTxt.setColor(sf::Color{136, 199, 234, 255}); testAmmoTxt.setTracking(-3);
				testAmmoTxt.setPosition(86, (240 - ssvs::getGlobalHeight(hudSprite) / 2.f) - 3.f);

				txtShards.setColor(sf::Color{195, 90, 10, 255}); txtShards.setTracking(-3);
				txtShards.setPosition(235, (240 - ssvs::getGlobalHeight(hudSprite) / 2.f) - 3.f);

				txtVM.setColor(sf::Color{225, 225, 225, 255}); txtVM.setTracking(-3);
				txtVM.setPosition(120, (240 - ssvs::getGlobalHeight(hudSprite) / 2.f) - 3.f);

				txtInfo.setColor(sf::Color{225, 225, 225, 255}); txtInfo.setTracking(-3);
				txtInfo.setPosition(270, (240 - ssvs::getGlobalHeight(hudSprite) / 2.f) - 3.f);
				txtInfo.setString("Sector 1");

				pauseRect.setFillColor(sf::Color{0, 0, 0, 150});
				pauseRect.setOrigin(0.f, 0.f);
				pauseRect.setPosition(0.f, 0.f);

				pauseTxt.setPosition(overlayCamera.getCenter());
				pauseTxt.setOrigin(ssvs::getGlobalHalfSize(pauseTxt));

				gameState.onAnyEvent += [this](const sf::Event& mEvent){ guiCtx.onAnyEvent(mEvent); };

				formIO = &guiCtx.create<FormIO>();
				formIO->onLoad += [this](const std::string& mFilename)
				{
					ssvufs::Path path{mFilename};
					if(!path.exists()) return;

					sharedData.loadPack(path);
					formIO->getLblCurrentPath().setString("CURRENT: " + sharedData.getCurrentPath());
					newGame(); manager.update(0);
				};
			}

			inline void newGame()
			{
				sharedData.setCurrentSector(0);
				sharedData.setCurrentLevel(0, 0);
				loadCurrentLevel();
			}

			inline void createBounds()
			{
				int width{levelWidthCoords}, height{levelHeightCoords}, offset{toCoords(100)};

				std::initializer_list<std::pair<Vec2i, Vec2i>> bounds
				{
					{{0 - offset, 0},	{0, height}},
					{{width, 0},		{width + offset, height}},
					{{0, 0 - offset},	{width, 0}},
					{{0, height},		{width, height + offset}}
				};

				for(const auto& p : bounds) world.create(ssvs::getCenter(p.first, p.second), ssvs::getSize(p.first, p.second), true).addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GLevelBound);
			}

			inline void loadCurrentLevel()
			{
				auto getTilePos = [](int mX, int mY){ return toCoords(Vec2i{mX * 10 + 5, mY * 10 + 5}); };
				manager.clear(); world.clear(); particles.clear(factory);

				try
				{
					for(auto& p : sharedData.getCurrentTiles()) sharedData.getDatabase().spawn(sharedData.getCurrentLevel(), p.second, getTilePos(p.second.getX(), p.second.getY()));
				}
				catch(...) { ssvu::lo("Fatal error") << "Failed to load level" << std::endl; }

				createBounds();
			}

			template<typename TPlayer> inline bool changeLevel(const TPlayer& mPlayer, int mDirX, int mDirY)
			{
				auto playerData(mPlayer.getData());
				int nextLevelX{sharedData.getCurrentLevelX() + mDirX}, nextLevelY{sharedData.getCurrentLevelY() + mDirY}, offset{toCoords(10)};

				if(mDirX == 1) playerData.pos.x = toCoords(0) + offset;
				else if(mDirX == -1) playerData.pos.x = levelWidthCoords - offset;

				if(mDirY == 1) playerData.pos.y = toCoords(0) + offset;
				else if(mDirY == -1) playerData.pos.y = levelHeightCoords - offset;

				if(!sharedData.isLevelValid(nextLevelX, nextLevelY)) return false;

				onPostUpdate += [this, nextLevelX, nextLevelY, playerData]
				{
					if(sharedData.getCurrentLevelX() != nextLevelX || sharedData.getCurrentLevelY() != nextLevelY)
					{
						sharedData.setCurrentLevel(nextLevelX, nextLevelY);
						loadCurrentLevel();

						// Remove existing players (TODO: change)
						for(auto& e : manager.getEntities(OBGroup::GPlayer)) e->destroy();

						// If the level was cleared, remove all enemies (TODO: change not spawn)
						if(levelStats[&sharedData.getCurrentLevel()].clear) for(auto& e : manager.getEntities(OBGroup::GEnemy)) e->destroy();

						factory.createPlayer(playerData.pos).template getComponent<TPlayer>().initFromData(playerData);
					}
				};

				return true;
			}

			inline bool isLevelClear() noexcept		{ return manager.getEntityCount(OBGroup::GEnemy) <= 0; }
			inline void updateLevelStat() noexcept	{ if(isLevelClear()) levelStats[&sharedData.getCurrentLevel()].clear = true; }

			inline void update(FT mFT)
			{
				if(!paused && !sharedData.isCurrentLevelNull())
				{
					manager.update(mFT);
					world.update(mFT);
				}
				else
				{
					guiCtx.update(mFT);
				}

				debugText.update(mFT);
				gameCamera.update<int>(mFT);

				if(!paused && !sharedData.isCurrentLevelNull())
				{
					updateLevelStat();
					onPostUpdate();
					onPostUpdate.clear();
				}

				testAmmoTxt.setString(ssvu::toStr(testhp.getValue()));
			}
			inline void draw()
			{
				//TODO: canc in textbox
				gameCamera.apply<int>();
				manager.draw();
				gameCamera.unapply();

				overlayCamera.apply<int>();
				{
					if(paused)
					{
						render(pauseRect);
						render(pauseTxt);
					}

					render(hudSprite); render(testhp); render(testAmmoTxt); render(txtShards); render(txtVM); render(txtInfo);
				}
				overlayCamera.unapply();

				if(paused) guiCtx.draw();

				debugText.draw();
			}

			template<typename... TArgs> inline void render(const sf::Drawable& mDrawable, TArgs&&... mArgs)	{ gameWindow.draw(mDrawable, std::forward<TArgs>(mArgs)...); }

			inline void setEditor(OBLEEditor& mEditor) noexcept { editor = &mEditor; }
			inline void setDatabase(OBLEDatabase& mDatabase) noexcept { sharedData.setDatabase(mDatabase, this); }

			inline Vec2i getMousePosition() const						{ return toCoords(gameCamera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow() noexcept			{ return gameWindow; }
			inline OBAssets& getAssets() noexcept						{ return assets; }
			inline OBFactory& getFactory() noexcept						{ return factory; }
			inline ssvs::GameState& getGameState() noexcept				{ return gameState; }
			inline World& getWorld() noexcept							{ return world; }
			inline sses::Manager& getManager() noexcept					{ return manager; }
			inline const decltype(input)& getInput() const noexcept		{ return input; }

			inline void createPBlood(unsigned int mCount, const Vec2f& mPos, float mMult = 1.f)
			{
				createParticles(ob::createPBlood, particles.getPSPerm(), mCount, mPos, mMult);
				createParticles(ob::createPGib, particles.getPSTemp(), mCount / 2, mPos);
			}
			inline void createPExplosion(unsigned int mCount, const Vec2f& mPos)
			{
				createParticles(ob::createPExplosion, particles.getPSTempAdd(), mCount, mPos);
				createParticles(ob::createPExplosion, particles.getPSTemp(), mCount / 4 + 1, mPos);
			}
			inline void createPGib(unsigned int mCount, const Vec2f& mPos)						{ createParticles(ob::createPGib,			particles.getPSTemp(),		mCount, mPos); }
			inline void createPDebris(unsigned int mCount, const Vec2f& mPos)					{ createParticles(ob::createPDebris,		particles.getPSTemp(),		mCount, mPos); }
			inline void createPDebrisFloor(unsigned int mCount, const Vec2f& mPos)				{ createParticles(ob::createPDebrisFloor,	particles.getPSTemp(),		mCount, mPos); }
			inline void createPMuzzleBullet(unsigned int mCount, const Vec2f& mPos)				{ createParticles(ob::createPMuzzleBullet,	particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPMuzzlePlasma(unsigned int mCount, const Vec2f& mPos)				{ createParticles(ob::createPMuzzlePlasma,	particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPMuzzleRocket(unsigned int mCount, const Vec2f& mPos)				{ createParticles(ob::createPMuzzleRocket,	particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPPlasma(unsigned int mCount, const Vec2f& mPos)					{ createParticles(ob::createPPlasma,		particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPSmoke(unsigned int mCount, const Vec2f& mPos)					{ createParticles(ob::createPSmoke,			particles.getPSTemp(),		mCount, mPos); }
			inline void createPElectric(unsigned int mCount, const Vec2f& mPos)					{ createParticles(ob::createPElectric,		particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPCharge(unsigned int mCount, const Vec2f& mPos, float mDist)		{ createParticles(ob::createPCharge,		particles.getPSTempAdd(),	mCount, mPos, mDist); }
			inline void createPShard(unsigned int mCount, const Vec2f& mPos)					{ createParticles(ob::createPShard,			particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPHeal(unsigned int mCount, const Vec2f& mPos)						{ createParticles(ob::createPHeal,			particles.getPSTempAdd(),	mCount, mPos); }
			inline void createPCaseBullet(unsigned int mCount, const Vec2f& mPos, float mDeg)	{ createParticles(ob::createPCaseBullet,	particles.getPSTemp(),		mCount, mPos, mDeg); }
			inline void createPCaseRocket(unsigned int mCount, const Vec2f& mPos, float mDeg)	{ createParticles(ob::createPCaseRocket,	particles.getPSTemp(),		mCount, mPos, mDeg); }
			inline void createPForceField(unsigned int mCount, const Vec2f& mPos)				{ createParticles(ob::createPForceField,	particles.getPSTempAdd(),	mCount, mPos); }

			inline void createEShard(unsigned int mCount, const Vec2i& mPos) { for(auto i(0u); i < mCount; ++i) factory.createShard(mPos); }
	};
}

// TODO: add final where it makes sense
// ssvs::networking from testudpchat
// autoupdater "download_only_if_unexistant":[] ...
// bullet sensor pressure plates, SSVSC refactoring/optimization
// enemy orientation, organic group?, do not pierce breakable wall etc
// tripwires, laserwires, powerups, classes, weapon sets, etc
// bullet knockback? replicators? spawners?
// major group/facotry refactoring!, fuses, ammunition
// big enforcer variant that shoots a plasma cannon ball that splits in other plasma cannon balls
// switches that can be pressed with X
// multiple id actions (open 1, toggle 2...)
// refactor everything, check code quality
// customize turret rates in editor! (and projectile speed mult)
// add small red gun asset and implement red bouncing laser shots
// lock room until clear? (remove green doors?)
// cloning machines! (controlling multiple players can be really fun)
// pack/saving loading, pack options
// TODO: physics-driven gibs

#endif
