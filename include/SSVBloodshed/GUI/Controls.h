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
					setOutlineThickness(2); setOutlineColor(sf::Color::Black); setLabel(mLabel);
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

				inline void setState(bool mValue) { green = 255.f; setLabel(mValue ? "x" : ""); }

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
				inline void setState(bool mValue) 			{ state = mValue; cbsbBox.setState(mValue); }

				inline const std::string& getLabel() const noexcept	{ return lblLabel.getString(); }
				inline bool getState() const noexcept 				{ return state; }
		};

		class Form;

		class WidgetStrip : public Widget
		{
			private:
				std::vector<Widget*> widgets;
				At alignFirst{At::Left}, alignNext{At::Right};

				inline void refreshPositions()
				{
					if(widgets.empty()) return;

					auto prev(widgets[0]);
					prev->attach(alignFirst, *this, alignFirst);

					for(auto itr(std::begin(widgets) + 1); itr != std::end(widgets); ++itr)
					{
						auto& w(*itr);

						if(w->isHidden() || w->isExcluded()) continue;
						w->attach(alignFirst, *prev, alignNext, getAtVec(alignNext, 2.f));
						prev = w;
					}
				}
				inline void refreshSize()
				{
					float xMin{std::numeric_limits<float>::max()}, xMax{std::numeric_limits<float>::min()};
					float yMin{std::numeric_limits<float>::max()}, yMax{std::numeric_limits<float>::min()};

					for(const auto& w : widgets)
					{
						xMin = std::min(xMin, w->getLeft());
						xMax = std::max(xMax, w->getRight());
						yMin = std::min(yMin, w->getTop());
						yMax = std::max(yMax, w->getBottom());
					}

					setSize(xMax - xMin, yMax - yMin);
				}

				inline void update(float) override { refreshPositions(); refreshSize(); }

			public:
				WidgetStrip(Context& mContext, At mAlignFirst, At mAlignNext) : Widget{mContext}, alignFirst{mAlignFirst}, alignNext{mAlignNext} { setFillColor(sf::Color::Transparent); }

				inline WidgetStrip& operator+=(Widget& mWidget) { widgets.push_back(&mWidget); return *this; }
				inline WidgetStrip& operator+=(const std::initializer_list<Widget*> mWidgets) { for(const auto& w : mWidgets) *this += *w; return *this; }
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
					if(!wsShutter.isFocused()) wsShutter.setExcluded(true);
				}
				inline void postUpdate() override { wsShutter.setWidth(wsThis.getWidth()); }

			public:
				ShutterList(Context& mContext) : Widget{mContext},
					wsThis(create<WidgetStrip>(At::Right, At::Left)),
					wsShutter(create<WidgetStrip>(At::Top, At::Bottom)),
					lblLabel(create<Label>("shutter")),
					btnOpen(create<Button>("v", Vec2f{8.f, 8.f}))
				{
					setFillColor(sf::Color::Transparent);

					wsThis.setFillColor(sf::Color::Red);
					wsThis.setOutlineColor(sf::Color::Black);
					wsThis.setOutlineThickness(2);

					wsThis += {&btnOpen, &lblLabel};

					wsShutter.setFillColor(colorFocused);
					wsShutter.setOutlineColor(sf::Color::Black);
					wsShutter.setOutlineThickness(2);
					wsShutter.setExcluded(true);
					wsShutter.depthOffset = 1;

					wsThis.attach(At::Right, *this, At::Right);
					wsShutter.attach(At::Top, wsThis, At::Bottom, Vec2f{0.f, 2.f});

					btnOpen.onUse += [this]{ wsShutter.setExcluded(!wsShutter.isExcluded()); if(!wsShutter.isExcluded()) wsShutter.gainExclusiveFocus(); };
				}

				inline WidgetStrip& getShutter() const noexcept { return wsShutter; }
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
				FormBar(Context& mContext) : Widget{mContext},
					wsBtns(create<WidgetStrip>(At::Right, At::Left)),
					btnClose(create<Button>("x", Vec2f{8.f, 8.f})),
					btnMinimize(create<Button>("_", Vec2f{8.f, 8.f})),
					btnCollapse(create<Button>("^", Vec2f{8.f, 8.f})),
					lblTitle(create<Label>("UNNAMED FORM"))
				{
					setFillColor(sf::Color::Black);

					wsBtns += {&btnClose, &btnMinimize, &btnCollapse};

					wsBtns.attach(At::Right, *this, At::Right, Vec2f{-2.f, 0.f});
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
					fbBar.getBtnMinimize().setExcluded(!mValue);
					fbResizer.setExcluded(!mValue);
				}
				inline void setTitle(std::string mTitle)		{ fbBar.setTitle(std::move(mTitle)); }
				inline const std::string& getTitle() noexcept	{ return fbBar.getTitle(); }
		};
	}
}

#endif
