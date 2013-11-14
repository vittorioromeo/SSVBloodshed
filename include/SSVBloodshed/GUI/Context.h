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
				bool hovered{false}, busy{false}, focused{false};
				Vec2f mousePos, mousePosOld; bool mouseDown{false}, mouseDownOld{false};

				inline void del(Widget& mWidget) noexcept			{ widgets.del(mWidget); }
				inline void render(const sf::Drawable& mDrawable)	{ renderTexture.draw(mDrawable); }
				inline void unFocusAll()							{ for(auto& w : children) w->setFocusedRecursive(false); }
				inline void bringToFront(Widget& mWidget)			{ for(auto& w : children) if(w == &mWidget) { std::swap(w, children[0]); return; } }

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

					if(mouseDown && !ssvu::containsAnyIf(widgets, [](const ssvu::Uptr<Widget>& mW){ return mW->isHovered(); }))
					{
						unFocusAll();
						return;
					}

					Widget* found{nullptr};

					for(auto& c : children)
					{
						if(!c->isAnyChildPressed()) continue;

						found = c;
						bringToFront(*c);
						break;
					}

					if(found == nullptr) return;
					for(auto& c : children) if(found != c) c->setFocusedRecursive(false);

					const auto& hierarchy(found->getAllRecursive());
					int maxPressedDepth{-1};
					Widget* deepest{nullptr};

					for(const auto& w : hierarchy)
					{
						if(w->isPressed() && w->depth > maxPressedDepth)
						{
							maxPressedDepth = w->depth;
							deepest = w;
						}
					}

					if(maxPressedDepth == -1) return;

					for(const auto& w : hierarchy) if(w->depth != maxPressedDepth) { w->dirty = true; w->focused = false; }
					deepest->setFocusedSameDepth(true);
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
					children.push_back(&result); return result;
				}

				inline void update(float mFT)
				{
					updateMouse();

					ssvu::eraseRemoveIf(children, [](const Widget* mW){ return ssvu::MemoryManager<Widget>::isDead(mW); });
					widgets.refresh();

					hovered = false;
					for(auto& w : widgets) { w->recalculateDepth(); w->checkHover(); w->checkPressed(); }

					updateFocus();

					busy = focused = false;
					for(auto& w : children) w->updateRecursive(mFT);
					for(auto& w : children) w->refreshDirtyRecursive();
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
