// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME
#define SSVOB_LEVELEDITOR_GAME

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/LevelEditor/OBLEGDebugText.h"
#include "SSVBloodshed/LevelEditor/OBLEGInput.h"
#include "SSVBloodshed/LevelEditor/OBLETile.h"
#include "SSVBloodshed/LevelEditor/OBLELevel.h"

#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBLEEditor
	{
		template<typename> friend class OBLEGInput;
		template<typename> friend class OBLEGDebugText;

		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::Camera gameCamera{gameWindow, 2.f}, overlayCamera{gameWindow, 2.f};
			ssvs::GameState gameState;
			sses::Manager manager;
			OBLEGInput<OBLEEditor> input{*this};
			OBLEGDebugText<OBLEEditor> debugText{*this};

			std::map<OBLETType, OBLETileDataDrawable> tileMap
			{
				{LETFloor,		{LETFloor,		assets.txSmall,		assets.floor									}},
				{LETWall,		{LETWall,		assets.txSmall,		assets.wallSingle								}},
				{LETGrate,		{LETGrate,		assets.txSmall,		assets.floorGrate								}},
				{LETPit,		{LETPit,		assets.txSmall,		assets.pit										}},
				{LETTurret,		{LETTurret,		assets.txSmall,		assets.eTurret,				{{"rot", 0}}		}},
				{LETSpawner,	{LETSpawner,	assets.txSmall,		assets.pjCannonPlasma,		{{"spawns", {}}}	}},
				{LETPlayer,		{LETPlayer,		assets.txSmall,		assets.p1Stand									}}
			};

			OBLELevel level{320 / 10, 240 / 10 - 2, tileMap[OBLETType::LETFloor]};
			OBLETile* currentTile{nullptr};
			int currentBrushIdx{0}, currentX{0}, currentY{0}, currentZ{0};

			OBGame* game{nullptr};

		public:
			inline OBLEEditor(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets)
			{
				gameCamera.pan(-5, -5);

				gameState.onUpdate += [this](float mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				newGame();
			}

			inline void newGame()
			{
				manager.clear();
				level = {320 / 10, 240 / 10 - 2, tileMap[OBLETType::LETFloor]};
			}

			inline const OBLETileDataDrawable& getCurrentData() { return tileMap[static_cast<OBLETType>(currentBrushIdx)]; }

			inline bool isValid() { return currentTile != nullptr; }

			inline void updateXY()
			{
				const auto& tileVec((gameCamera.getMousePosition() + Vec2f(5, 5)) / 10.f);
				currentX = tileVec.x; currentY = tileVec.y;
			}
			inline void grabTile()
			{
				if(level.isValid(currentX, currentY, currentZ)) currentTile = &level.getTile(currentX, currentY, currentZ);
				else currentTile = nullptr;
			}

			inline void paint()				{ if(!isValid()) return; currentTile->initFromDataDrawable(getCurrentData()); }
			inline void del()				{ if(!isValid()) return; level.del(currentX, currentY, currentZ); }
			inline void rotate(int mDeg)	{ if(!isValid()) return; currentTile->rotate(mDeg); }

			inline void cycleBrush(int mDir)	{ currentBrushIdx = ssvu::getSIMod(currentBrushIdx + mDir, (int)tileMap.size()); }
			inline void cycleZ(int mDir)		{ currentZ = ssvu::getSIMod(currentZ + 2 + mDir, 5) - 2; }

			inline void update(float mFT)
			{
				level.update();

				updateXY(); grabTile();
				if(input.painting) paint();
				if(input.deleting) del();

				manager.update(mFT);
				debugText.update(mFT);
				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				{
					manager.draw();
					level.draw(gameWindow, true, currentZ);

					sf::RectangleShape hr({10.f, 10.f});
					hr.setFillColor({255, 0, 0, 125});
					hr.setOutlineColor({255, 255, 0, 125});
					hr.setOutlineThickness(0.5f);
					hr.setPosition(currentX * 10.f, currentY * 10.f);
					hr.setOrigin(5.f, 5.f);
					render(hr);
				}
				gameCamera.unapply();

				overlayCamera.apply<int>();
				{
					sf::Sprite s{*getCurrentData().texture, getCurrentData().intRect}; s.setPosition(5, 240 - 15); render(s);
				}
				overlayCamera.unapply();

				debugText.draw();
			}

			inline void render(const sf::Drawable& mDrawable)									{ gameWindow.draw(mDrawable); }
			inline void render(const sf::Drawable& mDrawable, sf::RenderStates mRenderStates)	{ gameWindow.draw(mDrawable, mRenderStates); }

			inline void setGame(OBGame& mGame)						{ game = &mGame; }

			inline ssvs::GameWindow& getGameWindow() noexcept		{ return gameWindow; }
			inline OBAssets& getAssets() noexcept					{ return assets; }
			inline ssvs::GameState& getGameState() noexcept			{ return gameState; }
			inline sses::Manager& getManager() noexcept				{ return manager; }
			inline const decltype(input)& getInput() const noexcept	{ return input; }
	};
}

#endif
