// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_LEVELEDITOR_GAME
#define SSVOB_LEVELEDITOR_GAME

#include <initializer_list>
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
		enum class At{Left, Right, Top, Bottom, NW, NE, SW, SE, Center};

		class Context;
		class Widget;

		struct AABBShape : public sf::RectangleShape
		{
			using sf::RectangleShape::RectangleShape;

			AABBShape() = default;
			AABBShape(const Vec2f& mPosition, const Vec2f& mHalfSize) { AABBShape::setPosition(mPosition); AABBShape::setSize(mHalfSize * 2.f); }

			inline void setPosition(const Vec2f& mPosition)		{ sf::RectangleShape::setPosition(mPosition); }
			inline void setPosition(float mX, float mY)			{ sf::RectangleShape::setPosition(Vec2f{mX, mY}); }
			inline void setSize(const Vec2f& mSize)				{ sf::RectangleShape::setSize(mSize); sf::RectangleShape::setOrigin(getHalfSize()); }
			inline void setSize(float mX, float mY)				{ sf::RectangleShape::setSize(Vec2f{mX, mY}); sf::RectangleShape::setOrigin(getHalfSize()); }
			inline void setWidth(float mWidth) noexcept			{ setSize(mWidth, getHeight()); }
			inline void setHeight(float mHeight) noexcept		{ setSize(getWidth(), mHeight); }

			inline float getX() const noexcept					{ return sf::RectangleShape::getPosition().x; }
			inline float getY() const noexcept					{ return sf::RectangleShape::getPosition().y; }
			inline float getLeft() const noexcept				{ return sf::RectangleShape::getPosition().x - getHalfSize().x; }
			inline float getRight() const noexcept				{ return sf::RectangleShape::getPosition().x + getHalfSize().x; }
			inline float getTop() const noexcept				{ return sf::RectangleShape::getPosition().y - getHalfSize().y; }
			inline float getBottom() const noexcept				{ return sf::RectangleShape::getPosition().y + getHalfSize().y; }
			inline Vec2f getHalfSize() const noexcept			{ return sf::RectangleShape::getSize() / 2.f; }
			inline float getHalfWidth() const noexcept			{ return getHalfSize().x; }
			inline float getHalfHeight() const noexcept			{ return getHalfSize().y; }
			inline float getWidth() const noexcept				{ return sf::RectangleShape::getSize().x; }
			inline float getHeight() const noexcept				{ return sf::RectangleShape::getSize().y; }

			template<typename T> inline Vec2<T> getVertexNW() const noexcept { return Vec2<T>(getLeft(), getTop()); }
			template<typename T> inline Vec2<T> getVertexNE() const noexcept { return Vec2<T>(getRight(), getTop()); }

			template<typename T> inline Vec2<T> getVertexSW() const noexcept { return Vec2<T>(getLeft(), getBottom()); }
			template<typename T> inline Vec2<T> getVertexSE() const noexcept { return Vec2<T>(getRight(), getBottom()); }

			inline bool isOverlapping(const Vec2f& mPoint) const noexcept	{ return mPoint.x >= getLeft() && mPoint.x < getRight() && mPoint.y >= getTop() && mPoint.y < getBottom(); }
			inline bool contains(const Vec2f& mPoint) const noexcept		{ return isOverlapping(mPoint); }
		};

		class Widget : public AABBShape, public ssvu::MemoryManageable
		{
			friend class Context;

			protected:
				Context& context;
				void render(const sf::Drawable& mDrawable);
				void setBusy(bool mValue);

			private:
				ssvu::MemoryManager<Widget> children;
				int zOrder{0};

				// Status
				bool focused{false}, visible{true}, active{true};
				bool hovered{false}, pressed{false}, pressedPreviously{false};

				// Positioning
				Widget* neighbor{nullptr};
				At from{At::Center}, to{At::Center};
				Vec2f offsetTo;

				inline virtual void update(float) { }
				inline virtual void draw() { }

				inline void updateWithChildren(float mFT)	{ children.refresh(); update(mFT); updateNeighbor(); for(auto& w : children) w->updateWithChildren(mFT); }
				inline void drawWithChildren()				{ if(!visible) return; draw(); render(*this); for(auto& w : children) w->drawWithChildren(); }

				inline void updateNeighbor()
				{
					if(neighbor == nullptr) return;
					neighbor->updateNeighbor();

					auto getVecPos = [](At mAt, Widget& mWidget) -> Vec2f
					{
						switch(mAt)
						{
							case At::Left:		return {mWidget.getLeft(), mWidget.getY()};
							case At::Right:		return {mWidget.getRight(), mWidget.getY()};
							case At::Top:		return {mWidget.getX(), mWidget.getTop()};
							case At::Bottom:	return {mWidget.getX(), mWidget.getBottom()};
							case At::NW:		return mWidget.getVertexNW<float>();
							case At::NE:		return mWidget.getVertexNE<float>();
							case At::SW:		return mWidget.getVertexSW<float>();
							case At::SE:		return mWidget.getVertexSE<float>();
							case At::Center:	return mWidget.getPosition();
						}
						return mWidget.getPosition();
					};

					setPosition(getVecPos(to, *neighbor) + offsetTo + (this->getPosition() - getVecPos(from, *this)));
				}

				void checkUse();

			public:
				using AABBShape::AABBShape;

				Widget(Context& mContext) : context(mContext) { }
				Widget(Context& mContext, const Vec2f& mHalfSize) : AABBShape(Vec2f{0.f, 0.f}, mHalfSize), context(mContext) { }
				Widget(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize) : AABBShape(mPosition, mHalfSize), context(mContext) { }

				template<typename T, typename... TArgs> T& createChild(TArgs&&... mArgs);
				void destroy();

				inline void attach(At mFrom, Widget &mNeigh, At mTo, const Vec2f& mOffsetTo = Vec2f{0.f, 0.f}) { from = mFrom; neighbor = &mNeigh; to = mTo; offsetTo = mOffsetTo; }
				inline void show() { setVisible(true); setActive(true); }
				inline void hide() { setVisible(false); setActive(false); }

				inline void setFocused(bool mValue)	{ focused = mValue;	for(auto& w : children) w->setFocused(mValue); }
				inline void setVisible(bool mValue)	{ visible = mValue;	for(auto& w : children) w->setVisible(mValue); }
				inline void setActive(bool mValue)	{ active = mValue;	for(auto& w : children) w->setActive(mValue); }

				inline bool isFocused() const noexcept			{ return focused; }
				inline bool isHovered() const noexcept			{ return active && hovered; }
				inline bool isVisible() const noexcept			{ return visible; }
				inline bool isActive() const noexcept			{ return active; }
				inline bool isPressed() const noexcept			{ return isHovered() && pressed; }
				bool wasPressed() const noexcept;
				inline bool isAnyChildPressed() const noexcept	{ for(const auto& w : children) if(w->isPressed()) return true; return isPressed(); }
				Vec2f getMousePos() const noexcept;
				bool isMBtnLeftDown() const noexcept;
		};

		class Context
		{
			friend class Widget;

			private:
				OBAssets& assets;
				ssvs::GameWindow& gameWindow;
				sf::RenderTexture renderTexture;
				sf::Sprite sprite;
				ssvu::MemoryManager<Widget> widgets;
				std::vector<Widget*> sorted;
				bool hovered{false}, busy{false};
				Vec2f mousePos; bool mouseDown{false}, mousePreviouslyDown{false};

				inline void del(Widget& mWidget) { widgets.del(mWidget); }
				inline void render(const sf::Drawable& mDrawable) { renderTexture.draw(mDrawable); }
				inline void unFocusAll() { for(auto& w : widgets) w->setFocused(false); }
				inline void bringToFront(Widget& mWidget) { for(auto& w : widgets) w->zOrder = 0; mWidget.zOrder = -1; }

			public:
				Context(OBAssets& mAssets, ssvs::GameWindow& mGameWindow) : assets(mAssets), gameWindow(mGameWindow)
				{
					renderTexture.create(gameWindow.getWidth(), gameWindow.getHeight());
					sprite.setTexture(renderTexture.getTexture());
				}

				template<typename T, typename... TArgs> inline T& create(TArgs&&... mArgs)
				{
					static_assert(ssvu::isBaseOf<Widget, T>(), "T must be derived from Widget");
					return widgets.create<T>(*this, std::forward<TArgs>(mArgs)...);
				}

				inline void update(float mFT)
				{
					mousePreviouslyDown = mouseDown;
					mouseDown = gameWindow.isBtnPressed(ssvs::MBtn::Left);
					mousePos = gameWindow.getMousePosition();

					widgets.refresh();

					sorted.clear();
					for(auto& w : widgets) sorted.push_back(w.get());
					ssvu::sort(sorted, [](const Widget* mA, const Widget* mB){ return mA->zOrder < mB->zOrder; });

					hovered = false;
					for(auto& w : sorted) w->checkUse();

					if(!isBusy())
					{
						for(auto& w : sorted)
							if(w->isAnyChildPressed())
							{
								unFocusAll();
								w->setFocused(true);
								bringToFront(*w);
								break;
							}
					}

					busy = false;
					for(auto& w : sorted) w->updateWithChildren(mFT);

				}
				inline void draw()
				{
					renderTexture.clear(sf::Color::Transparent);
					for(auto itr(std::rbegin(sorted)); itr != std::rend(sorted); ++itr) (*itr)->drawWithChildren();
					renderTexture.display();

					sprite.setColor(sf::Color(255, 255, 255, isInUse() ? 255 : 175));
					gameWindow.draw(sprite);
				}

				inline OBAssets& getAssets() const noexcept				{ return assets; }
				inline ssvs::GameWindow& getGameWindow() const noexcept	{ return gameWindow; }
				inline bool isHovered() const noexcept					{ return hovered; }
				inline bool isBusy() const noexcept						{ return busy; }
				inline bool isInUse() const noexcept					{ return hovered || busy; }
		};

		inline void Widget::render(const sf::Drawable& mDrawable) { context.render(mDrawable); }
		inline void Widget::destroy() { ssvu::MemoryManageable::destroy(*this); for(const auto& c : children) c->destroy(); }
		inline void Widget::checkUse()
		{
			hovered = isOverlapping(getMousePos());
			if(hovered) context.hovered = true;
			pressedPreviously = pressed;
			pressed = isMBtnLeftDown() && hovered;
			for(const auto& c : children) c->checkUse();
		}
		inline Vec2f Widget::getMousePos() const noexcept		{ return context.mousePos; }
		inline bool Widget::isMBtnLeftDown() const noexcept		{ return active && context.mouseDown; }
		inline void Widget::setBusy(bool mValue)				{ context.busy = mValue; }
		inline bool Widget::wasPressed() const noexcept			{ return context.mousePreviouslyDown || (isHovered() && pressedPreviously); }

		template<typename T, typename... TArgs> inline T& Widget::createChild(TArgs&&... mArgs)
		{
			static_assert(ssvu::isBaseOf<Widget, T>(), "T must be derived from Widget");
			return children.create<T>(context, std::forward<TArgs>(mArgs)...);
		}

		class Label : public Widget
		{
			private:
				ssvs::BitmapText text;

				inline void draw() override { text.setPosition(getPosition() - Vec2f{0.f, 1.f}); render(text); }

			public:
				Label(Context& mContext, std::string mText = "") : Widget{mContext}, text{*context.getAssets().obStroked}
				{
					setFillColor(sf::Color::Transparent);
					text.setColor(sf::Color::White);
					text.setTracking(-3);
					setString(std::move(mText));
				}

				inline void setString(std::string mLabel)
				{
					text.setString(std::move(mLabel));
					text.setOrigin(ssvs::getGlobalHalfSize(text));
					setSize(ssvs::getGlobalSize(text));
				}
				inline const std::string& getString() const noexcept { return text.getString(); }
		};

		class Button : public Widget
		{
			private:
				bool used{false};
				Label& lblLabel;
				float green{0.f};

				inline void update(float mFT) override
				{
					setFillColor(sf::Color(255, green, 0, 255));
					green = ssvu::getClampedMin(green - mFT * 15.f, 0.f);

					if(!isFocused()) return;

					if(!wasPressed() && isPressed()) use();
					else if(!isMBtnLeftDown()) used = false;
				}

				inline void use()
				{
					setBusy(true);
					if(used) return;
					used = true; onUse(); green = 255;
				}

			public:
				ssvu::Delegate<void()> onUse;

				Button(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					lblLabel(createChild<Label>("button"))
				{
					setOutlineThickness(2);
					setOutlineColor(sf::Color::Black);
					setLabel(mLabel);
					lblLabel.attach(At::Center, *this, At::Center);
				}

				inline void setLabel(std::string mLabel)			{ lblLabel.setString(std::move(mLabel)); }
				inline const std::string& getLabel() const noexcept	{ return lblLabel.getString(); }
		};

		class FormBar : public Widget
		{
			private:
				Button& btnClose;
				Button& btnMinimize;
				Label& lblTitle;

			public:
				FormBar(Context& mContext) : Widget{mContext},
					btnClose(createChild<Button>("x", Vec2f{8.f, 8.f})),
					btnMinimize(createChild<Button>("_", Vec2f{8.f, 8.f})),
					lblTitle(createChild<Label>("UNNAMED FORM"))
				{
					setFillColor(sf::Color::Black);
					btnClose.setLabel("x");
					btnMinimize.setLabel("_");

					btnClose.attach(At::Right, *this, At::Right, Vec2f{-2.f, 0.f});
					btnMinimize.attach(At::Right, btnClose, At::Left, Vec2f{-2.f, 0.f});
					lblTitle.attach(At::NW, *this, At::NW, Vec2f{0.f, 2.f});
				}

				inline Button& getBtnClose() const noexcept		{ return btnClose; }
				inline Button& getBtnMinimize() const noexcept	{ return btnMinimize; }

				inline void setTitle(std::string mTitle)		{ lblTitle.setString(std::move(mTitle)); }
				inline const std::string& getTitle() noexcept	{ return lblTitle.getString(); }
		};

		class Form : public Widget
		{
			private:
				enum class Action{None, Move, Resize};

				FormBar& fbBar;
				Widget& fbResizer;
				bool draggable{true}, resizable{true};

				Action action;
				Vec2f dragPos;

				inline void update(float) override
				{
					fbBar.setSize(getSize().x + 4.f, 12.f);

					if(!isFocused())
					{
						setFillColor(sf::Color{165, 165, 165, 255});
						return;
					}

					setFillColor(sf::Color{190, 190, 190, 255});

					switch(action)
					{
						case Action::Move:
						{
							setBusy(true);
							setPosition(getMousePos() - (dragPos - getPosition()));
							break;
						}
						case Action::Resize:
						{
							setBusy(true);
							setWidth(ssvu::getClampedMin(getWidth(), 70.f));
							setHeight(ssvu::getClampedMin(getHeight(), 70.f));

							auto oldNW(getVertexNW<float>());
							setSize(getMousePos() - (dragPos - getSize()));
							setPosition(oldNW + getHalfSize());
							break;
						}
						default: break;
					}

					dragPos = getMousePos();

					if(draggable && !context.isBusy() && fbBar.isPressed()) action = Action::Move;
					else if(resizable && !context.isBusy() && fbResizer.isPressed()) action = Action::Resize;
					else if(!isMBtnLeftDown()) action = Action::None;
				}

			public:
				Form(Context& mContext, const Vec2f& mPosition, const Vec2f& mSize) : Widget{mContext, mPosition, mSize / 2.f},
					fbBar(createChild<FormBar>()), fbResizer(createChild<Widget>(Vec2f{4.f, 4.f}))
				{
					setOutlineThickness(2);
					setOutlineColor(sf::Color::Black);
					fbBar.getBtnClose().onUse += [this]{ hide(); };
					fbBar.getBtnMinimize().onUse += [this]{ setSize(70.f, 70.f); };
					fbResizer.setFillColor(sf::Color{140, 140, 140, 255});

					fbBar.attach(At::Bottom, *this, At::Top);
					fbResizer.attach(At::SE, *this, At::SE);
				}

				inline void setDraggable(bool mValue)		{ draggable = mValue; }
				inline void setResizable(bool mValue)		{ resizable = mValue; fbBar.getBtnMinimize().setActive(mValue); fbBar.getBtnMinimize().setVisible(mValue); fbResizer.setVisible(mValue); }
				inline void setTitle(std::string mTitle)	{ fbBar.setTitle(std::move(mTitle)); }
				inline std::string getTitle() noexcept		{ return fbBar.getTitle(); }
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
			std::pair<OBLETType, std::map<std::string, ssvuj::Obj>> copiedParams{OBLETType::LETFloor, {}};

			OBLETile copiedTile;

			sf::RectangleShape rsBrush{Vec2f{10.f * brush.size, 10.f * brush.size}}, rsBrushSingle{Vec2f{10.f, 10.f}};

			GUI::Context guiCtx;

			GUI::Form* formMenu{nullptr};

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

				formMenu = &guiCtx.create<GUI::Form>(Vec2f{400, 100}, Vec2f{64, 80});
				formMenu->setTitle("MENU");
				formMenu->setResizable(false);
				formMenu->show();

				auto& btnParams(formMenu->createChild<GUI::Button>("parameters", Vec2f{56, 8}));
				btnParams.onUse += [this]{ formParams->show(); };
				btnParams.attach(GUI::At::NW, *formMenu, GUI::At::NW, Vec2f{4.f, 4.f});

				auto& btnInfo(formMenu->createChild<GUI::Button>("info", Vec2f{56, 8}));
				btnInfo.onUse += [this]{ formInfo->show(); };
				btnInfo.attach(GUI::At::Top, btnParams, GUI::At::Bottom, Vec2f{0.f, 6.f});

				formParams = &guiCtx.create<GUI::Form>(Vec2f{100, 100}, Vec2f{150, 80});
				formParams->setTitle("PARAMETERS");
				lblParams = &formParams->createChild<GUI::Label>();
				lblParams->attach(GUI::At::NW, *formParams, GUI::At::NW, Vec2f{2.f, 2.f});

				formInfo = &guiCtx.create<GUI::Form>(Vec2f{100, 100}, Vec2f{150, 80});
				formInfo->setTitle("INFO");
				lblInfo = &formInfo->createChild<GUI::Label>();
				lblInfo->attach(GUI::At::NW, *formInfo, GUI::At::NW, Vec2f{2.f, 2.f});
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
				if(idx > 0 && (formParams->getWidth() < lblParams->getWidth() * 1.2f || formParams->getHeight() < lblParams->getHeight() * 1.2f)) formParams->setSize(lblParams->getSize() * 1.2f);
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
				if(formInfo->getWidth() < lblInfo->getWidth() * 1.2f || formInfo->getHeight() < lblInfo->getHeight() * 1.2f) formInfo->setSize(lblInfo->getSize() * 1.2f);

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
			inline const OBLEDatabaseEntry& getCurrentEntry() const	{ return database.get(OBLETType(brush.idx)); }
	};
}

#endif

