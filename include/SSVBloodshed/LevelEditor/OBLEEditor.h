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
#include "SSVBloodshed/LevelEditor/OBLEDatabase.h"

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
			OBLEGInput<OBLEEditor> input{*this};
			OBLEGDebugText<OBLEEditor> debugText{*this};
			OBLEDatabase database;
			OBLELevel level;
			std::vector<OBLETile*> currentTiles;
			int brushIdx{0}, brushSize{1}, currentX{0}, currentY{0}, currentZ{0}, currentRot{0}, currentId{0}, currentParamIdx{0};
			OBGame* game{nullptr};
			ssvs::BitmapText paramsText{*assets.obStroked};
			std::pair<OBLETType, std::map<std::string, ssvuj::Obj>> copiedParams{OBLETType::LETFloor, {}};

		public:
			inline OBLEEditor(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets), database{assets}
			{
				gameCamera.pan(-5, -5);
				gameState.onUpdate += [this](float mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };
				paramsText.setTracking(-3);
				paramsText.setPosition(35, 240 - 15);
				newGame();
			}

			inline void newGame()
			{
				level = {320 / 10, 240 / 10 - 2, database.get(OBLETType::LETFloor)};
				refresh();
			}

			inline void refresh() { for(auto& t : level.getTiles()) if(t.second.hasParam("id")) t.second.setId(assets, t.second.getParam<int>("id")); }

			inline void updateXY()
			{
				const auto& tileVec((gameCamera.getMousePosition() + Vec2f(5, 5)) / 10.f);
				currentX = tileVec.x; currentY = tileVec.y;
			}
			inline void grabTiles()
			{
				currentTiles.clear();

				for(int iY{0}; iY < brushSize; ++iY)
					for(int iX{0}; iX < brushSize; ++iX)
						if(level.isValid(currentX + iX, currentY + iY, currentZ))
							currentTiles.push_back(&level.getTile(currentX + iX, currentY + iY, currentZ));
			}

			inline void paint()	{ for(auto& t : currentTiles) { t->initFromEntry(getCurrentEntry()); t->setRot(currentRot); t->setId(assets, currentId); } }
			inline void del()	{ for(auto& t : currentTiles) { level.del(*t); } }
			inline void pick()	{ for(auto& t : currentTiles) { brushIdx = int(t->getType()); return; } }

			inline void copyParams()	{ for(auto& t : currentTiles) { copiedParams = std::make_pair(t->getType(), t->getParams()); return; } }
			inline void pasteParams()	{ for(auto& t : currentTiles) { if(t->getType() == copiedParams.first) t->setParams(copiedParams.second); } }

			inline void cycleRot(int mDeg)			{ currentRot = ssvu::wrapDegrees(currentRot + mDeg); }
			inline void cycleId(int mDir)			{ currentId += mDir; }
			inline void cycleParam(int mDir)		{ currentParamIdx += mDir; }
			inline void cycleCurrentParam(int mDir)
			{
				for(auto& t : currentTiles)
				{
					int idx{0};
					for(auto& p : t->getParams())
					{
						if(ssvu::getSIMod(currentParamIdx, (int)t->getParams().size()) == idx) p.second = ssvuj::as<int>(p.second) + mDir;
						++idx;
					}

					return;
				}
			}
			inline void cycleBrush(int mDir)		{ brushIdx = ssvu::getSIMod(brushIdx + mDir, database.getSize()); }
			inline void cycleZ(int mDir)			{ currentZ = -ssvu::getSIMod(-currentZ + mDir, 3); }
			inline void cycleBrushSize(int mDir)	{ brushSize = ssvu::getClamped(brushSize + mDir, 1, 20); }

			inline void updateParamsText()
			{
				for(auto& t : currentTiles)
				{
					std::string str;

					int idx{0};
					for(const auto& p : t->getParams())
					{
						bool cp{ssvu::getSIMod(currentParamIdx, (int)t->getParams().size()) == idx};

						if(cp) str += " >";
						str += p.first + "(" + ssvu::getReplacedAll(ssvu::toStr(p.second), "\n", "") + ")";
						if(cp) str += "< ";
						++idx;
					}

					paramsText.setString(str);
					break;
				}
			}

			inline void update(float mFT)
			{
				level.update();

				updateXY(); grabTiles();
				updateParamsText();
				if(input.painting) paint();
				if(input.deleting) del();

				debugText.update(mFT);
				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				{
					level.draw(gameWindow, true, currentZ);

					sf::RectangleShape hr({10.f * brushSize, 10.f * brushSize});
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
					sf::Sprite s{*getCurrentEntry().texture, getCurrentEntry().intRect};
					Vec2f origin{s.getTextureRect().width / 2.f, s.getTextureRect().height / 2.f};
					s.setOrigin(origin); s.setPosition(5 + origin.x, 240 - 15 + origin.x);
					if(getCurrentEntry().defaultParams.count("rot") > 0) s.setRotation(currentRot);
					render(s); render(paramsText);
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
			inline const decltype(input)& getInput() const noexcept	{ return input; }
			inline const OBLEDatabaseEntry& getCurrentEntry() const	{ return database.get(OBLETType(brushIdx)); }
	};
}

#endif
