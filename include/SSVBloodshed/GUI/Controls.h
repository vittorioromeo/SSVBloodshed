// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS
#define SSVOB_GUI_CONTROLS

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/GUI/Widget.h"

namespace ob
{
	namespace GUI
	{
		const sf::Color colorFocused{190, 190, 190, 255};
		const sf::Color colorUnfocused{150, 150, 150, 255};

		class Label : public Widget
		{
			private:
				ssvs::BitmapText text;

				inline void draw() override { text.setPosition(getX(), getY() - 1.f); render(text); }

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
					lblLabel(create<Label>(std::move(mLabel)))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);
					lblLabel.attach(At::Center, *this, At::Center);
				}

				inline Label& getLabel() noexcept { return lblLabel; }
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

				inline void setState(bool mValue) { green = 255.f; lblState.setString(mValue ? "x" : ""); }
				inline Label& getLabel() noexcept { return lblState; }
		};

		class CheckBox : public Widget
		{
			private:
				CheckBoxStateBox& cbsbBox;
				Label& lblLabel;
				bool state{false};

				inline void update(float) override
				{
					float w{lblLabel.getRight() - cbsbBox.getLeft()};
					float h{lblLabel.getBottom() - cbsbBox.getTop()};
					setSize(w, h);

					if(isClickedOnce()) setState(!state);
				}

			public:
				CheckBox(Context& mContext, std::string mLabel, bool mState = false) : Widget{mContext},
					cbsbBox(create<CheckBoxStateBox>()), lblLabel(create<Label>(std::move(mLabel)))
				{
					setFillColor(sf::Color::Transparent); setState(mState);

					cbsbBox.attach(At::NW, *this, At::NW);
					lblLabel.attach(At::Left, cbsbBox, At::Right, Vec2f{2.f, 0.f});
				}

				inline void setState(bool mValue) 		{ state = mValue; cbsbBox.setState(mValue); }
				inline bool getState() const noexcept	{ return state; }
				inline Label& getLabel() noexcept		{ return lblLabel; }
		};

		class Form;

		class WidgetStrip : public Widget
		{
			private:
				std::vector<Widget*> widgets;
				At alignFirst{At::Left}, alignNext{At::Right};
				Vec2f padding;
				float widgetOffset{6.f};

				inline void update(float) override
				{
					if(widgets.empty()) return;

					auto prev(widgets[0]);
					prev->attach(alignFirst, *this, alignFirst, getAtVec(alignFirst, -ssvs::getMag(padding / 4.f)));

					float xMin{prev->getLeft()}, xMax{prev->getRight()};
					float yMin{prev->getTop()}, yMax{prev->getBottom()};

					for(auto itr(std::begin(widgets) + 1); itr != std::end(widgets); ++itr)
					{
						auto& w(*itr);
						if(w->isHidden() || w->isExcluded()) continue;

						w->attach(alignFirst, *prev, alignNext, getAtVec(alignNext, widgetOffset));

						xMin = std::min(xMin, w->getLeft());
						xMax = std::max(xMax, w->getRight());
						yMin = std::min(yMin, w->getTop());
						yMax = std::max(yMax, w->getBottom());

						prev = w;
					}

					setSize(xMax - xMin + padding.x, yMax - yMin + padding.y);
				}

			public:
				WidgetStrip(Context& mContext, At mAlignFirst, At mAlignNext) : Widget{mContext}, alignFirst{mAlignFirst}, alignNext{mAlignNext} { setFillColor(sf::Color::Transparent); }

				inline WidgetStrip& operator+=(Widget& mWidget) { widgets.push_back(&mWidget); mWidget.setParent(*this); return *this; }
				inline WidgetStrip& operator+=(const std::initializer_list<Widget*> mWidgets) { for(const auto& w : mWidgets) *this += *w; return *this; }

				inline void setPadding(const Vec2f& mValue) noexcept	{ padding = mValue; }
				inline void setWidgetOffset(float mValue) noexcept		{ widgetOffset = mValue; }
		};

		class ShutterList : public Widget
		{
			private:
				WidgetStrip& wsThis;
				WidgetStrip& wsShutter;
				Label& lblLabel;
				Button& btnOpen;

				inline void update(float) override
				{
					setSize(wsThis.getSize());
					wsShutter.setFillColor(wsShutter.isFocused() ? colorFocused : colorUnfocused);

					if(!wsShutter.isAnyChildFocused()) wsShutter.setExcludedRecursive(true);
				}
				inline void postUpdate() override { }

			public:
				ShutterList(Context& mContext, std::string mLabel) : Widget{mContext},
					wsThis(create<WidgetStrip>(At::Right, At::Left)),
					wsShutter(create<WidgetStrip>(At::Top, At::Bottom)),
					lblLabel(create<Label>(std::move(mLabel))),
					btnOpen(create<Button>("v", Vec2f{8.f, 8.f}))
				{
					setFillColor(sf::Color::Transparent);

					wsThis.setFillColor(sf::Color::Red);
					wsThis.setOutlineColor(sf::Color::Black);
					wsThis.setOutlineThickness(2);

					wsThis += {&btnOpen, &lblLabel};

					wsShutter.setOutlineColor(sf::Color::Black);
					wsShutter.setOutlineThickness(2);
					wsShutter.setExcludedRecursive(true);
					wsShutter.setContainer(true);
					wsShutter.setPadding(Vec2f{8.f, 8.f});

					wsThis.attach(At::Right, *this, At::Right);
					wsShutter.attach(At::Top, wsThis, At::Bottom, Vec2f{0.f, 2.f});

					btnOpen.onUse += [this]
					{
						wsShutter.setExcludedSameDepth(false);
						wsShutter.gainExclusiveFocus();
					};
				}

				inline ShutterList& operator+=(Widget& mWidget) { wsShutter += mWidget; return *this; }
				inline ShutterList& operator+=(const std::initializer_list<Widget*> mWidgets) { wsShutter += mWidgets; return *this; }

				inline WidgetStrip& getShutter() const noexcept { return wsShutter; }
				inline Label& getLabel() noexcept				{ return lblLabel; }
		};

		class FormBar : public Widget
		{
			private:
				WidgetStrip& wsBtns;
				Button& btnClose;
				Button& btnMinimize;
				Button& btnCollapse;
				Label& lblTitle;

			public:
				FormBar(Context& mContext, std::string mTitle) : Widget{mContext},
					wsBtns(create<WidgetStrip>(At::Right, At::Left)),
					btnClose(create<Button>("x", Vec2f{8.f, 8.f})),
					btnMinimize(create<Button>("_", Vec2f{8.f, 8.f})),
					btnCollapse(create<Button>("^", Vec2f{8.f, 8.f})),
					lblTitle(create<Label>(std::move(mTitle)))
				{
					setFillColor(sf::Color::Black);

					wsBtns += {&btnClose, &btnMinimize, &btnCollapse};
					wsBtns.setWidgetOffset(2.f);

					wsBtns.attach(At::Right, *this, At::Right, Vec2f{-2.f, 0.f});
					lblTitle.attach(At::NW, *this, At::NW, Vec2f{0.f, 2.f});
				}

				inline Button& getBtnClose() const noexcept		{ return btnClose; }
				inline Button& getBtnMinimize() const noexcept	{ return btnMinimize; }
				inline Button& getBtnCollapse() const noexcept	{ return btnCollapse; }
				inline Label& getTitle() noexcept				{ return lblTitle; }
		};

		class Form : public Widget
		{
			private:
				enum class Action{None, Move, Resize};

				FormBar& fbBar;
				Widget& fbResizer;
				bool draggable{true}, resizable{true}, collapsed{false};
				Action action;
				float minWidth{65.f}, minHeight{65.f}, previousHeight;

				inline void update(float) override
				{
					setFillColor(isFocused() ? colorFocused : colorUnfocused);

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
				Form(Context& mContext, std::string mTitle, const Vec2f& mPosition, const Vec2f& mSize) : Widget{mContext, mPosition, mSize / 2.f},
					fbBar(create<FormBar>(std::move(mTitle))), fbResizer(create<Widget>(Vec2f{4.f, 4.f}))
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
						fbBar.getBtnCollapse().getLabel().setString("v");
						previousHeight = getHeight();
						resizeFromBottom(0.f);
					}
					else
					{
						fbBar.getBtnCollapse().getLabel().setString("^");
						resizeFromBottom(previousHeight);
					}

					collapsed = !collapsed;
					for(const auto& w : children) if(w != &fbBar) w->setHiddenRecursive(collapsed);
				}

				inline void setDraggable(bool mValue) noexcept { draggable = mValue; }
				inline void setResizable(bool mValue)
				{
					resizable = mValue;
					fbBar.getBtnMinimize().setExcludedRecursive(!mValue);
					fbResizer.setExcludedRecursive(!mValue);
				}
				inline void setTitle(std::string mTitle)		{ fbBar.getTitle().setString(std::move(mTitle)); }
				inline const std::string& getTitle() noexcept	{ return fbBar.getTitle().getString(); }
				inline bool isCollapsed() const noexcept		{ return collapsed; }
		};
	}
}

#endif
