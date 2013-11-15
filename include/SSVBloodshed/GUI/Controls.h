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
					// TODO: dirty here
					text.setString(std::move(mLabel));
					text.setOrigin(ssvs::getGlobalHalfSize(text));
					setSize(ssvs::getGlobalSize(text));
				}
				inline const std::string& getString() const noexcept { return text.getString(); }
				inline decltype(text)& getText() noexcept { return text; }
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

				inline void refreshIfDirty() override
				{
					float w{lblLabel.getRight() - cbsbBox.getLeft()};
					float h{lblLabel.getBottom() - cbsbBox.getTop()};
					setSize(w, h);
				}
				inline void update(float) override { if(isClickedOnce()) setState(!state); }

			public:
				ssvu::Delegate<void()> onStateChanged;

				CheckBox(Context& mContext, std::string mLabel, bool mState = false) : Widget{mContext},
					cbsbBox(create<CheckBoxStateBox>()), lblLabel(create<Label>(std::move(mLabel)))
				{
					setFillColor(sf::Color::Transparent); setState(mState);

					cbsbBox.attach(At::NW, *this, At::NW);
					lblLabel.attach(At::Left, cbsbBox, At::Right, Vec2f{2.f, 0.f});
				}

				inline void setState(bool mValue) 		{ state = mValue; cbsbBox.setState(mValue); onStateChanged(); }
				inline bool getState() const noexcept	{ return state; }
				inline Label& getLabel() noexcept		{ return lblLabel; }
		};

		class Form;

		class WidgetStrip : public Widget
		{
			private:
				std::vector<Widget*> widgets;
				At alignFrom{At::Left}, alignTo{At::Right}, alignDir{At::Right};
				bool tabbed{false};
				float widgetOffset{6.f}, tabSize;

				inline void refreshIfDirty() override
				{
					if(widgets.empty()) return;

					auto prev(widgets[0]);
					prev->attach(alignFrom, *this, alignFrom, getAtVec(alignDir, getPadding()));

					for(auto i(1u); i < widgets.size(); ++i)
					{
						auto& w(*widgets[i]);
						if(w.isHidden() || w.isExcluded()) continue;

						if(tabbed) w.attach(alignFrom, *prev, alignFrom, getAtVec(alignDir, i++ * tabSize));
						else
						{
							w.attach(alignFrom, *prev, alignTo, getAtVec(alignDir, widgetOffset));
							prev = &w;
						}
					}
				}

			public:
				WidgetStrip(Context& mContext, At mAlignFrom, At mAlignTo, At mAlignDir)
					: Widget{mContext}, alignFrom{mAlignFrom}, alignTo{mAlignTo}, alignDir{mAlignDir}
				{
					setFillColor(sf::Color::Transparent);
					setScaling(Scaling::FitToChildren);
				}

				inline WidgetStrip& operator+=(Widget& mWidget) { widgets.push_back(&mWidget); mWidget.setParent(*this); return *this; }
				inline WidgetStrip& operator+=(const std::initializer_list<Widget*> mWidgets) { for(const auto& w : mWidgets) *this += *w; return *this; }

				inline void setWidgetOffset(float mValue) noexcept { widgetOffset = mValue; }
				inline void setTabSize(float mValue) noexcept { tabSize = mValue; tabbed = true; }
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
					if(!wsShutter.isAnyChildFocused()) wsShutter.setExcludedRecursive(true);
				}
				inline void refreshIfDirty() override
				{
					wsShutter.setFillColor(wsShutter.isFocused() ? colorFocused : colorUnfocused);
				}

			public:
				ShutterList(Context& mContext, std::string mLabel) : Widget{mContext},
					wsThis(create<WidgetStrip>(At::Right, At::Left, At::Left)),
					wsShutter(create<WidgetStrip>(At::Top, At::Bottom, At::Bottom)),
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
					wsShutter.setPadding(4.f);

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

		class TextBox : public Widget
		{
			private:
				Widget& tBox;
				Label& lblText;
				bool editing{false};
				std::string editStr, str;
				float green{0.f}, greenDir{1.f};

				inline void update(float mFT) override
				{
					if(lblText.isClickedOnce())
					{
						editing = true; editStr = str;
						lblText.gainExclusiveFocus();
					}

					if(editing)
					{
						if(!lblText.isFocused()) finishEditing();

						// TODO: pingpongvalue<t>
						green = green + mFT * greenDir * 15.f;
						if(green <= 0) { green = 0; greenDir = 1.f; }
						else if(green >= 255) { green = 255; greenDir = -1.f; }

						lblText.setString(editStr);
						lblText.getText().setColor(sf::Color(255, green, 255, 255));
					}
					else
					{
						lblText.setString(str);
						lblText.getText().setColor(sf::Color::White);
					}
				}

				inline void finishEditing() { editing = false; str = editStr; onTextChanged(); }

			public:
				ssvu::Delegate<void()> onTextChanged;

				TextBox(Context& mContext, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					tBox(create<Widget>()), lblText(create<Label>(""))
				{
					context.onAnyEvent += [this](const sf::Event& mEvent)
					{
						if(!editing) return;

						if(mEvent.type == sf::Event::TextEntered)
						{
							if(mEvent.text.unicode == '\b' && !editStr.empty()) editStr.erase(std::end(editStr) - 1, std::end(editStr));
							else if(mEvent.text.unicode < 128) editStr += static_cast<char>(mEvent.text.unicode);
						}
						else if(mEvent.type == sf::Event::KeyPressed)
						{
							if(mEvent.key.code == ssvs::KKey::Return) finishEditing();
						}
					};

					setContainer(true);
					setFillColor(sf::Color::Transparent);

					tBox.setOutlineColor(sf::Color::Black);
					tBox.setOutlineThickness(2);
					tBox.setFillColor(sf::Color::White);
					tBox.setScaling(Widget::Scaling::FitToNeighbor);

					lblText.setScaling(Widget::Scaling::FitToNeighbor);

					tBox.attach(At::Center, *this, At::Center);
					lblText.attach(At::Center, tBox, At::Center);
				}

				inline void setString(std::string mString) { str = std::move(mString); }
				inline const std::string& getString() const noexcept { return str; }
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
					wsBtns(create<WidgetStrip>(At::Right, At::Left, At::Left)),
					btnClose(create<Button>("x", Vec2f{8.f, 8.f})),
					btnMinimize(create<Button>("_", Vec2f{8.f, 8.f})),
					btnCollapse(create<Button>("^", Vec2f{8.f, 8.f})),
					lblTitle(create<Label>(std::move(mTitle)))
				{
					external = true;
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
				float minWidth{45.f}, minHeight{45.f}, oldHeight;
				Scaling oldScalingX, oldScalingY;

				inline void update(float) override
				{
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
				inline void refreshIfDirty() override
				{
					setFillColor(isFocused() ? colorFocused : colorUnfocused);
				}

			public:
				Form(Context& mContext, std::string mTitle, const Vec2f& mPosition, const Vec2f& mSize) : Widget{mContext, mPosition, mSize / 2.f},
					fbBar(create<FormBar>(std::move(mTitle))), fbResizer(create<Widget>(Vec2f{4.f, 4.f}))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);

					fbBar.getBtnClose().onUse += [this]{ hide(); };
					fbBar.getBtnMinimize().onUse += [this]{ /* TODO */ };
					fbBar.getBtnCollapse().onUse += [this]{ toggleCollapsed(); };
					fbBar.setScalingX(Scaling::FitToNeighbor);
					fbBar.setHeight(12.f); fbBar.setPadding(-2.f);

					fbResizer.setFillColor(sf::Color::Transparent);
					fbResizer.setOutlineThickness(2); fbResizer.setOutlineColor(sf::Color::Black);

					fbBar.attach(At::Bottom, *this, At::Top);
					fbResizer.attach(At::SE, *this, At::SE);
				}

				inline void toggleCollapsed()
				{
					if(!collapsed)
					{
						oldHeight = getHeight();
						oldScalingX = getScalingX();
						oldScalingY = getScalingY();
						setScaling(Scaling::Manual);
						fbBar.getBtnCollapse().getLabel().setString("v");
						resizeFromBottom(0.f);
					}
					else
					{
						setScalingX(oldScalingX); setScalingY(oldScalingY);
						fbBar.getBtnCollapse().getLabel().setString("^");
						resizeFromBottom(oldHeight);
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
