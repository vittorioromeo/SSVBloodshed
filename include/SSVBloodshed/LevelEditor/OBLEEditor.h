// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME
#define SSVOB_LEVELEDITOR_GAME

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/LevelEditor/OBLEGDebugText.h"
#include "SSVBloodshed/LevelEditor/OBLEGInput.h"
#include "SSVBloodshed/LevelEditor/OBLETile.h"
#include "SSVBloodshed/LevelEditor/OBLELevel.h"
#include "SSVBloodshed/LevelEditor/OBLESector.h"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.h"
#include "SSVBloodshed/LevelEditor/OBLEJson.h"

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

			OBLESector sector;
			OBLELevel* currentLevel{nullptr};
			int currentLevelX{0}, currentLevelY{0};

			std::vector<OBLETile*> currentTiles;
			int brushIdx{0}, brushSize{1}, currentBrushLeft{0}, currentBrushTop{0}, currentBrushX{0}, currentBrushY{0}, currentZ{0}, currentRot{0}, currentId{-1}, currentParamIdx{0};
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
				paramsText.setPosition(55, 240 - 15);

				newSector();
			}

			inline void newSector() { sector.clear(); sector.init(database); refreshCurrentLevel(); clearCurrentLevel(); }

			inline void refreshCurrentLevel()	{ currentLevel = &sector.getLevel(currentLevelX, currentLevelY); }
			inline void clearCurrentLevel()		{ *currentLevel = {levelRows, levelColumns, database.get(OBLETType::LETFloor)}; refreshCurrentTiles(); }
			inline void refreshCurrentTiles()	{ for(auto& t : currentLevel->getTiles()) if(t.second.hasParam("id")) t.second.setId(assets, t.second.getParam<int>("id")); }

			inline void updateXY()
			{
				const auto& fColumns(static_cast<float>(currentLevel->getColumns()));
				const auto& fRows(static_cast<float>(currentLevel->getRows()));
				const auto& tileVec((gameCamera.getMousePosition() + Vec2f(10, 10)) / 10.f);

				currentBrushLeft =	ssvu::getClamped(tileVec.x - brushSize / 2.f, 0.f, fColumns - brushSize);
				currentBrushTop =	ssvu::getClamped(tileVec.y - brushSize / 2.f, 0.f, fRows - brushSize);
				currentBrushX =		ssvu::getClamped(tileVec.x - 1 / 2.f, 0.f, fColumns - 1);
				currentBrushY =		ssvu::getClamped(tileVec.y - 1 / 2.f, 0.f, fRows - 1);
			}
			inline void grabTiles()
			{
				currentTiles.clear();

				for(int iY{0}; iY < brushSize; ++iY)
					for(int iX{0}; iX < brushSize; ++iX)
						if(currentLevel->isValid(currentBrushLeft + iX, currentBrushTop + iY, currentZ))
							currentTiles.push_back(&currentLevel->getTile(currentBrushLeft + iX, currentBrushTop + iY, currentZ));
			}

			inline OBLETile& getPickTile() const noexcept { return currentLevel->getTile(currentBrushX, currentBrushY, currentZ); }

			inline void paint()	{ for(auto& t : currentTiles) { t->initFromEntry(getCurrentEntry()); t->setRot(currentRot); t->setId(assets, currentId); } }
			inline void del()	{ for(auto& t : currentTiles) { currentLevel->del(*t); } }
			inline void pick()	{ auto& t(getPickTile()); brushIdx = int(t.getType()); }

			inline void copyParams()	{ auto& t(getPickTile()); copiedParams = std::make_pair(t.getType(), t.getParams()); }
			inline void pasteParams()	{ for(auto& t : currentTiles) { if(t->getType() == copiedParams.first) t->setParams(copiedParams.second); } }

			inline void cycleRot(int mDeg)			{ currentRot = ssvu::wrapDeg(currentRot + mDeg); }
			inline void cycleId(int mDir)			{ currentId += mDir; }
			inline void cycleParam(int mDir)		{ currentParamIdx += mDir; }
			inline void cycleCurrentParam(int mDir)
			{
				auto& t(getPickTile());

				int idx{0};
				for(auto& p : t.getParams())
				{
					if(ssvu::getSIMod(currentParamIdx, static_cast<int>(t.getParams().size())) == idx++)
					{
						if(ssvuj::is<int>(p.second)) p.second = ssvuj::as<int>(p.second) + mDir;
						else if(ssvuj::is<float>(p.second)) p.second = ssvuj::as<float>(p.second) + float(mDir);
					}
				}
			}
			inline void cycleBrush(int mDir)				{ brushIdx = ssvu::getSIMod(brushIdx + mDir, database.getSize()); }
			inline void cycleZ(int mDir)					{ currentZ = -ssvu::getSIMod(-currentZ + mDir, 3); }
			inline void cycleBrushSize(int mDir)			{ brushSize = ssvu::getClamped(brushSize + mDir, 1, 20); }
			inline void cycleLevel(int mDirX, int mDirY)	{ currentLevelX += mDirX; currentLevelY += mDirY; refreshCurrentLevel(); refreshCurrentTiles(); }

			inline void saveToFile(const ssvu::FileSystem::Path& mPath) { ssvuj::writeToFile(ssvuj::getArch(sector), mPath); }
			inline void loadFromFile(const ssvu::FileSystem::Path& mPath)
			{
				sector.clear();
				sector = ssvuj::as<OBLESector>(ssvuj::readFromFile(mPath));
				sector.init(database); refreshCurrentLevel(); refreshCurrentTiles();
			}

			inline void updateParamsText()
			{
				auto& t(getPickTile());
				std::string str;

				int idx{0};
				for(const auto& p : t.getParams())
				{
					bool cp{ssvu::getSIMod(currentParamIdx, static_cast<int>(t.getParams().size())) == idx++};
					if(cp) str += " >";
					str += p.first + "(" + ssvu::getReplacedAll(ssvu::toStr(p.second), "\n", "") + ")";
					if(cp) str += "< ";
				}

				paramsText.setString(str);
				paramsText.setScale(t.getParams().size() < 3 ? Vec2f(0.8f, 0.8f) : Vec2f(0.65f, 0.65f));
			}

			inline void update(float mFT)
			{
				currentLevel->update();
				updateXY(); grabTiles(); updateParamsText();
				if(input.painting) paint();
				if(input.deleting) del();

				debugText.update(mFT);
				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				{
					currentLevel->draw(gameWindow, true, currentZ);

					{
						sf::RectangleShape hr({10.f * brushSize, 10.f * brushSize});
						hr.setFillColor({255, 0, 0, 125});
						hr.setOutlineColor({255, 255, 0, 125});
						hr.setOutlineThickness(0.5f);
						hr.setPosition(currentBrushLeft * 10.f, currentBrushTop * 10.f);
						hr.setOrigin(5.f, 5.f);
						render(hr);
					}

					{
						sf::RectangleShape hr({10.f, 10.f});
						hr.setFillColor({0, 0, 0, 0});
						hr.setOutlineColor({255, 255, 0, 125});
						hr.setOutlineThickness(0.65f);
						hr.setPosition(currentBrushX * 10.f, currentBrushY * 10.f);
						hr.setOrigin(5.f, 5.f);
						render(hr);
					}
				}
				gameCamera.unapply();

				overlayCamera.apply<int>();
				{
					for(int i{-1}; i < 3; ++i)
					{
						auto& e(database.get(OBLETType(ssvu::getSIMod(brushIdx + i, database.getSize()))));
						sf::Sprite s{*e.texture, e.intRect};
						Vec2f origin{5.f, 5.f};
						s.setScale(10.f / s.getTextureRect().width, 10.f / s.getTextureRect().height);
						s.setOrigin(origin); s.setPosition(15 + origin.x + (12 * i), 240 - 15 + origin.y);
						if(e.defaultParams.count("rot") > 0) s.setRotation(currentRot);
						render(s);

						if(i != 0) continue;
						sf::RectangleShape ind{Vec2f{3.2f, 3.2f}};
						ind.setFillColor(sf::Color::White); ind.setOrigin(origin);
						ind.setPosition(15 + 3.5f + origin.x + (12 * i), 240 - 18.5f + origin.y);
						render(ind);
					}

					render(paramsText);
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
