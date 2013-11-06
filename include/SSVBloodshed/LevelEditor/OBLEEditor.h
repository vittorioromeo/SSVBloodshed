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
	namespace GUI
	{
		class Context;
		class WidgetBase;

		class Form;
		class Label;
		class Button;

		class AABB
		{
			protected:
				Vec2f position, halfSize;

			public:
				AABB(const Vec2f& mPosition, const Vec2f& mHalfSize) : position{mPosition}, halfSize{mHalfSize} { }

				inline void move(const Vec2f& mOffset) noexcept				{ position += mOffset; }

				inline void setPosition(const Vec2f& mPosition) noexcept	{ position = mPosition; }
				inline void setX(float mX) noexcept							{ position.x = mX; }
				inline void setY(float mY) noexcept							{ position.y = mY; }
				inline void setHalfSize(const Vec2f& mHalfSize) noexcept	{ halfSize = mHalfSize; }
				inline void setSize(const Vec2f& mSize) noexcept			{ halfSize = mSize / 2.f; }
				inline void setWidth(float mWidth) noexcept					{ halfSize.x = mWidth / 2.f; }
				inline void setHeight(float mHeight) noexcept				{ halfSize.y = mHeight / 2.f; }

				inline const Vec2f& getPosition() const noexcept	{ return position; }
				inline float getX() const noexcept					{ return position.x; }
				inline float getY() const noexcept					{ return position.y; }
				inline float getLeft() const noexcept				{ return position.x - halfSize.x; }
				inline float getRight() const noexcept				{ return position.x + halfSize.x; }
				inline float getTop() const noexcept				{ return position.y - halfSize.y; }
				inline float getBottom() const noexcept				{ return position.y + halfSize.y; }
				inline const Vec2f& getHalfSize() const noexcept	{ return halfSize; }
				inline float getHalfWidth() const noexcept			{ return halfSize.x; }
				inline float getHalfHeight() const noexcept			{ return halfSize.y; }
				inline Vec2f getSize() const noexcept				{ return halfSize * 2.f; }
				inline float getWidth() const noexcept				{ return halfSize.x * 2.f; }
				inline float getHeight() const noexcept				{ return halfSize.y * 2.f; }

				template<typename T> inline Vec2<T> getVertexNW() const noexcept { return Vec2<T>(getLeft(), getTop()); }
				template<typename T> inline Vec2<T> getVertexNE() const noexcept { return Vec2<T>(getRight(), getTop()); }
				template<typename T> inline Vec2<T> getVertexSW() const noexcept { return Vec2<T>(getLeft(), getBottom()); }
				template<typename T> inline Vec2<T> getVertexSE() const noexcept { return Vec2<T>(getRight(), getBottom()); }

				inline bool isOverlapping(const Vec2f& mPoint) const noexcept	{ return mPoint.x >= getLeft() && mPoint.x < getRight() && mPoint.y >= getTop() && mPoint.y < getBottom(); }
				inline bool contains(const Vec2f& mPoint) const noexcept		{ return isOverlapping(mPoint); }
		};

		struct AABBShape : public sf::RectangleShape, public AABB
		{
			using AABB::AABB;
			using sf::RectangleShape::RectangleShape;

			AABBShape() : AABB{{0.f, 0.f}, {1.f, 1.f}} { }

			inline void setPosition(const Vec2f& mPosition) { AABB::setPosition(mPosition); sf::RectangleShape::setPosition(mPosition); }
			inline void setSize(const Vec2f& mSize)
			{
				AABB::setSize(mSize);
				sf::RectangleShape::setSize(mSize);
				sf::RectangleShape::setOrigin(AABB::halfSize);
			}
			inline void setOrigin(const Vec2f& mOrigin) = delete;

			inline const Vec2f& getPosition() const noexcept { return AABB::getPosition(); }
		};

		class WidgetBase : public AABB
		{
			protected:
				Context& context;

			public:
				using AABB::AABB;

				WidgetBase(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize)
					: AABB{mPosition, mHalfSize}, context(mContext) { }

				inline virtual void update(float) { }
				inline virtual void draw() { }

				Vec2f getMousePos() const noexcept;
				bool isMBtnLeftDown() const noexcept;
		};

		class Context
		{
			private:
				OBAssets& assets;
				ssvs::GameWindow& gameWindow;
				std::vector<Uptr<WidgetBase>> widgets;

			public:
				Context(OBAssets& mAssets, ssvs::GameWindow& mGameWindow) : assets(mAssets), gameWindow(mGameWindow) { }

				inline void update(float mFT)	{ for(auto itr(std::rbegin(widgets)); itr != std::rend(widgets); ++itr) (*itr)->update(mFT); }// for(auto& w : widgets) w->update(mFT); }
				inline void draw()				{ for(auto itr(std::rbegin(widgets)); itr != std::rend(widgets); ++itr) (*itr)->draw(); }// for(auto& w : widgets) w->draw(); }

				inline void render(const sf::Drawable& mDrawable) { gameWindow.draw(mDrawable); }

				inline OBAssets& getAssets() const noexcept { return assets; }
				inline ssvs::GameWindow& getGameWindow() const noexcept { return gameWindow; }

				template<typename T, typename... TArgs> inline T& create(TArgs&&... mArgs)
				{
					// TODO: shortcut for is_base_of
					static_assert(std::is_base_of<WidgetBase, T>::value, "T must be derived from WidgetBase");

					auto result(new T(*this, std::forward<TArgs>(mArgs)...));
					widgets.emplace_back(result);
					return *result;
				}
		};

		inline Vec2f WidgetBase::getMousePos() const noexcept { return context.getGameWindow().getMousePosition(); }
		inline bool WidgetBase::isMBtnLeftDown() const noexcept { return context.getGameWindow().isBtnPressed(ssvs::MBtn::Left); }

		class Button : public WidgetBase
		{
			private:
				bool used{false};
				ssvs::BitmapText label;
				AABBShape body;

				inline void refreshPosition()	{ body.setPosition(position); label.setPosition(position - Vec2f{0.f, 1.f}); }
				inline void refreshSize()		{ body.setSize(getSize()); }

			public:
				ssvu::Delegate<void()> onUse;

				Button(Context& mContext, const Vec2f& mPosition, const Vec2f& mSize) : WidgetBase{mContext, mPosition, mSize / 2.f},
					label{*context.getAssets().obStroked}
				{
					body.setFillColor(sf::Color::Red);
					body.setOutlineThickness(2);
					body.setOutlineColor(sf::Color::Black);
					label.setColor(sf::Color::White);
					label.setString("x");
					label.setTracking(-3);
					label.setOrigin(ssvs::getGlobalSize(label) / 2.f);
				}

				inline void update(float) override
				{
					refreshSize(); refreshPosition();
					if(body.isOverlapping(getMousePos()) && isMBtnLeftDown())
					{
						if(!used) { used = true; onUse(); }
					}
					else if(used && !isMBtnLeftDown()) used = false;
				}
				inline void draw() override { context.render(body); context.render(label); }

				inline void setLabel(std::string mLabel) { label.setString(std::move(mLabel)); }
				inline std::string getLabel() noexcept { return label.getString(); }
		};

		class Form : public WidgetBase
		{
			private:
				Button& btnClose;
				ssvs::BitmapText title;
				AABBShape body, bar;

				bool dragging{false};
				Vec2f dragPos;

				inline void refreshPosition()
				{
					body.setPosition(position);
					bar.setPosition({position.x, ssvs::getGlobalTop(body) + bar.getHalfSize().y});
					title.setPosition(bar.getVertexNW<float>() + Vec2f{0.f, 1.f});
					btnClose.setPosition({bar.getRight() - 4.f, bar.getY()});
				}

				inline void refreshSize()
				{
					body.setSize(getSize());
					bar.setSize({getSize().x, 12.f});
					btnClose.setSize({8.f, 8.f});
				}

			public:
				Form(Context& mContext, const Vec2f& mPosition, const Vec2f& mSize) : WidgetBase{mContext, mPosition, mSize / 2.f},
					btnClose(context.create<Button>(Vec2f{0.f, 0.f}, Vec2f{8.f, 8.f})),
					title{*context.getAssets().obStroked}
				{
					body.setFillColor(sf::Color{190, 190, 190, 255});
					body.setOutlineThickness(2);
					body.setOutlineColor(sf::Color::Black);
					bar.setFillColor(sf::Color::Black);
					title.setColor(sf::Color::White);
					title.setString("form 1");
					title.setTracking(-3);
					btnClose.setLabel("x");
				}

				inline void update(float) override
				{
					if(dragging) { setPosition(getMousePos() - (dragPos - getPosition())); }
					dragPos = getMousePos();
					if(bar.isOverlapping(getMousePos()) && isMBtnLeftDown())
					{
						dragging = true;
					}
					else if(!isMBtnLeftDown()) dragging = false;

					refreshSize();
					refreshPosition();
				}
				inline void draw() override
				{
					context.render(body);
					context.render(bar);
					context.render(title);
				}

				inline void setTitle(std::string mTitle) { title.setString(std::move(mTitle)); }

				inline std::string getTitle() noexcept { return title.getString(); }
		};
	}

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
			ssvs::BitmapText paramsText{*assets.obStroked};
			std::pair<OBLETType, std::map<std::string, ssvuj::Obj>> copiedParams{OBLETType::LETFloor, {}};

			OBLETile copiedTile;

			sf::RectangleShape rsBrush{Vec2f{10.f * brush.size, 10.f * brush.size}}, rsBrushSingle{Vec2f{10.f, 10.f}};

			GUI::Context guiCtx;

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

				paramsText.setTracking(-3);
				paramsText.setPosition(55, 240 - 15);

				newSector();

				guiCtx.create<GUI::Form>(Vec2f{100, 100}, Vec2f{150, 80});
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

			inline void paint()	{ for(auto& t : currentTiles) { t->initFromEntry(getCurrentEntry()); t->setRot(currentRot); t->setId(assets, currentId); } }
			inline void del()	{ for(auto& t : currentTiles) { currentLevel->del(*t); } }
			inline void pick()	{ brush.idx = int(getPickTile().getType()); }

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
				std::string str;

				int idx{0};
				for(const auto& p : t.getParams())
				{
					bool cp{ssvu::getWrapIdx(currentParamIdx, static_cast<int>(t.getParams().size())) == idx++};
					const auto& temp(p.first + "(" + ssvu::getReplacedAll(ssvu::toStr(p.second), "\n", "") + ")");
					str += cp ? " >> " + ssvu::toUpper(temp) + " <<  " : temp + " ";
				}

				paramsText.setString(str);
				paramsText.setScale(t.getParams().size() < 3 ? Vec2f(1.f, 1.f) : Vec2f(0.5f, 0.5f));
			}

			inline void update(float mFT)
			{
				guiCtx.update(mFT);

				currentLevel->update();
				updateXY(); grabTiles(); updateParamsText();

				if(input.painting) paint();
				else if(input.deleting) del();

				debugText.update(mFT);
				gameCamera.update<int>(mFT);
			}
			inline void draw()
			{
				gameCamera.apply<int>();
				{
					currentLevel->draw(gameWindow, true, currentZ);
					rsBrush.setSize({brush.size * 10.f, brush.size * 10.f});
					rsBrush.setPosition(brush.left * 10.f, brush.top * 10.f);
					rsBrushSingle.setPosition(brush.x * 10.f, brush.y * 10.f);
					render(rsBrush); render(rsBrushSingle);
				}
				gameCamera.unapply();

				overlayCamera.apply<int>();
				{
					for(int i{-1}; i < (getPickTile().getParams().empty() ? 25 : 3); ++i)
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

					render(paramsText);

				}
				overlayCamera.unapply();

				guiCtx.draw();
				debugText.draw();
			}

			template<typename... TArgs> inline void render(const sf::Drawable& mDrawable, TArgs&&... mArgs)	{ gameWindow.draw(mDrawable, std::forward<TArgs>(mArgs)...); }

			inline void setGame(OBGame& mGame)						{ game = &mGame; }

			inline ssvs::GameWindow& getGameWindow() noexcept		{ return gameWindow; }
			inline OBAssets& getAssets() noexcept					{ return assets; }
			inline ssvs::GameState& getGameState() noexcept			{ return gameState; }
			inline const decltype(input)& getInput() const noexcept	{ return input; }
			inline const OBLEDatabaseEntry& getCurrentEntry() const	{ return database.get(OBLETType(brush.idx)); }
	};
}

#endif

