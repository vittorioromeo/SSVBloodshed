// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS
#define SSVOB_GUI_CONTROLS

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"

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
				bool scaleWithText{true};

				inline void draw() override { text.setPosition(getX(), getY() - 1.f); render(text); }

			public:
				Label(Context& mContext, std::string mText = "") : Widget{mContext}, text{*context.getAssets().obStroked}
				{
					text.setColor(sf::Color::White); text.setTracking(-3);
					setFillColor(sf::Color::Transparent);
					setString(std::move(mText));
				}

				inline void setScaleWithText(bool mValue) noexcept { scaleWithText = mValue; }
				inline void setString(std::string mLabel)
				{
					text.setString(std::move(mLabel));
					text.setOrigin(ssvs::getGlobalHalfSize(text));
					if(scaleWithText) setSize(ssvs::getGlobalSize(text));
				}
				inline const std::string& getString() const noexcept { return text.getString(); }
				inline decltype(text)& getText() noexcept { return text; }
		};

		class Button : public Widget
		{
			private:
				Label& lblLabel;
				float green{0.f};

				inline void update(FT mFT) override
				{
					setFillColor(sf::Color(255, green, 0, 255));
					green = ssvu::getClampedMin(green - mFT * 15.f, 0.f);
				}

			public:
				Button(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					lblLabel(create<Label>(std::move(mLabel)))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);
					lblLabel.attach(At::Center, *this, At::Center);
					onLeftClick += [this]{ green = 255; };
				}

				inline Label& getLabel() noexcept { return lblLabel; }
		};

		namespace Internal
		{
			class CheckBoxStateBox : public Widget
			{
				private:
					Label& lblState;
					float green{0.f};

					inline void update(FT mFT) override
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
		}

		class CheckBox : public Widget
		{
			private:
				Internal::CheckBoxStateBox& cbsbBox;
				Label& lblLabel;
				bool state{false};

			public:
				ssvu::Delegate<void()> onStateChanged;

				CheckBox(Context& mContext, std::string mLabel, bool mState = false) : Widget{mContext},
					cbsbBox(create<Internal::CheckBoxStateBox>()), lblLabel(create<Label>(std::move(mLabel)))
				{
					setScaling(Scaling::FitToChildren);
					setFillColor(sf::Color::Transparent); setState(mState);

					cbsbBox.attach(At::NW, *this, At::NW);
					lblLabel.attach(At::Left, cbsbBox, At::Right, Vec2f{2.f, 0.f});

					onLeftClick += [this]{ setState(!state); };
				}

				inline void setState(bool mValue) 		{ state = mValue; cbsbBox.setState(mValue); onStateChanged(); }
				inline bool getState() const noexcept	{ return state; }
				inline Label& getLabel() noexcept		{ return lblLabel; }
		};

		class Form;

		class Strip : public Widget
		{
			private:
				At alignFrom{At::Left}, alignTo{At::Right}, alignDir{At::Right};
				bool tabbed{false};
				float widgetOffset{6.f}, tabSize;

				inline void update(FT) override
				{
					auto& widgets(getChildren());
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
				Strip(Context& mContext, At mAlignFrom, At mAlignTo, At mAlignDir)
					: Widget{mContext}, alignFrom{mAlignFrom}, alignTo{mAlignTo}, alignDir{mAlignDir}
				{
					setFillColor(sf::Color::Transparent);
					setScaling(Scaling::FitToChildren);
				}

				inline Strip& operator+=(Widget& mWidget) { mWidget.setParent(*this); return *this; }
				inline Strip& operator+=(const std::initializer_list<Widget*> mWidgets) { for(const auto& w : mWidgets) *this += *w; return *this; }

				inline void setWidgetOffset(float mValue) noexcept { widgetOffset = mValue; }
				inline void setTabSize(float mValue) noexcept { tabSize = mValue; tabbed = true; }
		};

		class Shutter : public Widget
		{
			private:
				Strip& wsBar;
				Strip& wsShutter;
				Button& btnOpen;
				Label& lblLabel;

				inline void update(FT) override
				{
					setSize(wsBar.getSize());
					if(!wsShutter.isAnyChildRecursive([](const Widget& mW){ return mW.isFocused(); })) wsShutter.setExcludedRecursive(true);
					wsShutter.setFillColor(wsShutter.isFocused() ? colorFocused : colorUnfocused);
				}

			public:
				Shutter(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext},
					wsBar(create<Strip>(At::Right, At::Left, At::Left)),
					wsShutter(create<Strip>(At::Top, At::Bottom, At::Bottom)),
					btnOpen(wsBar.create<Button>("v", Vec2f{8.f, 8.f})),
					lblLabel(wsBar.create<Label>(std::move(mLabel)))
				{
					setFillColor(sf::Color::Transparent);

					wsBar.setFillColor(sf::Color::Red);
					wsBar.setOutlineColor(sf::Color::Black);
					wsBar.setOutlineThickness(2);
					wsBar.setScaling(Scaling::Manual);
					wsBar.setSize(mSize);

					wsShutter.setOutlineColor(sf::Color::Black);
					wsShutter.setOutlineThickness(2);
					wsShutter.setExcludedRecursive(true);
					wsShutter.setContainer(true);
					wsShutter.setPadding(4.f);

					wsBar.attach(At::Right, *this, At::Right);
					wsShutter.attach(At::Top, wsBar, At::Bottom, Vec2f{0.f, 2.f});

					btnOpen.onLeftClick += [this]
					{
						wsShutter.setExcludedSameDepth(false);
						wsShutter.gainExclusiveFocus();
					};
				}

				inline Shutter& operator+=(Widget& mWidget) { wsShutter += mWidget; return *this; }
				inline Shutter& operator+=(const std::initializer_list<Widget*> mWidgets) { wsShutter += mWidgets; return *this; }

				inline Strip& getBar() const noexcept		{ return wsBar; }
				inline Strip& getShutter() const noexcept	{ return wsShutter; }
				inline Label& getLabel() noexcept			{ return lblLabel; }
		};

		class ChoiceShutter : public Shutter
		{
			private:
				std::vector<std::string> choices;
				int idxOffset{0};

				std::size_t currentChoiceIdx{0}, choiceBtnsMax{5};
				Strip& wsChoices;
				Strip& wsScroll;
				Button& btnUp;
				Button& btnDown;
				Label& lblCount;
				std::vector<Button*> btnsChoices;

				inline void refreshChoices()
				{
					if(choices.empty()) { getLabel().setString("null"); return; }

					const auto& lb(ssvu::getWrapIdx(idxOffset, choices.size()));
					const auto& ub(ssvu::getWrapIdx(idxOffset + choiceBtnsMax, choices.size()));
					lblCount.setString("(" + ssvu::toStr(lb) + ":" + ssvu::toStr(ub) + ")/" + ssvu::toStr(choices.size()));

					for(auto i(0u); i < choiceBtnsMax; ++i)
					{
						const auto& choiceIdx(ssvu::getWrapIdx(i + idxOffset, choices.size()));
						btnsChoices[i]->getLabel().setString(choiceIdx == currentChoiceIdx ? ">" + ssvu::toUpper(choices[choiceIdx]) + "<" : choices[choiceIdx]);
					}

					getLabel().setString(choices[currentChoiceIdx]);
				}

			public:
				ssvu::Delegate<void()> onChoiceSelected;

				ChoiceShutter(Context& mContext, const std::vector<std::string>& mChoices, const Vec2f& mSize)
						: Shutter{mContext, "", mSize}, choices{mChoices},
						wsChoices(getShutter().create<Strip>(At::Top, At::Bottom, At::Bottom)),
						wsScroll(getShutter().create<Strip>(At::Right, At::Left, At::Left)),
						btnUp(wsScroll.create<Button>("^", Vec2f{8.f, 8.f})),
						btnDown(wsScroll.create<Button>("v", Vec2f{8.f, 8.f})),
						lblCount(wsScroll.create<Label>(""))
				{
					btnUp.onLeftClick += [this]{ --idxOffset; refreshChoices(); };
					btnDown.onLeftClick += [this]{ ++idxOffset; refreshChoices(); };

					for(auto i(0u); i < choiceBtnsMax; ++i)
					{
						auto& btn(wsChoices.create<Button>("", Vec2f{56.f, 8.f}));
						btn.onLeftClick += [this, i]
						{
							if(choices.empty()) return;
							setChoiceIdx(ssvu::getWrapIdx(i + idxOffset, choices.size())); onChoiceSelected();
						};
						btnsChoices.push_back(&btn);
					}

					refreshChoices();
				}

				inline void addChoice(std::string mStr)	{ choices.emplace_back(std::move(mStr)); refreshChoices(); }
				inline void clearChoices()				{ choices.clear(); refreshChoices(); }
				inline std::size_t getChoiceCount()		{ return choices.size(); }

				inline void setChoiceIdx(std::size_t mIdx)				{ currentChoiceIdx = mIdx; refreshChoices(); }
				inline int getChoiceIdx() const noexcept				{ return currentChoiceIdx; }
				inline const std::string& getChoice() const noexcept	{ return choices[currentChoiceIdx]; }
		};

		class TextBox : public Widget
		{
			private:
				Widget& tBox;
				Label& lblText;
				bool editing{false};
				std::string editStr, str;
				float green{0.f}, greenDir{1.f};

				inline void update(FT mFT) override
				{
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

					for(const auto& e : getEventsToPoll())
					{
						if(!editing) return;

						if(e.type == sf::Event::TextEntered)
						{
							if(e.text.unicode == '\b' && !editStr.empty()) editStr.erase(std::end(editStr) - 1, std::end(editStr));
							else if(e.text.unicode > 31 && e.text.unicode < 127) editStr += static_cast<char>(e.text.unicode);
						}
						else if(e.type == sf::Event::KeyPressed)
						{
							if(e.key.code == ssvs::KKey::Return) finishEditing();
						}
					}
				}

				inline void finishEditing() { editing = false; str = editStr; onTextChanged(); }

			public:
				ssvu::Delegate<void()> onTextChanged;

				TextBox(Context& mContext, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					tBox(create<Widget>()), lblText(tBox.create<Label>(""))
				{
					setFillColor(sf::Color::Transparent);

					tBox.setOutlineColor(sf::Color::Black);
					tBox.setOutlineThickness(2);
					tBox.setFillColor(sf::Color::White);
					tBox.setScaling(Scaling::FitToNeighbor);

					lblText.setScaling(Scaling::FitToNeighbor);

					tBox.attach(At::Center, *this, At::Center);
					lblText.attach(At::Center, tBox, At::Center);

					onLeftClick += [this]{ editing = true; editStr = str; lblText.gainExclusiveFocus(); };
				}

				inline void setString(std::string mString) { str = std::move(mString); }
				inline const std::string& getString() const noexcept { return str; }
		};

		class FormBar : public Widget
		{
			private:
				Strip& wsBtns;
				Button& btnClose;
				Button& btnMinimize;
				Button& btnCollapse;
				Label& lblTitle;

			public:
				FormBar(Context& mContext, std::string mTitle) : Widget{mContext},
					wsBtns(create<Strip>(At::Right, At::Left, At::Left)),
					btnClose(wsBtns.create<Button>("x", Vec2f{8.f, 8.f})),
					btnMinimize(wsBtns.create<Button>("_", Vec2f{8.f, 8.f})),
					btnCollapse(wsBtns.create<Button>("^", Vec2f{8.f, 8.f})),
					lblTitle(create<Label>(std::move(mTitle)))
				{
					external = true;
					setFillColor(sf::Color::Black);

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
				//float minWidth{45.f}, minHeight{45.f};
				float oldHeight;
				Scaling oldScalingX, oldScalingY;
				Vec2f dragOrigin;

				inline void update(FT) override
				{
					setFillColor(isFocused() ? colorFocused : colorUnfocused);

					if(action == Action::Move) setPosition(getMousePos() - dragOrigin);
					else if(action == Action::Resize)
					{
						//setSize(ssvu::getClampedMin(getWidth(), minWidth), ssvu::getClampedMin(getHeight(), minHeight));

						auto oldNW(getVertexNW());
						setSize(getMousePos() - dragOrigin);
						setPosition(oldNW + getHalfSize());
					}
				}

			public:
				Form(Context& mContext, std::string mTitle, const Vec2f& mPosition, const Vec2f& mSize) : Widget{mContext, mPosition, mSize / 2.f},
					fbBar(create<FormBar>(std::move(mTitle))), fbResizer(create<Widget>(Vec2f{4.f, 4.f}))
				{
					setOutlineThickness(2); setOutlineColor(sf::Color::Black);

					fbBar.getBtnClose().onLeftClick += [this]{ hide(); };
					fbBar.getBtnMinimize().onLeftClick += [this]{ /* TODO */ };
					fbBar.getBtnCollapse().onLeftClick += [this]{ toggleCollapsed(); };
					fbBar.setScalingX(Scaling::FitToNeighbor);
					fbBar.setHeight(12.f); fbBar.setPadding(-2.f);

					fbResizer.setFillColor(sf::Color::Transparent);
					fbResizer.setOutlineThickness(2); fbResizer.setOutlineColor(sf::Color::Black);

					fbBar.attach(At::Bottom, *this, At::Top);
					fbResizer.attach(At::SE, *this, At::SE);

					fbBar.onLeftClickDown += [this]{ if(draggable) { action = Action::Move; dragOrigin = getMousePos() - getPosition(); } };
					fbBar.onLeftRelease += [this]{ ssvu::lo() << "fbBar released" << std::endl; if(action == Action::Move) action = Action::None; };
					fbResizer.onLeftClickDown += [this]{ if(resizable) { action = Action::Resize; dragOrigin = getMousePos() - getSize(); } };
					fbResizer.onLeftRelease += [this]{ if(action == Action::Resize) action = Action::None; };
				}

				inline void toggleCollapsed()
				{
					action = Action::None;

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
					recurseChildrenIf<false>([this](Widget& mW){ return &mW != &fbBar; }, [this](Widget& mW){ mW.setHiddenRecursive(collapsed); });
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
