// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTEXT
#define SSVOB_GUI_CONTEXT

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
				bool hovered{false}, busy{false};
				Vec2f mousePos, mousePosOld; bool mouseDown{false}, mouseDownOld{false};

				inline void del(Widget& mWidget) noexcept			{ widgets.del(mWidget); }
				inline void render(const sf::Drawable& mDrawable)	{ renderTexture.draw(mDrawable); }
				inline void unFocusAll()							{ for(auto& w : children) w->setFocused(false); }
				inline void bringToFront(Widget& mWidget)			{ for(auto& w : children) if(w == &mWidget) std::swap(w, children[0]); }

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

				inline void recalculateDepth()
				{
					for(auto& w : children) w->recalculateDepth();
				}

				inline void updateFocus()
				{
					if(isBusy()) return;

					for(auto& w : children)
					{
						bool gainFocus{false};

						// If any child is pressed and has exclusive focus, go to next container widget
						if(w->isAnyChildExclusiveFocusPressed()) continue;

						// If the container widget or any child is pressed, gain focus
						if(w->isPressed()) gainFocus = true;
						else for(auto& c : w->children) if(c->isPressed()) { gainFocus = true; break; }

						if(!gainFocus) continue;

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
					recalculateDepth();

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
	}
}

#endif
