// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTEXT
#define SSVOB_GUI_CONTEXT

#include <map>
#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBAssets.h"

namespace ob
{
	namespace GUI
	{
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
				bool hovered{false}, busy{false}, focused{false}, unfocusOnUnhover{true};
				Vec2f mousePos, mousePosOld;
				bool mouseLDown{false}, mouseLDownOld{false}, mouseRDown{false}, mouseRDownOld{false};

				inline void del(Widget& mWidget) noexcept { widgets.del(mWidget); }
				inline void render(sf::View* mView, const sf::Drawable& mDrawable)
				{
					renderTexture.setView(mView != nullptr ? *mView : gameWindow.getRenderWindow().getView());
					renderTexture.draw(mDrawable);
				}
				inline void unFocusAll() { for(auto& w : children) w->setFocusedRecursive(false); }
				inline void bringToFront(Widget& mWidget) { ssvu::eraseRemove(children, &mWidget); children.insert(std::begin(children), &mWidget); }

				template<typename T, typename... TArgs> inline T& allocateWidget(TArgs&&... mArgs)
				{
					static_assert(ssvu::isBaseOf<Widget, T>(), "T must be derived from Widget");
					return widgets.create<T>(*this, std::forward<TArgs>(mArgs)...);
				}

				inline void updateMouse()
				{
					mouseLDownOld = mouseLDown;
					mouseRDownOld = mouseRDown;
					mousePosOld = mousePos;
					mouseLDown = gameWindow.isBtnPressed(ssvs::MBtn::Left);
					mouseRDown = gameWindow.isBtnPressed(ssvs::MBtn::Right);
					mousePos = gameWindow.getMousePosition();
				}

				inline void updateFocus()
				{
					// If the context is busy (dragging, resizing, editing...), do not change focus
					if(isBusy()) return;

					// If mouse is pressed outside of the context or context is unfocused, unfocus everything
					if((unfocusOnUnhover || mouseLDown || mouseRDown) && !hovered) { unFocusAll(); return; }

					// Find the topmost pressed child, if any
					Widget* found{nullptr};
					for(auto& c : children) if(c->isAnyChildPressed()) { found = c; bringToFront(*c); break; }
					if(found == nullptr) return;

					// Find the "deepest" pressed child in the hierarchy
					for(const auto& w : found->getAllRecursive()) if(w->isPressed() && w->depth > found->depth) found = w;

					// Unfocus everything but the widgets as deep as the deepest child
					unFocusAll(); found->setFocusedSameDepth(true);
				}

			public:
				ssvu::Delegate<void(const sf::Event&)> onAnyEvent;

				Context(OBAssets& mAssets, ssvs::GameWindow& mGameWindow) : assets(mAssets), gameWindow(mGameWindow)
				{
					renderTexture.create(gameWindow.getWidth(), gameWindow.getHeight());
					sprite.setTexture(renderTexture.getTexture());
				}

				template<typename T, typename... TArgs> inline T& create(TArgs&&... mArgs)
				{
					auto& result(allocateWidget<T>(std::forward<TArgs>(mArgs)...));
					children.insert(std::begin(children), &result); return result;
				}

				inline void update(float mFT)
				{
					updateMouse();

					ssvu::eraseRemoveIf(children, &ssvu::MemoryManager<Widget>::isDead<Widget*>);
					widgets.refresh();

					hovered = false;
					for(auto& w : widgets) { w->recalculateDepth(); w->checkHover(); w->checkPressed(); }

					updateFocus();

					busy = focused = false;
					for(auto& w : children) { w->updateRecursive(mFT); w->refreshDirtyRecursive(); }
					for(auto& w : widgets) { w->checkHover(); if(w->isFocused()) focused = true; }

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
				inline bool isBusy() const noexcept						{ return busy; }
				inline bool isFocused() const noexcept					{ return focused; }
				inline bool isInUse() const noexcept					{ return isFocused() || isHovered() || isBusy(); }
		};
	}
}

#endif
