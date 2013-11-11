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
			inline void setX(float mX) noexcept					{ setPosition(mX, getY()); }
			inline void setY(float mY) noexcept					{ setPosition(getX(), mY); }

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

			inline void resizeFromLeft(float mWidth) 			{ setX(getX() - (mWidth - getWidth()) / 2.f); setWidth(mWidth); }
			inline void resizeFromRight(float mWidth) 			{ setX(getX() + (mWidth - getWidth()) / 2.f); setWidth(mWidth); }
			inline void resizeFromTop(float mHeight) 			{ setY(getY() - (mHeight - getHeight()) / 2.f); setHeight(mHeight); }
			inline void resizeFromBottom(float mHeight) 		{ setY(getY() + (mHeight - getHeight()) / 2.f); setHeight(mHeight); }
			inline void resizeFromNW(const Vec2f& mSize) 		{ resizeFrom(Vec2f{-1.f, -1.f}, mSize); }
			inline void resizeFromNE(const Vec2f& mSize) 		{ resizeFrom(Vec2f{1.f, -1.f}, mSize); }
			inline void resizeFromSW(const Vec2f& mSize) 		{ resizeFrom(Vec2f{-1.f, 1.f}, mSize); }
			inline void resizeFromSE(const Vec2f& mSize) 		{ resizeFrom(Vec2f{1.f, 1.f}, mSize); }

			inline void resizeFrom(const Vec2f& mOrigin, const Vec2f& mSize)
			{				
				assert(mOrigin.x >= 0.f && mOrigin.x <= 1.f);
				assert(mOrigin.y >= 0.f && mOrigin.y <= 1.f);

				setX(getX() - mOrigin.x * ((mSize.x - getWidth()) / 2.f));
				setY(getY() - mOrigin.y * ((mSize.y - getHeight()) / 2.f));
				setSize(mSize);
			}

			template<typename T = float> inline Vec2<T> getVertexNW() const noexcept { return Vec2<T>(getLeft(), getTop()); }
			template<typename T = float> inline Vec2<T> getVertexNE() const noexcept { return Vec2<T>(getRight(), getTop()); }
			template<typename T = float> inline Vec2<T> getVertexSW() const noexcept { return Vec2<T>(getLeft(), getBottom()); }
			template<typename T = float> inline Vec2<T> getVertexSE() const noexcept { return Vec2<T>(getRight(), getBottom()); }

			inline bool isOverlapping(const Vec2f& mPoint) const noexcept	{ return mPoint.x >= getLeft() && mPoint.x < getRight() && mPoint.y >= getTop() && mPoint.y < getBottom(); }
			inline bool contains(const Vec2f& mPoint) const noexcept		{ return isOverlapping(mPoint); }
		};

		class Widget : public AABBShape, public ssvu::MemoryManageable
		{
			friend class Context;

			protected:
				Context& context;
				std::vector<Widget*> children;
				void render(const sf::Drawable& mDrawable);

			private:
				int zOrder{0};

				// Status 
				bool hidden{false}; // Controlled by hide/show: if true, it makes the widget implicitly invisible and inactive
				bool focused{false}, visible{true}, active{true};
				bool hovered{false}, pressed{false}, pressedPreviously{false};

				// Positioning
				Widget* neighbor{nullptr};
				At from{At::Center}, to{At::Center};
				Vec2f offset;

				inline virtual void update(float) { }
				inline virtual void draw() { }

				void updateWithChildren(float mFT);
				inline void drawWithChildren() 
				{ 
					if(!isVisible()) return; 
					draw(); render(*this); 
					for(auto& w : children) w->drawWithChildren(); 
				}

				/*template<At TAt> struct VecPosHelper;
				template<> struct VecPosHelper<At::Left> 	{ inline static Vec2f get(Widget& mWidget) { return {mWidget.getLeft(), mWidget.getY()}; } };
				template<> struct VecPosHelper<At::Right> 	{ inline static Vec2f get(Widget& mWidget) { return {mWidget.getRight(), mWidget.getY()}; } };
				template<> struct VecPosHelper<At::Top> 	{ inline static Vec2f get(Widget& mWidget) { return {mWidget.getX(), mWidget.getTop()}; } };
				template<> struct VecPosHelper<At::Bottom> 	{ inline static Vec2f get(Widget& mWidget) { return {mWidget.getX(), mWidget.getBottom()}; } };
				template<> struct VecPosHelper<At::NW> 		{ inline static Vec2f get(Widget& mWidget) { return mWidget.getVertexNW<float>(); } };
				template<> struct VecPosHelper<At::NE> 		{ inline static Vec2f get(Widget& mWidget) { return mWidget.getVertexNE<float>(); } };
				template<> struct VecPosHelper<At::SW> 		{ inline static Vec2f get(Widget& mWidget) { return mWidget.getVertexSW<float>(); } };
				template<> struct VecPosHelper<At::SE> 		{ inline static Vec2f get(Widget& mWidget) { return mWidget.getVertexSE<float>(); } };
				template<> struct VecPosHelper<At::Center>	{ inline static Vec2f get(Widget& mWidget) { return mWidget.getPosition(); } };*/

				inline Vec2f getVecPos(At mAt, Widget& mWidget)
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
				}

				inline void updateNeighbor() { if(neighbor != nullptr) setPosition(getVecPos(to, *neighbor) + offset + (this->getPosition() - getVecPos(from, *this))); }
				
				void checkHover();
				inline void checkUse()
				{
					pressedPreviously = pressed;
					pressed = isMBtnLeftDown() && hovered;
					for(const auto& c : children) c->checkUse();
				}
				inline void setFocused(bool mValue)	{ focused = mValue;	for(auto& w : children) w->setFocused(mValue); }

			public:
				using AABBShape::AABBShape;

				Widget(Context& mContext) : context(mContext) { }
				Widget(Context& mContext, const Vec2f& mHalfSize) : AABBShape(Vec2f{0.f, 0.f}, mHalfSize), context(mContext) { }
				Widget(Context& mContext, const Vec2f& mPosition, const Vec2f& mHalfSize) : AABBShape(mPosition, mHalfSize), context(mContext) { }

				template<typename T, typename... TArgs> T& create(TArgs&&... mArgs);
				void destroy();

				inline void attach(At mFrom, Widget &mNeigh, At mTo, const Vec2f& mOffset = Vec2f{0.f, 0.f}) { from = mFrom; neighbor = &mNeigh; to = mTo; offset = mOffset; }
				inline void show() { setHidden(false); }
				inline void hide() { setHidden(true); }

				// An hidden widget is both invisible and inactive
				inline void setHidden(bool mValue)	{ hidden = mValue;	for(auto& w : children) w->setHidden(mValue); }
				
				// An invisible widget is only "graphically hidden"
				inline void setVisible(bool mValue)	{ visible = mValue;	for(auto& w : children) w->setVisible(mValue); }
				
				// A widget can be used only when active
				inline void setActive(bool mValue)	{ active = mValue;	for(auto& w : children) w->setActive(mValue); }
				
				inline bool isFocused() const noexcept	{ return focused; }
				inline bool isHovered() const noexcept	{ return isActive() && hovered; }
				inline bool isVisible() const noexcept	{ return !hidden && visible; }
				inline bool isActive() const noexcept	{ return !hidden && active; }
				inline bool isPressed() const noexcept	{ return isHovered() && pressed; }
				
				// Only these two should be used in widget code
				inline bool isClickedAlways() const noexcept 	{ return isFocused() && isPressed(); }
				inline bool isClickedOnce() const noexcept 		{ return isClickedAlways() && !wasPressed(); }
				
				inline bool isAnyChildPressed() const noexcept	{ for(const auto& w : children) if(w->isPressed()) return true; return isPressed(); }				
				bool wasPressed() const noexcept;
				const Vec2f& getMousePos() const noexcept;
				const Vec2f& getMousePosOld() const noexcept;
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
				std::vector<Widget*> children;
				bool hovered{false}, busy{false};
				Vec2f mousePos, mousePosOld; bool mouseDown{false}, mouseDownOld{false};

				inline void del(Widget& mWidget) 					{ widgets.del(mWidget); }
				inline void render(const sf::Drawable& mDrawable) 	{ renderTexture.draw(mDrawable); }
				inline void unFocusAll() 							{ for(auto& w : children) w->setFocused(false); }
				inline void bringToFront(Widget& mWidget) 			
				{ 
					//std::swap(&mWidget, children.front());
					for(auto& w : children) w->zOrder = 0; 
					mWidget.zOrder = -1; 
					ssvu::sort(children, [](const Widget* mA, const Widget* mB){ return mA->zOrder < mB->zOrder; });
				}

				template<typename T, typename... TArgs> inline T& allocateWidget(TArgs&&... mArgs)
				{
					static_assert(ssvu::isBaseOf<Widget, T>(), "T must be derived from Widget");
					return widgets.create<T>(*this, std::forward<TArgs>(mArgs)...);
				}

				inline void updateMouse()
				{
					mouseDownOld = mouseDown;
					mouseDown = gameWindow.isBtnPressed(ssvs::MBtn::Left);
					mousePosOld = mousePos;
					mousePos = gameWindow.getMousePosition();
				}

				inline void updateFocus()
				{
					if(isBusy()) return;
					
					for(auto& w : children)
						if(w->isAnyChildPressed())
						{
							unFocusAll();
							w->setFocused(true);
							bringToFront(*w);
							return;
						}					
				}

			public:
				Context(OBAssets& mAssets, ssvs::GameWindow& mGameWindow) : assets(mAssets), gameWindow(mGameWindow)
				{
					renderTexture.create(gameWindow.getWidth(), gameWindow.getHeight());
					sprite.setTexture(renderTexture.getTexture());
				}

				template<typename T, typename... TArgs> inline T& create(TArgs&&... mArgs)
				{
					auto& result(allocateWidget<T>(std::forward<TArgs>(mArgs)...));
					children.push_back(&result); return result;
				}			

				inline void update(float mFT)
				{
					updateMouse();

					ssvu::eraseRemoveIf(children, [](const Widget* mW){ return ssvu::MemoryManager<Widget>::isDead(mW); }); 
					widgets.refresh();

					hovered = false;
					for(auto& w : children) { w->checkHover(); w->checkUse(); }

					updateFocus();

					busy = false;
					for(auto& w : children) { w->updateWithChildren(mFT); w->checkHover(); }

				}
				inline void draw()
				{
					renderTexture.clear(sf::Color::Transparent);
					for(auto itr(std::rbegin(children)); itr != std::rend(children); ++itr) (*itr)->drawWithChildren();
					renderTexture.display();

					sprite.setColor(sf::Color(255, 255, 255, isInUse() ? 255 : 175));
					gameWindow.draw(sprite);
				}

				inline OBAssets& getAssets() const noexcept				{ return assets; }
				inline ssvs::GameWindow& getGameWindow() const noexcept	{ return gameWindow; }
				inline bool isHovered() const noexcept					{ return hovered; }
				inline bool isBusy() const noexcept						{ return busy; }
				inline bool isInUse() const noexcept					{ return isHovered() || isBusy(); }
		};

		inline void Widget::render(const sf::Drawable& mDrawable) { context.render(mDrawable); }
		inline void Widget::destroy() { context.del(*this); for(const auto& c : children) c->destroy(); }
		inline void Widget::checkHover() 
		{
			hovered = isOverlapping(getMousePos());
			if(hovered) context.hovered = true;
			for(auto& w : children) w->checkHover();
		}
		
		inline void Widget::updateWithChildren(float mFT)	
		{ 
			ssvu::eraseRemoveIf(children, [](const Widget* mW){ return ssvu::MemoryManager<Widget>::isDead(mW); }); 
			update(mFT); updateNeighbor(); 
			if(isPressed()) context.busy = true;
			for(auto& w : children) w->updateWithChildren(mFT); 
		}
		inline const Vec2f& Widget::getMousePos() const noexcept	{ return context.mousePos; }
		inline const Vec2f& Widget::getMousePosOld() const noexcept	{ return context.mousePosOld; }
		inline bool Widget::isMBtnLeftDown() const noexcept			{ return isActive() && context.mouseDown; }
		inline bool Widget::wasPressed() const noexcept				{ return context.mouseDownOld || (isHovered() && pressedPreviously); }

		template<typename T, typename... TArgs> inline T& Widget::create(TArgs&&... mArgs)
		{
			auto& result(context.allocateWidget<T>(std::forward<TArgs>(mArgs)...));
			children.push_back(&result); return result;
		}

		class Label : public Widget
		{
			private:
				ssvs::BitmapText text;

				inline void draw() override { text.setPosition(getPosition() - Vec2f{0.f, 1.f}); render(text); }

			public:
				Label(Context& mContext, std::string mText = "") : Widget{mContext}, text{*context.getAssets().obStroked}
				{
					text.setColor(sf::Color::White); text.setTracking(-3);
					setFillColor(sf::Color::Transparent);
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
				Label& lblLabel;
				float green{0.f};

				inline void update(float mFT) override
				{
					setFillColor(sf::Color(255, green, 0, 255));
					green = ssvu::getClampedMin(green - mFT * 15.f, 0.f);
					if(isClickedOnce()) { onUse(); green = 255; }
				}

			public:
				ssvu::Delegate<void()> onUse;

				Button(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					lblLabel(create<Label>("button"))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);
					setLabel(mLabel);
					lblLabel.attach(At::Center, *this, At::Center);
				}

				inline void setLabel(std::string mLabel)			{ lblLabel.setString(std::move(mLabel)); }
				inline const std::string& getLabel() const noexcept	{ return lblLabel.getString(); }
		};

		class CheckBoxStateBox : public Widget
		{
			private:				
				Label& lblState;			
				float green{0.f};

				inline void update(float mFT) override
				{
					setFillColor(sf::Color(255, green, 0, 255));
					green = ssvu::getClampedMin(green - mFT * 15.f, 0.f);
				}

			public:				
				CheckBoxStateBox(Context& mContext) : Widget{mContext, Vec2f{4.f, 4.f}}, lblState(create<Label>(""))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);
					lblState.attach(At::Center, *this, At::Center);				
				}

				inline void use() { green = 255.f; }

				inline void setLabel(std::string mLabel) 			{ lblState.setString(std::move(mLabel)); }
				inline const std::string& getLabel() const noexcept	{ return lblState.getString(); }
		};

		class CheckBox : public Widget
		{
			private:				
				CheckBoxStateBox& cbsbBox;
				Label& lblLabel;
				bool state{false}, mustRefresh{false};

				inline void update(float) override
				{
					if(mustRefresh)
					{
						float w{lblLabel.getRight() - cbsbBox.getLeft()};
						float h{lblLabel.getBottom() - cbsbBox.getTop()};
						setSize(w, h); 
					}

					if(isClickedOnce()) setState(!state);					
				}

			public:				
				CheckBox(Context& mContext, std::string mLabel, bool mState = false) : Widget{mContext},
					cbsbBox(create<CheckBoxStateBox>()), lblLabel(create<Label>("checkbox"))
				{
					setFillColor(sf::Color::Transparent); setState(mState); setLabel(mLabel);

					cbsbBox.attach(At::NW, *this, At::NW);
					lblLabel.attach(At::Left, cbsbBox, At::Right, Vec2f{2.f, 0.f});
				}

				inline void setLabel(std::string mLabel) 	{ lblLabel.setString(std::move(mLabel)); mustRefresh = true; }
				inline void setState(bool mValue) 			{ state = mValue; cbsbBox.setLabel(state ? "x" : ""); cbsbBox.use(); }

				inline const std::string& getLabel() const noexcept	{ return lblLabel.getString(); }
				inline bool getState() const noexcept 				{ return state; }
		};

		class Form;

		class FormBar : public Widget
		{
			private:
				Button& btnClose;
				Button& btnMinimize;
				Button& btnCollapse;
				Label& lblTitle;

			public:
				FormBar(Context& mContext) : Widget{mContext},
					btnClose(create<Button>("x", Vec2f{8.f, 8.f})),
					btnMinimize(create<Button>("_", Vec2f{8.f, 8.f})),
					btnCollapse(create<Button>("^", Vec2f{8.f, 8.f})),
					lblTitle(create<Label>("UNNAMED FORM"))
				{
					setFillColor(sf::Color::Black);

					btnClose.attach(At::Right, *this, At::Right, Vec2f{-2.f, 0.f});
					btnMinimize.attach(At::Right, btnClose, At::Left, Vec2f{-2.f, 0.f});
					btnCollapse.attach(At::Right, btnMinimize, At::Left, Vec2f{-2.f, 0.f});
					lblTitle.attach(At::NW, *this, At::NW, Vec2f{0.f, 2.f});
				}

				inline void setTitle(std::string mTitle)			{ lblTitle.setString(std::move(mTitle)); }

				inline Button& getBtnClose() const noexcept			{ return btnClose; }
				inline Button& getBtnMinimize() const noexcept		{ return btnMinimize; }
				inline Button& getBtnCollapse() const noexcept		{ return btnCollapse; }
				inline const std::string& getTitle() const noexcept	{ return lblTitle.getString(); }
		};

		class Form : public Widget
		{
			private:
				enum class Action{None, Move, Resize};

				const sf::Color colorFocused{190, 190, 190, 255};
				const sf::Color colorUnfocused{150, 150, 150, 255};

				FormBar& fbBar;
				Widget& fbResizer;
				bool draggable{true}, resizable{true}, collapsed{false};
				Action action;
				float minWidth{65.f}, minHeight{65.f}, previousHeight;

				inline void update(float) override
				{					
					if(!isFocused()) { setFillColor(colorUnfocused); return; }

					setFillColor(colorFocused);					

					if(action == Action::Move)
					{
						setPosition(getMousePos() - (getMousePosOld() - getPosition()));
					}
					else if(action == Action::Resize)
					{
						setSize(ssvu::getClampedMin(getWidth(), minWidth), ssvu::getClampedMin(getHeight(), minHeight));

						auto oldNW(getVertexNW<float>());
						setSize(getMousePos() - (getMousePosOld() - getSize()));
						setPosition(oldNW + getHalfSize());
					}

					if(draggable && fbBar.isClickedAlways()) action = Action::Move;
					else if(resizable && fbResizer.isClickedAlways()) action = Action::Resize;
					else if(!isMBtnLeftDown()) action = Action::None;
				}
				inline void draw() override { fbBar.setSize(getSize().x + 4.f, 12.f); }

			public:
				Form(Context& mContext, const Vec2f& mPosition, const Vec2f& mSize) : Widget{mContext, mPosition, mSize / 2.f},
					fbBar(create<FormBar>()), fbResizer(create<Widget>(Vec2f{4.f, 4.f}))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);
					
					fbBar.getBtnClose().onUse += [this]{ hide(); };
					fbBar.getBtnMinimize().onUse += [this]{ /* TODO */ };
					fbBar.getBtnCollapse().onUse += [this]{ toggleCollapsed(); };

					fbResizer.setFillColor(sf::Color::Transparent);
					fbResizer.setOutlineThickness(2); fbResizer.setOutlineColor(sf::Color::Black);

					fbBar.attach(At::Bottom, *this, At::Top);
					fbResizer.attach(At::SE, *this, At::SE);
				}

				inline void toggleCollapsed()
				{
					if(!collapsed)
					{
						previousHeight = getHeight();
						resizeFromBottom(0.f);
					}
					else resizeFromBottom(previousHeight);

					collapsed = !collapsed;
					for(const auto& w : children) if(w != &fbBar) w->setHidden(collapsed);	
				}

				inline void setDraggable(bool mValue) noexcept { draggable = mValue; }
				inline void setResizable(bool mValue)		
				{ 
					resizable = mValue; 
					fbBar.getBtnMinimize().setActive(mValue); 
					fbBar.getBtnMinimize().setVisible(mValue); 
					fbResizer.setVisible(mValue); 
				}
				inline void setTitle(std::string mTitle)		{ fbBar.setTitle(std::move(mTitle)); }
				inline const std::string& getTitle() noexcept	{ return fbBar.getTitle(); }
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

				formMenu = &guiCtx.create<GUI::Form>(Vec2f{400, 100}, Vec2f{64, 80});
				formMenu->setTitle("MENU");
				formMenu->setResizable(false);
				formMenu->show();

				auto& btnParams(formMenu->create<GUI::Button>("parameters", Vec2f{56, 8}));
				btnParams.onUse += [this]{ formParams->show(); };
				btnParams.attach(GUI::At::NW, *formMenu, GUI::At::NW, Vec2f{4.f, 4.f});

				auto& btnInfo(formMenu->create<GUI::Button>("info", Vec2f{56, 8}));
				btnInfo.onUse += [this]{ formInfo->show(); };
				btnInfo.attach(GUI::At::Top, btnParams, GUI::At::Bottom, Vec2f{0.f, 6.f});

				chbShowId = &formMenu->create<GUI::CheckBox>("show id", true);
				chbShowId->attach(GUI::At::NW, btnInfo, GUI::At::SW, Vec2f{0.f, 6.f});

				chbOnion = &formMenu->create<GUI::CheckBox>("onion", true);
				chbOnion->attach(GUI::At::NW, *chbShowId, GUI::At::SW, Vec2f{0.f, 6.f});
				
				formParams = &guiCtx.create<GUI::Form>(Vec2f{100, 100}, Vec2f{150, 80});
				formParams->setTitle("PARAMETERS");
				lblParams = &formParams->create<GUI::Label>();
				lblParams->attach(GUI::At::NW, *formParams, GUI::At::NW, Vec2f{2.f, 2.f});

				formInfo = &guiCtx.create<GUI::Form>(Vec2f{100, 100}, Vec2f{150, 80});
				formInfo->setTitle("INFO");
				lblInfo = &formInfo->create<GUI::Label>();
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
			inline const OBLEDatabaseEntry& getCurrentEntry() const	{ return database.get(OBLETType(brush.idx)); }
	};
}

#endif
