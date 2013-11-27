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
#include "SSVBloodshed/LevelEditor/OBLEPack.hpp"
#include "SSVBloodshed/LevelEditor/OBLEDatabase.hpp"
#include "SSVBloodshed/LevelEditor/OBLEJson.hpp"
#include "SSVBloodshed/LevelEditor/OBLEBrush.hpp"
#include "SSVBloodshed/GUI/GUI.hpp"

namespace ob
{
	class FormPack;
	class FormParams;

	class OBLEEditor
	{
		friend class FormPack;
		friend class FormParams;
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

			OBLEPack pack;
			OBLESector* currentSector{nullptr};
			OBLELevel* currentLevel{nullptr};
			int currentSectorIdx{0}, currentLevelX{0}, currentLevelY{0};

			std::vector<OBLETile*> currentTiles;
			OBLEBrush brush{{0, 0, levelColumns, levelRows}};
			int currentZ{0}, currentRot{0}, currentId{-1};
			OBGame* game{nullptr};
			std::pair<OBLETType, std::map<std::string, ssvuj::Obj>> copiedParams{OBLETType::LETFloor, {}};

			OBLETile copiedTile;

			GUI::Context guiCtx;

			GUI::Form* formMenu{nullptr};
			GUI::CheckBox* chbShowId{nullptr};
			GUI::CheckBox* chbOnion{nullptr};

			GUI::Form* formInfo{nullptr};
			GUI::Label* lblInfo{nullptr};

			FormPack* formPack{nullptr};

		public:
			inline OBLEEditor(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets), database{assets},
				guiCtx(assets, gameWindow)
			{
				gameCamera.pan(-5, -5);
				gameState.onUpdate += [this](FT mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				newPack();

				gameState.onAnyEvent += [this](const sf::Event& mEvent){ guiCtx.onAnyEvent(mEvent); };

				formMenu = &guiCtx.create<GUI::Form>("MENU", Vec2f{400, 100}, Vec2f{64, 80});
				formMenu->setResizable(false); formMenu->show();

				auto& btnInfo(formMenu->create<GUI::Button>("info", Vec2f{56.f, 8.f}));
				btnInfo.onLeftClick += [this]{ formInfo->show(); };
				btnInfo.attach(GUI::At::Top, *formMenu, GUI::At::Top, Vec2f{0.f, 6.f});

				auto& shtrOptions(formMenu->create<GUI::Shutter>("options", Vec2f{56.f, 8.f}));
				auto& shtrOptionsInside(shtrOptions.getShutter());
				shtrOptions.attach(GUI::At::Top, btnInfo, GUI::At::Bottom, Vec2f{0.f, 6.f});

				chbShowId = &shtrOptionsInside.create<GUI::CheckBox>("show id", true);
				chbOnion = &shtrOptionsInside.create<GUI::CheckBox>("onion", true);

				auto& shtrList(formMenu->create<GUI::Shutter>("list 1", Vec2f{56.f, 8.f}));
				auto& shtrListInside(shtrList.getShutter());
				shtrList.attach(GUI::At::Top, shtrOptions, GUI::At::Bottom, Vec2f{0.f, 6.f});
				shtrListInside.create<GUI::Label>("hello");
				shtrListInside.create<GUI::Label>("how");
				shtrListInside.create<GUI::Label>("are");
				shtrListInside.create<GUI::Label>("you");

				auto& shtrList2(shtrList.getShutter().create<GUI::Shutter>("list 2", Vec2f{56.f, 8.f}));
				auto& shtrList2Inside(shtrList2.getShutter());
				shtrList2Inside.create<GUI::Label>("i'm");
				shtrList2Inside.create<GUI::Label>("fine");
				shtrList2Inside.create<GUI::Label>("thanks");
				shtrList2Inside.create<GUI::Label>("bro");

				shtrListInside.create<GUI::Button>("yomrwhite", Vec2f{56, 8});

				formInfo = &guiCtx.create<GUI::Form>("INFO", Vec2f{100, 100}, Vec2f{150, 80});
				lblInfo = &formInfo->create<GUI::Label>();
				lblInfo->attach(GUI::At::NW, *formInfo, GUI::At::NW, Vec2f{2.f, 2.f});

				formPack = &guiCtx.create<FormPack>(*this);
			}

			template<typename T = FormParams> inline void createFormParams(OBLETile& mTile)
			{
				if(mTile.getParams().empty()) return;
				guiCtx.create<T>(*this, mTile.getX(), mTile.getY(), mTile.getZ());
			}

			inline void newPack()
			{
				pack = OBLEPack{};
				loadSector(0);
				clearCurrentLevel();
			}

			template<typename TFormPack = FormPack> inline void loadPackFromFile(const ssvu::FileSystem::Path& mPath)
			{
				pack = ssvuj::as<OBLEPack>(ssvuj::readFromFile(mPath));
				reinterpret_cast<TFormPack*>(formPack)->syncFromPack();
				loadSector(0);
			}
			inline void loadSector(int mIdx)
			{
				currentSectorIdx = mIdx;
				currentSector = &pack.getSector(currentSectorIdx);
				currentSector->init(database);
				loadLevel(0, 0);
			}
			inline void loadLevel(int mX, int mY)
			{
				if(currentSector == nullptr) { currentLevel = nullptr; return; }
				currentLevelX = mX; currentLevelY = mY;
				currentLevel = &currentSector->getLevel(currentLevelX, currentLevelY);
				refreshTiles();
			}

			inline void clearCurrentSector()
			{
				if(currentSector == nullptr) return;
				currentSector->clear();
				loadLevel(0, 0);
			}

			inline void clearCurrentLevel()
			{
				if(currentLevel == nullptr) return;
				*currentLevel = {levelColumns, levelRows, database.get(OBLETType::LETFloor)};
				refreshTiles();
			}
			inline void refreshTiles() { for(auto& t : currentLevel->getTiles()) t.second.refreshIdText(assets); }

			inline void grabTiles()
			{
				brush.setPosition(Vec2i((gameCamera.getMousePosition() + Vec2f(5, 5)) / 10.f));
				currentTiles.clear();

				for(int iY{brush.getTop()}; iY < brush.getBottom(); ++iY)
					for(int iX{brush.getLeft()}; iX < brush.getRight(); ++iX)
						if(currentLevel->isValid(iX, iY, currentZ))
							currentTiles.push_back(&currentLevel->getTile(iX, iY, currentZ));
			}

			inline OBLETile& getPickTile() const noexcept { return currentLevel->getTile(brush.getX(), brush.getY(), currentZ); }

			inline void paint()			{ for(auto& t : currentTiles) { t->initFromEntry(getCurrentEntry()); t->setRot(currentRot); t->setId(assets, currentId); } }
			inline void del()			{ for(auto& t : currentTiles) { currentLevel->del(*t); } }
			inline void pick()			{ brush.setIdx(int(getPickTile().getType())); }
			inline void openParams()	{ createFormParams(getPickTile()); }

			inline void copyTiles()		{ auto& t(getPickTile()); copiedTile = t; }
			inline void pasteTiles()	{ for(auto& t : currentTiles) { t->initFromEntry(database.get(copiedTile.getType())); t->setParams(copiedTile.getParams()); t->refreshIdText(assets); } }

			inline void cycleRot(int mDeg)					{ currentRot = ssvu::wrapDeg(currentRot + mDeg); }
			inline void cycleId(int mDir)					{ currentId += mDir; }
			inline void cycleBrush(int mDir)				{ brush.setIdx(ssvu::getWrapIdx(brush.getIdx() + mDir, database.getSize())); }
			inline void cycleZ(int mDir)					{ currentZ = -ssvu::getWrapIdx(-currentZ + mDir, 3); }
			inline void cycleBrushSize(int mDir)			{ brush.setSize(ssvu::getClamped(brush.getSize() + mDir, 1, 20)); }
			inline void cycleLevel(int mDirX, int mDirY)	{ loadLevel(currentLevelX + mDirX, currentLevelY + mDirY); }

			inline void savePackToFile(const ssvu::FileSystem::Path& mPath) { ssvuj::writeToFile(ssvuj::getArch(pack), mPath); }

			inline void update(FT mFT)
			{
				guiCtx.update(mFT);

				if(currentLevel != nullptr)
				{
					currentLevel->update();
					grabTiles();

					if(!guiCtx.isInUse())
					{
						if(input.painting) paint();
						else if(input.deleting) del();
					}

					debugText.update(mFT);
					lblInfo->setString(debugText.getStr());
				}

				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				{
					if(currentLevel != nullptr) currentLevel->draw(gameWindow, chbOnion->getState(), chbShowId->getState(), currentZ);
					render(brush);
				}
				gameCamera.unapply();

				overlayCamera.apply<int>();
				{
					for(int i{-1}; i < 25; ++i)
					{
						auto& e(database.get(OBLETType(ssvu::getWrapIdx(brush.getIdx() + i - 2, database.getSize()))));
						sf::Sprite s{*e.texture, e.intRect};
						Vec2f origin{s.getTextureRect().width / 2.f, s.getTextureRect().height / 2.f};
						s.setScale(10.f / s.getTextureRect().width, 10.f / s.getTextureRect().height);
						s.setOrigin(origin); s.setPosition(20 + (12 * i), 230);
						if(e.defaultParams.count("rot") > 0) s.setRotation(currentRot);
						render(s);
					}

					sf::RectangleShape ind{{10.f, 2.f}};
					ind.setFillColor(sf::Color::White);
					ind.setPosition(39.f, 240.f - 18.f);
					render(ind);
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
			inline const OBLEDatabaseEntry& getCurrentEntry() const	{ return database.get(OBLETType(brush.getIdx())); }
	};

	class FormPack : public GUI::Form
	{
		private:
			OBLEEditor& editor;
			GUI::Strip& mainStrip;
			GUI::TextBox& tboxName;
			GUI::ChoiceShutter& shtrSectors;
			GUI::TextBox& tboxSectorIdx;
			GUI::Button& btnAddSector;

		public:
			FormPack(GUI::Context& mCtx, OBLEEditor& mEditor) : GUI::Form{mCtx, "", Vec2f{300.f, 300.f}, Vec2f{100.f, 100.f}},
				editor(mEditor), mainStrip(create<GUI::Strip>(GUI::At::Top, GUI::At::Bottom, GUI::At::Bottom)),
				tboxName(mainStrip.create<GUI::TextBox>(Vec2f{56.f * 2.f, 8.f})),
				shtrSectors(mainStrip.create<GUI::ChoiceShutter>(std::initializer_list<std::string>{}, Vec2f{56.f, 8.f})),
				tboxSectorIdx(mainStrip.create<GUI::TextBox>(Vec2f{56.f, 8.f})),
				btnAddSector(mainStrip.create<GUI::Button>("add sector", Vec2f{56.f, 8.f}))
			{
				setTitle("PACK");
				setScaling(GUI::Scaling::FitToChildren);
				setResizable(false); setPadding(2.f);

				mainStrip.attach(GUI::At::Center, *this, GUI::At::Center);
				mainStrip.setPadding(2.f);

				tboxName.onTextChanged += [this]{ editor.pack.setName(tboxName.getString()); };
				shtrSectors.onChoiceSelected += [this]{ editor.loadSector(std::stoi(shtrSectors.getChoice())); };
				btnAddSector.onLeftClick += [this]
				{
					if(tboxSectorIdx.getString().empty()) return;
					shtrSectors.addChoice(tboxSectorIdx.getString());
				};

				syncFromPack();
			}

			inline void syncFromPack()
			{
				const auto& pack(editor.pack);

				tboxName.setString(pack.getName());
				shtrSectors.clearChoices();

				for(const auto& s : pack.getSectors()) shtrSectors.addChoice(ssvu::toStr(s.first));
			}
	};

	class FormParams : public GUI::Form
	{
		private:
			OBLEEditor& editor;
			int x, y, z;
			GUI::Strip& mainStrip;
			OBLETType prevType{OBLETType::LETFloor};

			std::map<std::string, GUI::CheckBox*> checkBoxes;
			std::map<std::string, GUI::TextBox*> textBoxes;
			std::map<std::string, GUI::ChoiceShutter*> enumChoiceShutters;

			inline OBLETile* getTile()
			{
				if(editor.currentLevel == nullptr || !editor.currentLevel->isValid(x, y, z)) return nullptr;
				return &editor.currentLevel->getTile(x, y, z);
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

					const auto& entry(editor.database.get(tile->getType()));

					if(entry.isEnumParam(key))
					{
						// Enum parameters
						auto& choiceShutter(strip.create<GUI::ChoiceShutter>(getEnumStrVecByName(entry.getEnumName(key)), Vec2f{56.f, 8.f}));
						choiceShutter.onChoiceSelected += [key, tile, &choiceShutter]{ tile->setParam(key, ssvu::toStr(choiceShutter.getChoiceIdx())); };
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
			FormParams(GUI::Context& mCtx, OBLEEditor& mEditor, int mX, int mY, int mZ) : GUI::Form{mCtx, "", Vec2f{300.f, 300.f}, Vec2f{100.f, 100.f}},
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
