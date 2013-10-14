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

#include "SSVBloodshed/LevelEditor/OBLELevel.h"
#include "SSVBloodshed/LevelEditor/OBLEEditor.h"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.h"

namespace ob
{
	class OBCVMachine;

	struct OBGLevelStat
	{
		bool clear{false};
	};

	class OBGame
	{
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

			OBLEDatabase database{assets, this};
			OBLESector sector;
			std::unordered_map<OBLELevel*, OBGLevelStat> levelStats;
			OBLELevel* currentLevel{nullptr};
			int currentLevelX{0}, currentLevelY{0};

		public:
			ssvu::Delegate<void()> onPostUpdate;

			OBBarCounter testhp{2, 6, 13};
			ssvs::BitmapText txtShards{*assets.obStroked}, txtVM{*assets.obStroked};

			std::string lastMsUpdate, lastMsDraw;

			inline OBGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets)
			{
				gameState.onUpdate += [this](float mFT){ ssvu::startBenchmark(); update(mFT); lastMsUpdate = ssvu::endBenchmark(); };
				gameState.onDraw += [this]{ ssvu::startBenchmark(); draw(); lastMsDraw = ssvu::endBenchmark(); };

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

				reloadSector();
			}

			inline void reloadSector()	{ loadSector("./level.txt"); newGame(); }
			inline void newGame()		{ currentLevelX = currentLevelY = 0; loadLevel(); }

			inline void createBounds()
			{
				int width{toCoords(currentLevel->getWidth())}, height{toCoords(currentLevel->getHeight())}, offset{toCoords(100)};

				std::initializer_list<std::pair<Vec2i, Vec2i>> bounds
				{
					{{0 - offset, 0},	{0, height}},
					{{width, 0},		{width + offset, height}},
					{{0, 0 - offset},	{width, 0}},
					{{0, height},		{width, height + offset}}
				};

				for(const auto& p : bounds) world.create(ssvs::getCenter(p.first, p.second), ssvs::getSize(p.first, p.second), true).addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir, OBGroup::GLevelBound);
			}

			inline void loadSector(const ssvu::FileSystem::Path& mPath)
			{
				try { sector = ssvuj::as<OBLESector>(ssvuj::readFromFile(mPath)); }
				catch(...) { ssvu::lo("Fatal error") << "Failed to load sector" << std::endl; }
			}
			inline void loadLevel()
			{
				auto getTilePos = [](int mX, int mY) -> Vec2i { return toCoords(Vec2i{mX * 10 + 5, mY * 10 + 5}); };
				manager.clear(); world.clear(); particles.clear(factory);

				try
				{
					currentLevel = &sector.getLevel(currentLevelX, currentLevelY);
					for(auto& p : currentLevel->getTiles()) database.spawn(*currentLevel, p.second, getTilePos(p.second.getX(), p.second.getY()));
				}
				catch(...) { ssvu::lo("Fatal error") << "Failed to load level" << std::endl; }

				createBounds();
			}

			template<typename TPlayer> inline bool changeLevel(const TPlayer& mPlayer, int mDirX, int mDirY)
			{
				auto playerData(mPlayer.getData());
				int nextLevelX{currentLevelX + mDirX}, nextLevelY{currentLevelY + mDirY}, offset{toCoords(10)};

				if(mDirX == 1) playerData.pos.x = toCoords(0) + offset;
				else if(mDirX == -1) playerData.pos.x = toCoords(currentLevel->getWidth()) - offset;

				if(mDirY == 1) playerData.pos.y = toCoords(0) + offset;
				else if(mDirY == -1) playerData.pos.y = toCoords(currentLevel->getHeight()) - offset;

				if(!sector.isValid(nextLevelX, nextLevelY)) return false;

				onPostUpdate += [this, nextLevelX, nextLevelY, playerData]
				{
					if(currentLevelX != nextLevelX || currentLevelY != nextLevelY)
					{
						currentLevelX = nextLevelX;
						currentLevelY = nextLevelY;
						loadLevel();

						// Remove existing players (TODO: change)
						for(auto& e : manager.getEntities(OBGroup::GPlayer)) e->destroy();

						// If the level was cleared, remove all enemies (TODO: change not spawn)
						if(levelStats[currentLevel].clear) for(auto& e : manager.getEntities(OBGroup::GEnemy)) e->destroy();

						factory.createPlayer(playerData.pos).template getComponent<TPlayer>().initFromData(playerData);
					}
				};

				return true;
			}

			inline bool isLevelClear() noexcept		{ return manager.getEntityCount(OBGroup::GEnemy) <= 0; }
			inline void updateLevelStat() noexcept	{ if(isLevelClear()) levelStats[currentLevel].clear = true; }

			inline void update(float mFT)
			{
				manager.update(mFT);
				world.update(mFT);
				debugText.update(mFT);
				gameCamera.update<int>(mFT);

				updateLevelStat();

				onPostUpdate();
				onPostUpdate.clear();

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
				render(txtShards);
				render(txtVM);
				overlayCamera.unapply();

				debugText.draw();
			}

			inline void render(const sf::Drawable& mDrawable)									{ gameWindow.draw(mDrawable); }
			inline void render(const sf::Drawable& mDrawable, sf::RenderStates mRenderStates)	{ gameWindow.draw(mDrawable, mRenderStates); }

			inline void setEditor(OBLEEditor& mEditor) { editor = &mEditor; }

			inline Vec2i getMousePosition() const						{ return toCoords(gameCamera.getMousePosition()); }
			inline ssvs::GameWindow& getGameWindow() noexcept			{ return gameWindow; }
			inline OBAssets& getAssets() noexcept						{ return assets; }
			inline OBFactory& getFactory() noexcept						{ return factory; }
			inline ssvs::GameState& getGameState() noexcept				{ return gameState; }
			inline World& getWorld() noexcept							{ return world; }
			inline sses::Manager& getManager() noexcept					{ return manager; }
			inline const decltype(input)& getInput() const noexcept		{ return input; }
			inline const OBLESector& getSector() const noexcept			{ return sector; }
			inline const OBLELevel* getCurrentLevel() const noexcept	{ return currentLevel; }
			inline int getCurrentLevelX() const noexcept				{ return currentLevelX; }
			inline int getCurrentLevelY() const noexcept				{ return currentLevelY; }

			inline void createPBlood(unsigned int mCount, const Vec2f& mPos, float mMult = 1.f)
			{
				for(auto i(0u); i < mCount; ++i) ob::createPBlood(particles.getPSPerm(), mPos, mMult);
				for(auto i(0u); i < mCount / 2; ++i) ob::createPGib(particles.getPSTemp(), mPos);
			}
			inline void createPGib(unsigned int mCount, const Vec2f& mPos)						{ for(auto i(0u); i < mCount; ++i) ob::createPGib(particles.getPSTemp(), mPos); }
			inline void createPDebris(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPDebris(particles.getPSTemp(), mPos); }
			inline void createPDebrisFloor(unsigned int mCount, const Vec2f& mPos)				{ for(auto i(0u); i < mCount; ++i) ob::createPDebrisFloor(particles.getPSTemp(), mPos); }
			inline void createPMuzzle(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPMuzzle(particles.getPSTempAdd(), mPos); }
			inline void createPPlasma(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPPlasma(particles.getPSTempAdd(), mPos); }
			inline void createPSmoke(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPSmoke(particles.getPSTemp(), mPos); }
			inline void createPElectric(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPElectric(particles.getPSTempAdd(), mPos); }
			inline void createPCharge(unsigned int mCount, const Vec2f& mPos, float mDist)		{ for(auto i(0u); i < mCount; ++i) ob::createPCharge(particles.getPSTempAdd(), mPos, mDist); }
			inline void createPShard(unsigned int mCount, const Vec2f& mPos)					{ for(auto i(0u); i < mCount; ++i) ob::createPShard(particles.getPSTempAdd(), mPos); }
			inline void createPExplosion(unsigned int mCount, const Vec2f& mPos)				{ for(auto i(0u); i < mCount; ++i) ob::createPExplosion(particles.getPSTempAdd(), mPos); }

			inline void createEShard(unsigned int mCount, const Vec2i& mPos)					{ for(auto i(0u); i < mCount; ++i) factory.createShard(mPos); }
	};
}

// TODO: bullet sensor pressure plates, SSVSC refactoring/optimization
// TODO: decouple weapon sprite from enemy sprite, explosives, enemy orientation, organic group?, do not pierce breakable wall etc
// TODO: tripwires, laserwires, powerups, classes, weapon sets, etc
// TODO: consider changing body.onResolution lambda with a bool
// TODO: editor pick tile in center
// TODO: bullet knockback? replicators? spawners?
// TODO: major group/facotry refactoring!
// TODO: explosive crates id, fuses
// TODO: big enforcer variant that shoots a plasma cannon ball that splits in other plasma cannon balls
// TODO: vending machines hud becomes text

#endif
