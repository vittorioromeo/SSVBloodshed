// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME
#define SSVOB_LEVELEDITOR_GAME

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
	template<typename TGame> class OBLEGInput
	{
		private:
			TGame& game;

		public:
			bool painting{false};

			inline OBLEGInput(TGame& mGame) : game(mGame)
			{
				using k = sf::Keyboard::Key;
				using b = sf::Mouse::Button;
				using t = ssvs::Input::Trigger::Type;

				auto& gs(game.getGameState());

				ssvs::add2StateInput(gs, {{b::Left}}, painting);

				gs.addInput({{k::Escape}}, [this](float){ game.assets.musicPlayer.stop(); std::terminate(); });

				gs.addInput({{k::A}}, [=](float){ game.gameCamera.pan(-4, 0); });
				gs.addInput({{k::D}}, [=](float){ game.gameCamera.pan(4, 0); });
				gs.addInput({{k::W}}, [=](float){ game.gameCamera.pan(0, -4); });
				gs.addInput({{k::S}}, [=](float){ game.gameCamera.pan(0, 4); });
				gs.addInput({{k::Q}}, [=](float){ game.gameCamera.zoomOut(1.1f); });
				gs.addInput({{k::E}}, [=](float){ game.gameCamera.zoomIn(1.1f); });

				gs.addInput({{k::R}}, [this](float){ game.newGame(); }, t::Once);

				gs.addInput({{k::Z}}, [this](float){ game.cycle(-1); }, t::Once);
				gs.addInput({{k::X}}, [this](float){ game.cycle(1); }, t::Once);

				gs.addInput({{k::C}}, [this](float){ game.level.saveToFile("./level.txt"); }, t::Once);
				gs.addInput({{k::V}}, [this](float){ game.level.loadFromFile("./level.txt", game.tileMap); }, t::Once);

				gs.addInput({{k::B}}, [this](float){ game.rotate(-45); }, t::Once);
				gs.addInput({{k::N}}, [this](float){ game.rotate(45); }, t::Once);
			}
	};

	template<typename TGame> class OBLEGDebugText
	{
		private:
			TGame& game;
			ssvs::BitmapText debugText;

		public:
			OBLEGDebugText(TGame& mGame) : game(mGame), debugText{game.getAssets().template get<ssvs::BitmapFont>("fontObStroked")}
			{
				debugText.setTracking(-3);
			}

			inline void update(float mFT)
			{
				std::ostringstream s;
				const auto& entities(game.getManager().getEntities());
				std::size_t componentCount{0};
				for(const auto& e : entities) componentCount += e->getComponents().size();

				s	<< "FPS: "				<< static_cast<int>(game.getGameWindow().getFPS()) << "\t"
					<< "FT: "				<< mFT << "\n"
					<< "Entities: "			<< entities.size() << "\n"
					<< "Components: "		<< componentCount << std::endl;

				debugText.setString(s.str());
			}
			inline void draw() const { game.render(debugText); }
	};

	enum OBLEGTileType : unsigned int
	{
		Floor,
		Wall,
		Grate,
		Pit,
		Turret,
		Spawner,
	};

	struct OBLEGTileData
	{
		OBLEGTileType type;
		sf::Sprite sprite;
		std::map<std::string, ssvuj::Obj> params;

		inline OBLEGTileData() : type{OBLEGTileType::Floor} { }
		inline OBLEGTileData(OBLEGTileType mType, sf::Texture* mTexture, const sf::IntRect& mIntRect, const std::map<std::string, ssvuj::Obj> mParams = {}) : type{mType}, sprite{*mTexture, mIntRect}, params{mParams} { }
	};

	struct OBLEGTile
	{
		int x{0}, y{0};
		OBLEGTileData data;
		std::map<std::string, ssvuj::Obj> params;

		inline OBLEGTile(int mX, int mY, const OBLEGTileData& mData) : x{mX}, y{mY}
		{
			setData(mData);
		}

		inline void rotate(int mDeg)
		{
			if(params.count("rot") == 0) return;
			int currentRot(ssvuj::as<int>(params["rot"]));
			currentRot += mDeg;
			params["rot"] = currentRot;
		}

		inline void update()
		{
			if(params.count("rot") > 0) getSprite().setRotation(ssvuj::as<int>(params["rot"]));
			getSprite().setColor(sf::Color::White);
		}

		inline void setData(const OBLEGTileData& mData)
		{
			data = mData;
			params = data.params;

			getSprite().setOrigin(5, 5);
			getSprite().setPosition(x * 10.f, y * 10.f);
		}
		inline sf::Sprite& getSprite() noexcept { return data.sprite; }
	};


	class OBLEGLevel
	{
		private:
			int columns, rows;
			std::vector<OBLEGTile> tiles;

		public:
			inline OBLEGLevel(int mColumns, int mRows, const OBLEGTileData& mDefaultTileData) : columns{mColumns}, rows{mRows}
			{
				for(int iY{0}; iY < rows; ++iY) for(int iX{0}; iX < columns; ++iX) tiles.emplace_back(iX, iY, mDefaultTileData);
			}

			inline OBLEGTile& getTile(int mX, int mY)	{ return tiles[ssvu::get1DIndexFrom2D(mX, mY, columns)]; }
			inline bool isValid(int mX, int mY)			{ return mX >= 0 && mY >= 0 && mX < columns && mY < rows; }

			inline void update()								{ for(auto& t : tiles) t.update(); }
			inline void draw(sf::RenderTarget& mRenderTarget)	{ for(auto& t : tiles) mRenderTarget.draw(t.getSprite()); }

			inline void saveToFile(const ssvu::FileSystem::Path& mPath)
			{
				unsigned int idx{0};
				ssvuj::Obj root;

				for(int iX{0}; iX < columns; ++iX)
					for(int iY{0}; iY < rows; ++iY)
					{
						OBLEGTile& t(getTile(iX, iY));
						ssvuj::Obj tObj;
						ssvuj::archArray(tObj, t.x, t.y, t.data.type, t.params);
						ssvuj::set(root, idx, tObj);

						++idx;
					}

				ssvuj::writeToFile(root, mPath);
			}

			inline void loadFromFile(const ssvu::FileSystem::Path& mPath, std::map<OBLEGTileType, OBLEGTileData>& mTileMap)
			{
				unsigned int idx{0};
				ssvuj::Obj root{ssvuj::readFromFile(mPath)};

				for(int iX{0}; iX < columns; ++iX)
					for(int iY{0}; iY < rows; ++iY)
					{
						OBLEGTile& t(getTile(iX, iY));

						int dataType;
						std::map<std::string, ssvuj::Obj> params;
						ssvuj::extrArray(ssvuj::get(root, idx), t.x, t.y, dataType, params);

						t.setData(mTileMap[static_cast<OBLEGTileType>(dataType)]);
						t.params = params;

						++idx;
					}
			}
	};

	class OBLEGame
	{
		template<typename> friend class OBLEGInput;

		private:
			ssvs::GameWindow& gameWindow;
			OBAssets& assets;
			ssvs::Camera gameCamera{gameWindow, 2.f}, overlayCamera{gameWindow, 2.f};
			ssvs::GameState gameState;
			sses::Manager manager;
			OBLEGInput<OBLEGame> input{*this};
			OBLEGDebugText<OBLEGame> debugText{*this};

			std::map<OBLEGTileType, OBLEGTileData> tileMap
			{
				{Floor,		{Floor, assets.txSmall, assets.floor}},
				{Wall,		{Wall, assets.txSmall, assets.wallSingle}},
				{Grate,		{Grate, assets.txSmall, assets.floorGrate}},
				{Pit,		{Pit, assets.txSmall, assets.pit}},
				{Turret,	{Turret, assets.txSmall, assets.eTurret, {{"rot", 0}}}},
				{Spawner,	{Spawner, assets.txSmall, assets.pjCannonPlasma, {{"spawns", {}}}}}
			};

			OBLEGLevel level{320 / 10, 240 / 10 - 2, tileMap[OBLEGTileType::Floor]};
			OBLEGTile* currentTile{nullptr};
			int currentBrushIdx{0};

		public:
			inline OBLEGame(ssvs::GameWindow& mGameWindow, OBAssets& mAssets) : gameWindow(mGameWindow), assets(mAssets)
			{
				gameCamera.pan(-5, -5);

				gameState.onUpdate += [this](float mFT){ update(mFT); };
				gameState.onDraw += [this]{ draw(); };

				newGame();
			}

			inline void newGame()
			{
				manager.clear();
				level = {320 / 10, 240 / 10 - 2, tileMap[OBLEGTileType::Floor]};
			}

			inline const OBLEGTileData& getCurrentData() { return tileMap[static_cast<OBLEGTileType>(currentBrushIdx)]; }
			inline void grabTile()
			{
				const auto& tileVec((gameCamera.getMousePosition() + Vec2f(5, 5)) / 10.f);
				int tX(tileVec.x), tY(tileVec.y);
				if(level.isValid(tX, tY)) currentTile = &level.getTile(tX, tY);
				else currentTile = nullptr;
			}
			inline void highlight()
			{
				if(currentTile == nullptr) return;
				currentTile->getSprite().setColor(sf::Color::Red);
			}
			inline void paint()
			{
				if(currentTile == nullptr) return;
				currentTile->setData(getCurrentData());
			}
			inline void cycle(int mDir)
			{
				currentBrushIdx += mDir;
				currentBrushIdx = ssvu::getSIMod(currentBrushIdx, (int)tileMap.size());
			}
			inline void rotate(int mDeg)
			{
				if(currentTile == nullptr) return;
				currentTile->rotate(mDeg);
			}

			inline void update(float mFT)
			{
				level.update();

				grabTile();
				highlight();
				if(input.painting) paint();

				manager.update(mFT);
				debugText.update(mFT);
				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				manager.draw();
				level.draw(gameWindow);
				gameCamera.unapply();

				overlayCamera.apply<int>();

				sf::Sprite s{getCurrentData().sprite}; s.setPosition(5, 240 - 15); render(s);

				overlayCamera.unapply();

				debugText.draw();
			}

			inline void render(const sf::Drawable& mDrawable)									{ gameWindow.draw(mDrawable); }
			inline void render(const sf::Drawable& mDrawable, sf::RenderStates mRenderStates)	{ gameWindow.draw(mDrawable, mRenderStates); }

			inline ssvs::GameWindow& getGameWindow() noexcept		{ return gameWindow; }
			inline OBAssets& getAssets() noexcept					{ return assets; }
			inline ssvs::GameState& getGameState() noexcept			{ return gameState; }
			inline sses::Manager& getManager() noexcept				{ return manager; }
			inline const decltype(input)& getInput() const noexcept	{ return input; }
	};
}

#endif
