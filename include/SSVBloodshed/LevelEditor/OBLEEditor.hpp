// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME
#define SSVOB_LEVELEDITOR_GAME

#include <initializer_list>
#include <map>
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/OBSharedData.hpp"
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
#include "SSVBloodshed/GUIOB/FormIO.hpp"

namespace ob
{
    class FormPack;
    class FormParams;

    class OBLEEditor
    {
        friend FormPack;
        friend FormParams;
        template <typename>
        friend class OBLEGInput;
        template <typename>
        friend class OBLEGDebugText;

    private:
        ssvs::GameWindow& gameWindow;
        OBAssets& assets;
        ssvs::Camera gameCamera{gameWindow, 2.f},
            overlayCamera{gameWindow, 2.f};
        ssvs::GameState gameState;
        OBLEGInput<OBLEEditor> input{*this};
        OBLEGDebugText<OBLEEditor> debugText{*this};

        OBSharedData sharedData;

        std::vector<OBLETile*> currentTiles;
        OBLEBrush brush{{0, 0, levelCols, levelRows}};
        int currentZ{0}, currentRot{0}, currentId{-1};
        OBGame* game{nullptr};
        std::pair<OBLETType, std::map<std::string, ssvj::Val>> copiedParams{
            OBLETType::LETFloor, {}};

        OBLETileData copiedTile;

        GUI::Context guiCtx;

        GUI::Form* formMenu{nullptr};
        GUI::CheckBox* chbShowId{nullptr};
        GUI::CheckBox* chbOnion{nullptr};

        GUI::Form* formInfo{nullptr};
        GUI::Label* lblInfo{nullptr};

        FormPack* formPack{nullptr};
        FormIO* formIO{nullptr};

    public:
        inline OBLEEditor(ssvs::GameWindow& mGameWindow, OBAssets& mAssets)
            : gameWindow(mGameWindow), assets(mAssets),
              guiCtx(assets, gameWindow, GUI::Style{*assets.obStroked})
        {
            gameCamera.pan(-5, -5);
            gameState.onUpdate += [this](FT mFT)
            {
                update(mFT);
            };
            gameState.onDraw += [this]
            {
                draw();
            };

            gameState.onAnyEvent += [this](const sf::Event& mEvent)
            {
                guiCtx.onAnyEvent(mEvent);
            };

            formMenu = &guiCtx.create<GUI::Form>(
                "MENU", Vec2f{400, 100}, Vec2f{64, 80});
            formMenu->setResizable(false);
            formMenu->setPadding(4.f);
            formMenu->setScaling(GUI::Scaling::FitToChildren);
            formMenu->show();

            auto& btnInfo(formMenu->create<GUI::Button>(
                "info", guiCtx.getStyle().getBtnSizePerChar(7)));
            btnInfo.onLeftClick += [this]
            {
                formInfo->show();
            };
            btnInfo.attach(
                GUI::At::Top, *formMenu, GUI::At::Top, Vec2f{0.f, 4.f});

            auto& shtrOptions(formMenu->create<GUI::Shutter>(
                "options", guiCtx.getStyle().getBtnSizePerChar(7)));
            auto& shtrOptionsInside(shtrOptions.getShutter());
            shtrOptions.attach(
                GUI::At::Top, btnInfo, GUI::At::Bottom, Vec2f{0.f, 6.f});

            chbShowId =
                &shtrOptionsInside.create<GUI::CheckBox>("show id", true);
            chbOnion = &shtrOptionsInside.create<GUI::CheckBox>("onion", true);

            auto& shtrList(formMenu->create<GUI::Shutter>(
                "list 1", guiCtx.getStyle().getBtnSizePerChar(7)));
            auto& shtrListInside(shtrList.getShutter());
            shtrList.attach(
                GUI::At::Top, shtrOptions, GUI::At::Bottom, Vec2f{0.f, 6.f});
            shtrListInside.create<GUI::Label>("hello");
            shtrListInside.create<GUI::Label>("how");
            shtrListInside.create<GUI::Label>("are");
            shtrListInside.create<GUI::Label>("you");

            auto& shtrList2(shtrList.getShutter().create<GUI::Shutter>(
                "list 2", guiCtx.getStyle().getBtnSizePerChar(7)));
            auto& shtrList2Inside(shtrList2.getShutter());
            shtrList2Inside.create<GUI::Label>("i'm");
            shtrList2Inside.create<GUI::Label>("fine");
            shtrList2Inside.create<GUI::Label>("thanks");
            shtrList2Inside.create<GUI::Label>("bro");

            shtrListInside.create<GUI::Button>("yomrwhite", Vec2f{56, 8});

            formInfo = &guiCtx.create<GUI::Form>(
                "INFO", Vec2f{100, 100}, Vec2f{150, 80});
            lblInfo = &formInfo->create<GUI::Label>();
            lblInfo->attach(
                GUI::At::NW, *formInfo, GUI::At::NW, Vec2f{2.f, 2.f});

            formPack = &guiCtx.create<FormPack>(*this);
            formIO = &guiCtx.create<FormIO>();

            formIO->onSave += [this](const std::string& mFilename)
            {
                savePackToFile(mFilename);
                formIO->getLblCurrentPath().setString(
                    "CURRENT: " + sharedData.getCurrentPath());
            };
            formIO->onLoad += [this](const std::string& mFilename)
            {
                ssvufs::Path path{mFilename};

                if(!path.exists<ssvufs::Type::File>()) return;
                loadPackFromFile(path);
                formIO->getLblCurrentPath().setString(
                    "CURRENT: " + sharedData.getCurrentPath());
            };
        }

        template <typename T = FormParams>
        inline void createFormParams(OBLETile& mTile)
        {
            if(mTile.getParams().empty()) return;
            guiCtx.create<T>(*this, mTile.getX(), mTile.getY(), mTile.getZ());
        }

        inline void newPack()
        {
            sharedData.createEmptyPack();
            loadSector(0);
            clearCurrentLevel();
        }

        inline void loadPackFromFile(const ssvufs::Path& mPath)
        {
            sharedData.loadPack(mPath);
            loadSector(0);
        }
        inline void savePackToFile(const ssvufs::Path& mPath)
        {
            sharedData.savePack(mPath);
        }

        template <typename TFormPack = FormPack>
        inline void loadSector(int mIdx)
        {
            sharedData.setCurrentSector(mIdx);
            loadLevel(0, 0);
            ssvu::castUp<TFormPack>(formPack)->syncFromPack();
        }
        inline void loadLevel(int mX, int mY)
        {
            sharedData.setCurrentLevel(mX, mY);
            for(auto& t : sharedData.getCurrentTiles())
                if(t.second.hasParam("id")) t.second.refreshIdText(assets);
        }
        inline void clearCurrentSector()
        {
            sharedData.getCurrentSector().clear();
            loadLevel(0, 0);
        }
        inline void clearCurrentLevel()
        {
            sharedData.getCurrentLevel().clear(
                sharedData.getDatabase().get(OBLETType::LETFloor));
        }

        inline void grabTiles()
        {
            brush.setPosition(
                Vec2i((gameCamera.getMousePosition() + Vec2f(5, 5)) / 10.f));
            currentTiles.clear();

            for(int iY{brush.getTop()}; iY < brush.getBottom(); ++iY)
                for(int iX{brush.getLeft()}; iX < brush.getRight(); ++iX)
                    if(sharedData.isTileValid(iX, iY, currentZ))
                        currentTiles.emplace_back(
                            &sharedData.getCurrentLevel().getTile(
                                iX, iY, currentZ));
        }

        inline OBLETile& getPickTile() const noexcept
        {
            return sharedData.getCurrentLevel().getTile(
                brush.getX(), brush.getY(), currentZ);
        }

        inline void paint()
        {
            for(auto& t : currentTiles)
            {
                t->initFromEntry(getCurrentEntry());
                t->setRot(currentRot);
                t->setId(assets, currentId);
            }
        }
        inline void del()
        {
            for(auto& t : currentTiles)
            {
                sharedData.getCurrentLevel().del(*t);
            }
        }
        inline void pick()
        {
            const auto& type(int(getPickTile().getType()));
            if(type != -1) brush.setIdx(type);
        }
        inline void openParams() { createFormParams(getPickTile()); }

        inline void copyTiles()
        {
            auto& t(getPickTile());
            copiedTile = t.getData();
        }
        inline void pasteTiles()
        {
            for(auto& t : currentTiles)
            {
                t->initFromEntry(sharedData.getDatabase().get(copiedTile.type));
                t->setParams(copiedTile.params);
                t->refreshIdText(assets);
            }
        }

        inline void cycleRot(int mDeg)
        {
            currentRot = ssvu::getWrapDeg(currentRot + mDeg);
        }
        inline void cycleId(int mDir) { currentId += mDir; }
        inline void cycleBrush(int mDir)
        {
            brush.setIdx(ssvu::getMod(
                brush.getIdx() + mDir, sharedData.getDatabase().getSize()));
        }
        inline void cycleZ(int mDir)
        {
            currentZ = -ssvu::getMod(-currentZ + mDir, 3);
        }
        inline void cycleBrushSize(int mDir)
        {
            brush.setSize(ssvu::getClamped(brush.getSize() + mDir, 1, 20));
        }
        inline void cycleLevel(int mDirX, int mDirY)
        {
            loadLevel(sharedData.getCurrentLevelX() + mDirX,
                sharedData.getCurrentLevelY() + mDirY);
        }

        inline void update(FT mFT)
        {
            guiCtx.update(mFT);

            if(!sharedData.isCurrentLevelNull())
            {
                for(auto& p : sharedData.getCurrentTiles())
                {
                    auto& tile(p.second);

                    if(tile.getType() != OBLETType::LETNull)
                        tile.initGfxFromEntry(sharedData.getDatabase().get(
                            OBLETType(tile.getType())));
                }

                sharedData.getCurrentLevel().update();
                grabTiles();

                if(!guiCtx.isInUse())
                {
                    if(input.painting)
                        paint();
                    else if(input.deleting)
                        del();
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
                if(!sharedData.isCurrentLevelNull())
                    sharedData.getCurrentLevel().draw(gameWindow,
                        chbOnion->getState(), chbShowId->getState(), currentZ);
                render(brush);
            }
            gameCamera.unapply();

            overlayCamera.apply<int>();
            {
                for(int i{-1}; i < 25; ++i)
                {
                    auto& e(sharedData.getDatabase().get(
                        OBLETType(ssvu::getMod(brush.getIdx() + i - 2,
                            sharedData.getDatabase().getSize()))));
                    sf::Sprite s{*e.texture, e.intRect};
                    Vec2f origin{s.getTextureRect().width / 2.f,
                        s.getTextureRect().height / 2.f};
                    s.setScale(10.f / s.getTextureRect().width,
                        10.f / s.getTextureRect().height);
                    s.setOrigin(origin);
                    s.setPosition(20 + (12 * i), 230);
                    if(e.defaultParams.count("rot") > 0)
                        s.setRotation(currentRot);
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

        template <typename... TArgs>
        inline void render(const sf::Drawable& mDrawable, TArgs&&... mArgs)
        {
            gameWindow.draw(mDrawable, FWD(mArgs)...);
        }

        inline void setGame(OBGame& mGame) noexcept { game = &mGame; }
        inline void setDatabase(OBLEDatabase& mDatabase) noexcept
        {
            sharedData.setDatabase(mDatabase);
        }
        inline ssvs::GameWindow& getGameWindow() noexcept { return gameWindow; }
        inline OBAssets& getAssets() noexcept { return assets; }
        inline ssvs::GameState& getGameState() noexcept { return gameState; }
        inline const decltype(input)& getInput() const noexcept
        {
            return input;
        }
        inline const OBLEDatabaseEntry& getCurrentEntry()
        {
            return sharedData.getDatabase().get(OBLETType(brush.getIdx()));
        }
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
        FormPack(GUI::Context& mCtx, OBLEEditor& mEditor)
            : GUI::Form{mCtx, "PACK", Vec2f{300.f, 300.f}, Vec2f{100.f, 100.f}},
              editor(mEditor), mainStrip(create<GUI::Strip>(GUI::At::Top,
                                   GUI::At::Bottom, GUI::At::Bottom)),
              tboxName(mainStrip.create<GUI::TextBox>(
                  getStyle().getBtnSize(56.f * 2))),
              shtrSectors(mainStrip.create<GUI::ChoiceShutter>(
                  std::initializer_list<std::string>{},
                  getStyle().getBtnSizePerChar(7))),
              tboxSectorIdx(mainStrip.create<GUI::TextBox>(
                  getStyle().getBtnSizePerChar(7))),
              btnAddSector(mainStrip.create<GUI::Button>(
                  "add sector", getStyle().getBtnSizePerChar(7)))
        {
            setScaling(GUI::Scaling::FitToChildren);
            setResizable(false);
            setPadding(2.f);

            mainStrip.attach(GUI::At::Center, *this, GUI::At::Center);
            mainStrip.setPadding(2.f);

            tboxName.onTextChanged += [this]
            {
                editor.sharedData.getPack().setName(tboxName.getString());
            };
            shtrSectors.onChoiceSelected += [this]
            {
                editor.loadSector(ssvu::sToInt(shtrSectors.getChoice()));
            };
            btnAddSector.onLeftClick += [this]
            {
                if(tboxSectorIdx.getString().empty()) return;
                shtrSectors.addChoice(tboxSectorIdx.getString());
            };

            syncFromPack();
        }

        inline void syncFromPack()
        {
            const auto& pack(editor.sharedData.getPack());

            tboxName.setString(pack.getName());
            shtrSectors.clearChoices();

            for(const auto& s : pack.getSectors())
                shtrSectors.addChoice(ssvu::toStr(s.first));
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
            if(editor.sharedData.isCurrentLevelNull() ||
                !editor.sharedData.isTileValid(x, y, z))
                return nullptr;
            return &editor.sharedData.getCurrentLevel().getTile(x, y, z);
        }

        inline void refreshTile()
        {
            auto tile(getTile());

            // If the tile is invalid or has no parameters, destroy the form
            if(tile == nullptr || tile->getParams().empty())
            {
                destroyRecursive();
                return;
            }

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

                    if(tile->getParams()[p.first].is<std::string>())
                    {
                        p.second->setString(
                            tile->getParam<std::string>(p.first));
                    }
                    else if(tile->getParams()[p.first].is<ssvj::IntS>())
                    {
                        p.second->setString(
                            ssvu::toStr(tile->getParam<int>(p.first)));
                    }
                    else if(tile->getParams()[p.first].is<ssvj::Real>())
                    {
                        p.second->setString(
                            ssvu::toStr(tile->getParam<float>(p.first)));
                    }
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
            checkBoxes.clear();
            textBoxes.clear();
            enumChoiceShutters.clear();
            mainStrip.recurseChildren<false>([](Widget& mW)
                {
                    mW.destroyRecursive();
                });

            // Set the previous type to the current type
            prevType = tile->getType();

            // And build an interface for the tile's parameters
            for(auto& p : tile->getParams())
            {
                auto key(p.first);

                GUI::Strip& strip(mainStrip.create<GUI::Strip>(
                    GUI::At::Left, GUI::At::Right, GUI::At::Right));
                strip.create<GUI::Label>(key);
                strip.setTabSize(100.f);

                const auto& entry(
                    editor.sharedData.getDatabase().get(tile->getType()));

                if(entry.isEnumParam(key))
                {
                    // Enum parameters
                    auto& choiceShutter(strip.create<GUI::ChoiceShutter>(
                        getEnumStrVecByName(entry.getEnumName(key)),
                        getStyle().getBtnSizePerChar(7)));
                    choiceShutter.onChoiceSelected +=
                        [key, tile, &choiceShutter]
                    {
                        tile->setParam(
                            key, ssvu::toStr(choiceShutter.getChoiceIdx()));
                    };
                    choiceShutter.getShutter().setScalingX(
                        GUI::Scaling::Manual);
                    choiceShutter.getShutter().setWidth(100.f);
                    enumChoiceShutters[key] = &choiceShutter;
                }
                else
                {
                    // Generic parameters (bool, textbox)
                    if(p.second.is<bool>())
                    {
                        auto& checkBox(strip.create<GUI::CheckBox>(
                            "on", p.second.as<bool>()));
                        checkBox.onStateChanged += [key, tile, &checkBox]
                        {
                            tile->setParam(
                                key, checkBox.getState() ? "true" : "false");
                        };
                        checkBoxes[key] = &checkBox;
                    }
                    else
                    {
                        auto& textBox(strip.create<GUI::TextBox>(
                            getStyle().getBtnSizePerChar(7)));
                        auto str(ssvu::toStr(p.second));

                        {
                            auto i(str.size());
                            while(i > 0 && str[i - 1] == '\n') --i;
                            str.erase(i, str.size());
                        }

                        textBox.setString(str);
                        textBox.onTextChanged += [this, key, tile, &textBox]
                        {
                            tile->setParam(key, textBox.getString());
                            tile->refreshIdText(editor.getAssets());
                        };
                        textBoxes[key] = &textBox;
                    }
                }
            }
        }

    public:
        FormParams(
            GUI::Context& mCtx, OBLEEditor& mEditor, int mX, int mY, int mZ)
            : GUI::Form{mCtx, "", Vec2f{300.f, 300.f}, Vec2f{100.f, 100.f}},
              editor(mEditor), x{mX}, y{mY}, z{mZ},
              mainStrip(
                  create<GUI::Strip>(GUI::At::NW, GUI::At::SW, GUI::At::Bottom))
        {
            setTitle("PARAMS (" + ssvu::toStr(x) + ", " + ssvu::toStr(y) +
                     ", " + ssvu::toStr(z) + ")");
            setScaling(GUI::Scaling::FitToChildren);
            setResizable(false);
            setPadding(2.f);

            mainStrip.attach(GUI::At::Center, *this, GUI::At::Center);
            mainStrip.setPadding(2.f);

            onPostUpdate += [this]
            {
                refreshTile();
            };
        }
    };
}

#endif
