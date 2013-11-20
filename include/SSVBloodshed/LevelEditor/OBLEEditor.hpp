// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME
#define SSVOB_LEVELEDITOR_GAME

#include <initializer_list>
#include <map>
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/LevelEditor/OBLEGDebugText.hpp"
#include "SSVBloodshed/LevelEditor/OBLEGInput.hpp"
#include "SSVBloodshed/LevelEditor/OBLETile.hpp"
#include "SSVBloodshed/LevelEditor/OBLELevel.hpp"
#include "SSVBloodshed/LevelEditor/OBLESector.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"
#include "SSVBloodshed/LevelEditor/OBLEJson.hpp"
#include "SSVBloodshed/GUI/GUI.hpp"

namespace ob
{
	class ParamsForm;

	class OBLEEditor
	{
		template<typename> friend class OBLEGInput;
		template<typename> friend class OBLEGDebugText;

		private:
			struct Brush { int idx{0}, size{1}, left{0}, top{0}, x{0}, y{0}; };

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
			Brush brush;
			int currentZ{0}, currentRot{0}, currentId{-1}, currentParamIdx{0};
			OBGame* game{nullptr};
			std::pair<OBLETType, std::map<std::string, ssvuj::Obj>> copiedParams{OBLETType::LETFloor, {}};

			OBLETile copiedTile;

			sf::RectangleShape rsBrush{Vec2f{10.f * brush.size, 10.f * brush.size}}, rsBrushSingle{Vec2f{10.f, 10.f}};

			GUI::Context guiCtx;

			GUI::Form* formMenu{nullptr};
			GUI::CheckBox* chbShowId{nullptr};
			GUI::CheckBox* chbOnion{nullptr};

			GUI::Form* formParams{nullptr};
			GUI::Label* lblParams{nullptr};

			GUI::Form* formInfo{nullptr};
			GUI::Label* lblInfo{nullptr};

		public:
			inline OBLEEditor(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets), database{assets},
				guiCtx(assets, gameWindow)
			{
				gameCamera.pan(-5, -5);
				gameState.onUpdate += [this](float mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				rsBrush.setFillColor({255, 0, 0, 125});
				rsBrush.setOutlineColor({255, 255, 0, 125});
				rsBrush.setOutlineThickness(0.5f);
				rsBrush.setOrigin(5.f, 5.f);

				rsBrushSingle.setFillColor(sf::Color::Transparent);
				rsBrushSingle.setOutlineColor({255, 255, 0, 125});
				rsBrushSingle.setOutlineThickness(0.65f);
				rsBrushSingle.setOrigin(5.f, 5.f);

				newSector();

				getGameState().onAnyEvent += [this](const sf::Event& mEvent){ guiCtx.onAnyEvent(mEvent); };

				formMenu = &guiCtx.create<GUI::Form>("MENU", Vec2f{400, 100}, Vec2f{64, 80});
				formMenu->setResizable(false);
				auto& tbox(formMenu->create<GUI::TextBox>(Vec2f{56, 8.f}));
				tbox.attach(GUI::At::Top, *formMenu, GUI::At::Top, Vec2f{0.f, 4.f});

				formMenu->show();

				auto& btnParams(formMenu->create<GUI::Button>("parameters", Vec2f{56, 8}));
				btnParams.onUse += [this]{ formParams->show(); };
				btnParams.attach(GUI::At::Top, tbox, GUI::At::Bottom, Vec2f{0.f, 6.f});

				auto& btnInfo(formMenu->create<GUI::Button>("info", Vec2f{56, 8}));
				btnInfo.onUse += [this]{ formInfo->show(); };
				btnInfo.attach(GUI::At::Top, btnParams, GUI::At::Bottom, Vec2f{0.f, 6.f});

				chbShowId = &formMenu->create<GUI::CheckBox>("show id", true);
				chbOnion = &formMenu->create<GUI::CheckBox>("onion", true);

				auto& shtrOptions(formMenu->create<GUI::Shutter>("options"));
				shtrOptions.attach(GUI::At::Top, btnInfo, GUI::At::Bottom, Vec2f{0.f, 6.f});
				shtrOptions += {chbShowId, chbOnion};

				//chbShowId->attach(GUI::At::NW, btnInfo, GUI::At::SW, Vec2f{0.f, 6.f});
				//chbOnion->attach(GUI::At::NW, *chbShowId, GUI::At::SW, Vec2f{0.f, 6.f});

				auto& shtrList(formMenu->create<GUI::Shutter>("list 1"));
				shtrList.attach(GUI::At::Top, shtrOptions, GUI::At::Bottom, Vec2f{0.f, 6.f});
				shtrList.getShutter().create<GUI::Label>("hello");
				shtrList.getShutter().create<GUI::Label>("how");
				shtrList.getShutter().create<GUI::Label>("are");
				shtrList.getShutter().create<GUI::Label>("you");

				auto& shtrList2(shtrList.getShutter().create<GUI::Shutter>("list 2"));
				shtrList2.getShutter().create<GUI::Label>("i'm");
				shtrList2.getShutter().create<GUI::Label>("fine");
				shtrList2.getShutter().create<GUI::Label>("thanks");
				shtrList2.getShutter().create<GUI::Label>("bro");

				shtrList.getShutter().create<GUI::Button>("yomrwhite", Vec2f{56, 8});

				formParams = &guiCtx.create<GUI::Form>("PARAMETERS", Vec2f{100, 100}, Vec2f{150, 80});
				lblParams = &formParams->create<GUI::Label>();
				lblParams->attach(GUI::At::NW, *formParams, GUI::At::NW, Vec2f{2.f, 2.f});
				std::initializer_list<std::string> choices{"test1", "test2", "test3"};
				auto& test(formParams->create<GUI::ChoiceShutter>(choices));
				test.attach(GUI::At::NW, *formParams, GUI::At::NW);

				formInfo = &guiCtx.create<GUI::Form>("INFO", Vec2f{100, 100}, Vec2f{150, 80});
				lblInfo = &formInfo->create<GUI::Label>();
				lblInfo->attach(GUI::At::NW, *formInfo, GUI::At::NW, Vec2f{2.f, 2.f});
			}

			template<typename T = ParamsForm> inline void createParamsForm(OBLETile& mTile)
			{
				if(mTile.getParams().empty()) return;
				guiCtx.create<T>(*this, mTile.getX(), mTile.getY(), mTile.getZ());
			}

			inline void newSector() { sector.clear(); sector.init(database); refreshCurrentLevel(); clearCurrentLevel(); }

			inline void refreshCurrentLevel()	{ currentLevel = &sector.getLevel(currentLevelX, currentLevelY); }
			inline void clearCurrentLevel()		{ *currentLevel = {levelRows, levelColumns, database.get(OBLETType::LETFloor)}; refreshCurrentTiles(); }
			inline void refreshCurrentTiles()	{ for(auto& t : currentLevel->getTiles()) t.second.refreshIdText(assets); }

			inline void updateXY()
			{
				const auto& fColumns(static_cast<float>(currentLevel->getColumns()));
				const auto& fRows(static_cast<float>(currentLevel->getRows()));
				const auto& tileVec((gameCamera.getMousePosition() + Vec2f(10, 10)) / 10.f);

				brush.left =	ssvu::getClamped(tileVec.x - brush.size / 2.f, 0.f, fColumns - brush.size);
				brush.top =		ssvu::getClamped(tileVec.y - brush.size / 2.f, 0.f, fRows - brush.size);
				brush.x =		ssvu::getClamped(tileVec.x - 1 / 2.f, 0.f, fColumns - 1);
				brush.y =		ssvu::getClamped(tileVec.y - 1 / 2.f, 0.f, fRows - 1);
			}
			inline void grabTiles()
			{
				currentTiles.clear();

				for(int iY{0}; iY < brush.size; ++iY)
					for(int iX{0}; iX < brush.size; ++iX)
						if(currentLevel->isValid(brush.left + iX, brush.top + iY, currentZ))
							currentTiles.push_back(&currentLevel->getTile(brush.left + iX, brush.top + iY, currentZ));
			}

			inline OBLETile& getPickTile() const noexcept { return currentLevel->getTile(brush.x, brush.y, currentZ); }

			inline void paint()			{ for(auto& t : currentTiles) { t->initFromEntry(getCurrentEntry()); t->setRot(currentRot); t->setId(assets, currentId); } }
			inline void del()			{ for(auto& t : currentTiles) { currentLevel->del(*t); } }
			inline void pick()			{ brush.idx = int(getPickTile().getType()); }
			inline void openParams()	{ createParamsForm(getPickTile()); }

			inline void copyTiles()		{ auto& t(getPickTile()); copiedTile = t; }
			inline void pasteTiles()	{ for(auto& t : currentTiles) { t->initFromEntry(database.get(copiedTile.getType())); t->setParams(copiedTile.getParams()); t->refreshIdText(assets); } }

			inline void cycleRot(int mDeg)			{ currentRot = ssvu::wrapDeg(currentRot + mDeg); }
			inline void cycleId(int mDir)			{ currentId += mDir; }
			inline void cycleParam(int mDir)		{ currentParamIdx += mDir; }
			inline void cycleCurrentParam(int mDir)
			{
				auto& t(getPickTile());

				int idx{0};
				for(auto& p : t.getParams())
				{
					if(ssvu::getWrapIdx(currentParamIdx, static_cast<int>(t.getParams().size())) == idx++)
					{
						if(ssvuj::is<int>(p.second)) p.second = ssvuj::as<int>(p.second) + mDir;
						else if(ssvuj::is<float>(p.second)) p.second = ssvuj::as<float>(p.second) + float(mDir);
						else if(ssvuj::is<bool>(p.second)) p.second = !ssvuj::as<bool>(p.second);
					}
				}
			}
			inline void cycleBrush(int mDir)				{ brush.idx = ssvu::getWrapIdx(brush.idx + mDir, database.getSize()); }
			inline void cycleZ(int mDir)					{ currentZ = -ssvu::getWrapIdx(-currentZ + mDir, 3); }
			inline void cycleBrushSize(int mDir)			{ brush.size = ssvu::getClamped(brush.size + mDir, 1, 20); }
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
				std::ostringstream ss;
				std::string str;

				int idx{0};
				for(const auto& p : t.getParams())
				{
					bool cp{ssvu::getWrapIdx(currentParamIdx, static_cast<int>(t.getParams().size())) == idx++};
					std::string pName(cp ? ">> " : ""); pName += "[" + p.first + "]";
					ss << std::left << std::setw(22) << pName << ssvu::toStr(p.second);
				}

				lblParams->setString(ss.str());
			}

			inline void update(float mFT)
			{
				guiCtx.update(mFT);

				currentLevel->update();
				updateXY(); grabTiles(); updateParamsText();

				if(!guiCtx.isInUse())
				{
					if(input.painting) paint();
					else if(input.deleting) del();
				}

				debugText.update(mFT);
				lblInfo->setString(debugText.getStr());

				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				{
					currentLevel->draw(gameWindow, chbOnion->getState(), chbShowId->getState(), currentZ);
					rsBrush.setSize({brush.size * 10.f, brush.size * 10.f});
					rsBrush.setPosition(brush.left * 10.f, brush.top * 10.f);
					rsBrushSingle.setPosition(brush.x * 10.f, brush.y * 10.f);
					render(rsBrush); render(rsBrushSingle);
				}
				gameCamera.unapply();

				overlayCamera.apply<int>();
				{
					for(int i{-1}; i < 25; ++i)
					{
						auto& e(database.get(OBLETType(ssvu::getWrapIdx(brush.idx + i, database.getSize()))));
						sf::Sprite s{*e.texture, e.intRect};
						Vec2f origin{s.getTextureRect().width / 2.f, s.getTextureRect().height / 2.f};
						s.setScale(10.f / s.getTextureRect().width, 10.f / s.getTextureRect().height);
						s.setOrigin(origin); s.setPosition(20 + (12 * i), 230);
						if(e.defaultParams.count("rot") > 0) s.setRotation(currentRot);
						render(s);

						if(i != 0) continue;
						sf::RectangleShape ind{Vec2f{3.2f, 3.2f}};
						ind.setFillColor(sf::Color::White); ind.setOrigin(origin);
						ind.setPosition(15 + 3.5f + origin.x + (12 * i), 240 - 18.5f + origin.y);
						render(ind);
					}
				}
				overlayCamera.unapply();

				guiCtx.draw();
			}

			template<typename... TArgs> inline void render(const sf::Drawable& mDrawable, TArgs&&... mArgs)	{ gameWindow.draw(mDrawable, std::forward<TArgs>(mArgs)...); }

			inline void setGame(OBGame& mGame)						{ game = &mGame; }

			inline ssvs::GameWindow& getGameWindow() noexcept		{ return gameWindow; }
			inline OBAssets& getAssets() noexcept					{ return assets; }
			inline ssvs::GameState& getGameState() noexcept			{ return gameState; }
			inline const decltype(input)& getInput() const noexcept	{ return input; }
			inline OBLEDatabase& getDatabase() noexcept				{ return database; }
			inline const OBLEDatabaseEntry& getCurrentEntry() const	{ return database.get(OBLETType(brush.idx)); }
			inline OBLELevel* getCurrentLevel() const noexcept		{ return currentLevel; }
	};

	class ParamsForm : public GUI::Form
	{
		private:
			OBLEEditor& editor;
			int x, y, z;
			GUI::Strip& mainStrip;
			OBLETType prevType{OBLETType::LETFloor};

			std::vector<std::string> enemyTypeChoices
			{
				"runner",		// 0
				"runner_a",		// 1
				"charger",		// 2
				"charger_a",	// 3
				"charger_gl",	// 4
				"jugger",		// 5
				"jugger_a",		// 6
				"jugger_rl",	// 7
				"giant",		// 8
				"enforcer",		// 9
				"ball",			// 10
				"ball_fly"		// 11
			};


			std::map<std::string, GUI::CheckBox*> checkBoxes;
			std::map<std::string, GUI::TextBox*> textBoxes;
			std::map<std::string, GUI::ChoiceShutter*> enumChoiceShutters;

			inline OBLETile* getTile()
			{
				auto currentLevel(editor.getCurrentLevel());
				if(currentLevel == nullptr || !currentLevel->isValid(x, y, z)) return nullptr;
				return &currentLevel->getTile(x, y, z);
			}

			inline void refreshTile()
			{
				auto tile(getTile());

				// If the tile is invalid or has no parameters, destroy the form
				if(tile == nullptr || tile->getParams().empty()) { destroyRecursive(); return; }

				// If the tile's type hasn't changed...
				if(tile->getType() == prevType)
				{
					// Update unfocused widgets to match the tile's params

					for(auto& p : checkBoxes)
					{
						if(p.second->isFocused()) continue;
						bool state(tile->getParam<bool>(p.first));
						if(p.second->getState() != state) p.second->setState(state);
					}
					for(auto& p : textBoxes)
					{
						if(p.second->isFocused()) continue;
						p.second->setString(tile->getParam<std::string>(p.first));
					}
					for(auto& p : enumChoiceShutters)
					{
						if(p.second->isFocused()) continue;
						p.second->setChoiceIdx(tile->getParam<int>(p.first));
					}

					return;
				}

				// Else, if the tile's type has changed...

				// Clear references to widgets and destroy everything but the form
				checkBoxes.clear(); textBoxes.clear(); enumChoiceShutters.clear();
				mainStrip.recurseChildren<false>([](Widget& mW){ mW.destroyRecursive(); });

				// Set the previous type to the current type
				prevType = tile->getType();

				// And build an interface for the tile's parameters
				for(auto& p : tile->getParams())
				{
					auto key(p.first);

					GUI::Strip& strip(mainStrip.create<GUI::Strip>(GUI::At::Left, GUI::At::Right, GUI::At::Right));
					strip.create<GUI::Label>(key);
					strip.setTabSize(100.f);

					const auto& entry(editor.getDatabase().get(tile->getType()));

					if(entry.isEnumParam(key))
					{
						// Enum parameters
						auto& choiceShutter(strip.create<GUI::ChoiceShutter>(getEnumStrVecByName(entry.getEnumName(key))));
						choiceShutter.onChoiceSelected += [key, tile, &choiceShutter]{ tile->setParam(key, ssvu::toStr(choiceShutter.getChoiceIdx())); };
						choiceShutter.getBar().setScalingX(GUI::Scaling::Manual); choiceShutter.getBar().setWidth(56.f);
						choiceShutter.getShutter().setScalingX(GUI::Scaling::Manual); choiceShutter.getShutter().setWidth(100.f);
						enumChoiceShutters[key] = &choiceShutter;
					}
					else
					{
						// Generic parameters (bool, textbox)
						if(ssvuj::is<bool>(p.second))
						{
							auto& checkBox(strip.create<GUI::CheckBox>("on", ssvuj::as<bool>(p.second)));
							checkBox.onStateChanged += [key, tile, &checkBox]{ tile->setParam(key, checkBox.getState() ? "true" : "false"); };
							checkBoxes[key] = &checkBox;
						}
						else
						{
							auto& textBox(strip.create<GUI::TextBox>(Vec2f(56.f, 8.f)));
							auto str(ssvu::toStr(p.second));

							{
								auto i(str.size());
								while(i > 0 && str[i - 1] == '\n') --i;
								str.erase(i, str.size());
							}

							textBox.setString(str);
							textBox.onTextChanged += [key, tile, &textBox]{ tile->setParam(key, textBox.getString()); };
							textBoxes[key] = &textBox;
						}
					}
				}
			}

		public:
			ParamsForm(GUI::Context& mCtx, OBLEEditor& mEditor, int mX, int mY, int mZ) : GUI::Form{mCtx, "", Vec2f{300.f, 300.f}, Vec2f{100.f, 100.f}},
				editor(mEditor), x{mX}, y{mY}, z{mZ}, mainStrip(create<GUI::Strip>(GUI::At::NW, GUI::At::SW, GUI::At::Bottom))
			{
				setTitle("PARAMS (" + ssvu::toStr(x) + ", " + ssvu::toStr(y) + ", " + ssvu::toStr(z) + ")");
				setScaling(GUI::Scaling::FitToChildren);
				setResizable(false); setPadding(2.f);

				mainStrip.attach(GUI::At::Center, *this, GUI::At::Center);
				mainStrip.setPadding(2.f);

				onPostUpdate += [this]{ refreshTile(); };
			}
	};
}

#endif
