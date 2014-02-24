// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTEXT
#define SSVOB_GUI_CONTEXT

#include <map>
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBAssets.hpp"

namespace ob
{
	namespace GUI
	{
		struct Style
		{
			ssvs::BitmapFont& font;
			sf::Color colorOutline{0, 0, 0, 255};
			sf::Color colorBaseFocused{190, 190, 190, 255};
			sf::Color colorBaseUnfocused{150, 150, 150, 255};
			sf::Color colorBtnUnpressed{255, 0, 0, 255};
			sf::Color colorText{255, 255, 255, 255};
			sf::Color colorLabelBG{0, 0, 0, 0};
			float outlineThickness{2.f}, padding{2.f};

			inline Style(ssvs::BitmapFont& mFont) : font(mFont) { }

			inline float getGlyphWidth() const noexcept							{ return font.getCellWidth(); }
			inline float getGlyphHeight() const noexcept						{ return font.getCellHeight(); }
			inline Vec2f getBtnSquareSize() const noexcept						{ return Vec2f{getGlyphHeight() - 2.f, getGlyphHeight() - 2.f}; }
			inline Vec2f getBtnSize(float mWidth) const noexcept				{ return Vec2f{mWidth, getGlyphHeight() - 2.f}; }
			inline float getWidthPerChar(std::size_t mCount) const noexcept		{ return getGlyphWidth() * mCount; }
			inline Vec2f getBtnSizePerChar(std::size_t mCount) const noexcept	{ return Vec2f{getWidthPerChar(mCount), getGlyphHeight() - 2.f}; }
		};

		class Context
		{
			friend class Widget;

			private:
				OBAssets& assets;
				ssvs::GameWindow& gameWindow;
				Style style;
				sf::RenderTexture renderTexture;
				sf::Sprite sprite;
				ssvu::MemoryManager<Widget> widgets;
				std::vector<Widget*> children;
				Widget* busyWith{nullptr};
				bool hovered{false}, focused{false}, unfocusOnUnhover{true};
				Vec2f mousePos; bool mouseLDown{false}, mouseRDown{false};
				std::vector<sf::Event> eventsToPoll;

				inline void del(Widget& mWidget) const noexcept { widgets.del(mWidget); }
				inline void render(sf::View* mView, const sf::Drawable& mDrawable)
				{
					renderTexture.setView(mView != nullptr ? *mView : gameWindow.getRenderWindow().getView());
					renderTexture.draw(mDrawable);
				}
				inline void unFocusAll() { focused = false; for(auto& w : widgets) w->setFocused(false); }
				inline void bringToFront(Widget& mWidget) { ssvu::eraseRemove(children, &mWidget); children.insert(std::begin(children), &mWidget); }

				template<typename T, typename... TArgs> inline T& allocateWidget(TArgs&&... mArgs)
				{
					SSVU_ASSERT_STATIC(ssvu::isBaseOf<Widget, T>(), "T must be derived from Widget");
					return widgets.create<T>(*this, std::forward<TArgs>(mArgs)...);
				}

				inline void updateFocus()
				{
					// If the context is busy (dragging, resizing, editing...), do not change focus
					if(isBusy()) return;

					// If mouse is pressed outside of the context or context is unfocused, unfocus everything
					if((unfocusOnUnhover || mouseLDown || mouseRDown) && !hovered) { unFocusAll(); return; }

					// Find the topmost pressed child, if any
					Widget* found{nullptr};
					for(auto& c : children) if(c->isAnyChildRecursive([](const Widget& mW){ return mW.isPressedAny(); })) { found = c; bringToFront(*c); break; }
					if(found == nullptr) return;

					// Find the "deepest" pressed child in the hierarchy
					found->recurseChildren([&found](Widget& mW){ if(mW.isPressedAny() && mW.depth > found->depth) found = &mW; });

					// Unfocus everything but the widgets as deep as the deepest child, and focus the context
					for(auto& w : children) w->recurseChildrenIf<true>([found](Widget& mW){ return mW.isFocused() && &mW != found; }, [](Widget& mW){ mW.setFocused(false); });
					found->recurseChildren([this, found](Widget& mW){ if(mW.depth == found->depth) { mW.setFocused(true); focused = true; } });
				}

				inline bool isKeyPressed(ssvs::KKey mKey) const noexcept { return gameWindow.isKeyPressed(mKey); }

			public:
				Context(OBAssets& mAssets, ssvs::GameWindow& mGameWindow, Style mStyle) : assets(mAssets), gameWindow(mGameWindow),
					style{std::move(mStyle)}
				{
					renderTexture.create(gameWindow.getWidth(), gameWindow.getHeight());
					sprite.setTexture(renderTexture.getTexture());
				}

				template<typename T, typename... TArgs> inline T& create(TArgs&&... mArgs)
				{
					auto& result(allocateWidget<T>(std::forward<TArgs>(mArgs)...));
					children.insert(std::begin(children), &result); return result;
				}

				inline void onAnyEvent(const sf::Event& mEvent) { eventsToPoll.emplace_back(mEvent); }

				inline void update(FT mFT)
				{
					// Set "old" mouse variable and get mouse position/status
					mouseLDown = gameWindow.isBtnPressed(ssvs::MBtn::Left);
					mouseRDown = gameWindow.isBtnPressed(ssvs::MBtn::Right);
					mousePos = gameWindow.getMousePosition();

					// Recursively remove all dead widgets from children and set "not recalculated"
					for(auto& w : children) w->recurseChildren([](Widget& mW)
					{
						mW.recalculated.x = mW.recalculated.y = false;
						ssvu::eraseRemoveIf(mW.children, &ssvu::MemoryManager<Widget>::isDead<Widget*>);
					});
					ssvu::eraseRemoveIf(children, &ssvu::MemoryManager<Widget>::isDead<Widget*>);

					// If mouse buttons are not down or the `busyWith` widget is dead, stop being busy
					if((!mouseLDown && !mouseRDown) || (busyWith != nullptr && ssvu::MemoryManager<Widget>::isDead<Widget*>(busyWith)))
						busyWith = nullptr;

					// Free dead widgets memory / initialize new widgets
					widgets.refresh();

					// Focus the correct widgets. If any is focused, set `focused` to true.
					updateFocus();

					// Set `hovered` to false: if any widget is hovered, it will become true
					hovered = false;

					// Recursively update all widgets
					for(auto& w : children) w->updateRecursive(mFT);

					eventsToPoll.clear();
				}
				inline void draw()
				{
					renderTexture.clear(sf::Color::Transparent);
					for(auto itr(std::rbegin(children)); itr != std::rend(children); ++itr) (*itr)->drawHierarchy();
					renderTexture.display();

					sprite.setColor(sf::Color(255, 255, 255, isInUse() ? 255 : 175));
					gameWindow.draw(sprite);
				}

				inline OBAssets& getAssets() const noexcept				{ return assets; }
				inline ssvs::GameWindow& getGameWindow() const noexcept	{ return gameWindow; }
				inline bool isHovered() const noexcept					{ return hovered; }
				inline bool isBusy() const noexcept						{ return busyWith != nullptr; }
				inline bool isFocused() const noexcept					{ return focused; }
				inline bool isInUse() const noexcept					{ return isFocused() || isHovered() || isBusy(); }
				inline const Style& getStyle() const noexcept			{ return style; }
		};
	}
}

#endif
