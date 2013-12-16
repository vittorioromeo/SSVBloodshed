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
		namespace Internal
		{
			class ClickEffect
			{
				private:
					sf::Color color;
					AABBShape& target;
					float value{0.f};

				public:
					inline ClickEffect(sf::Color mColor, AABBShape& mTarget) noexcept : color{std::move(mColor)}, target(mTarget) { }
					inline void update(FT mFT) noexcept
					{
						auto colorNew(color); colorNew.g = value;
						target.setFillColor(colorNew);
						value = ssvu::getClampedMin(value - mFT * 15.f, 0.f);
					}
					inline void click() noexcept { value = 255; }
			};

			class TextCursor
			{
				private:
					AABBShape shape;
					int idxStart{0}, idxEnd{0}, idxMin, idxMax;
					float blinkValue{0.f};

					inline void delSelection(std::string& mStr)
					{
						mStr.erase(std::begin(mStr) + idxMin, std::begin(mStr) + idxMax);
						setBoth(idxMin);
					}
					inline void moveBoth(int mDir) noexcept	{ idxStart += mDir; idxEnd += mDir; }
					inline bool isSingle() const noexcept { return idxStart == idxEnd; }
					inline int getBreakGroup(char mChar) { if(std::isspace(mChar)) return 0; if(std::ispunct(mChar)) return 1; return -1; }
					inline int findNextWordBeginEnd(std::string& mStr, int mPos, int mDir)
					{
						do
						{
							mPos += mDir;
						} while(getBreakGroup(mStr[mPos]) == getBreakGroup(mStr[mPos - std::abs(mDir)]));

						return mPos;
					}

				public:
					inline TextCursor(float mFontHeight) : shape{Vec2f{0.f, 0.f}, Vec2f{1.f, mFontHeight / 2.f}} { }

					inline void update(FT mFT, std::size_t mStrSize) noexcept
					{
						blinkValue += mFT * 0.08f;

						ssvu::clamp(idxStart, 0, mStrSize);
						ssvu::clamp(idxEnd, 0, mStrSize);
						idxMin = std::min(idxStart, idxEnd);
						idxMax = std::max(idxStart, idxEnd);
					}

					inline const AABBShape& getShape() { return shape; }

					inline void refreshShape(float mLeft, float mY, float mSpacing)
					{
						auto effect(std::abs(std::sin(blinkValue)));
						shape.setPosition(Vec2f{mLeft + ssvs::getGlobalWidth(shape) / 2.f, mY} + Vec2f(mSpacing * idxMin, 0.f));

						if(isSingle())
						{
							shape.setFillColor(sf::Color(0, 0, 0, 155 + effect * 100));
							shape.setWidth(2.f);
						}
						else
						{
							shape.setFillColor(sf::Color(0, 55, 200, 75 + effect * 75));
							shape.setWidth(mSpacing * (idxMax - idxMin) + 3.f);
						}
					}

					inline void setEnd(int mIdx) noexcept	{ idxEnd = mIdx; }
					inline void setBoth(int mIdx) noexcept	{ idxStart = idxEnd = mIdx; }

					inline void delStrBack(std::string& mStr)
					{
						if(!isSingle()) { delSelection(mStr); return; }

						auto itrStart(std::begin(mStr) + idxStart);
						if(itrStart > std::begin(mStr)) mStr.erase(itrStart - 1, itrStart);
						moveBoth(-1);
					}

					inline void delStrFwd(std::string& mStr)
					{
						if(!isSingle()) { delSelection(mStr); return; }

						auto itrStart(std::begin(mStr) + idxStart);
						if(itrStart < std::end(mStr)) mStr.erase(itrStart, itrStart + 1);
					}

					inline void insertStr(std::string& mStr, char mChar)
					{
						if(!isSingle()) delSelection(mStr);
						mStr.insert(std::begin(mStr) + idxMin, mChar);
						moveBoth(1);
					}


					inline void move(std::string& mStr, bool mShift, bool mCtrl, int mDir)
					{
						if(mShift)
						{
							if(mCtrl) idxEnd = findNextWordBeginEnd(mStr, idxEnd, mDir);
							else idxEnd += mDir;
						}
						else if(!isSingle())
						{
							if(mCtrl)
							{
								int& idxVar(mDir > 0 ? idxMax : idxMin);
								idxVar = findNextWordBeginEnd(mStr, idxVar, mDir);
								setBoth(idxVar);
							}
							else setBoth(mDir > 0 ? idxMax + 1 : idxMin - 1);
						}
						else
						{
							if(mCtrl)
							{
								idxStart = findNextWordBeginEnd(mStr, idxStart, mDir);
								setBoth(idxStart);
							}
							else moveBoth(mDir);
						}
					}
			};
		}

		class Label : public Widget
		{
			private:
				ssvs::BitmapText text;
				bool scaleWithText{true};

				inline void drawWidget() override { text.setPosition(getX(), getY() - 1.f); render(text); }

			public:
				Label(Context& mContext, std::string mText = "") : Widget{mContext}, text{getStyle().font}
				{
					text.setColor(getStyle().colorText); text.setTracking(-3);
					setFillColor(getStyle().colorLabelBG);
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
				Internal::ClickEffect clickEffect{getStyle().colorBtnUnpressed, *this};

				inline void update(FT mFT) override { clickEffect.update(mFT); }

			public:
				Button(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					lblLabel(create<Label>(std::move(mLabel)))
				{
					setOutlineThickness(getStyle().outlineThickness); setOutlineColor(getStyle().colorOutline);
					lblLabel.attach(At::Center, *this, At::Center);
					onLeftClick += [this]{ clickEffect.click(); };
				}

				inline Label& getLabel() noexcept { return lblLabel; }
		};

		namespace Internal
		{
			class CheckBoxStateBox : public Widget
			{
				private:
					Label& lblState;
					Internal::ClickEffect clickEffect{getStyle().colorBtnUnpressed, *this};

					inline void update(FT mFT) override { clickEffect.update(mFT); }

				public:
					CheckBoxStateBox(Context& mContext) : Widget{mContext}, lblState(create<Label>(""))
					{
						setOutlineThickness(getStyle().outlineThickness); setOutlineColor(getStyle().colorOutline);
						setSize(getStyle().getBtnSquareSize());
						lblState.attach(At::Center, *this, At::Center);
					}

					inline void setState(bool mValue) { clickEffect.click(); lblState.setString(mValue ? "x" : ""); }
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
					lblLabel.attach(At::Left, cbsbBox, At::Right, Vec2f{getStyle().outlineThickness, 0.f});

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
					if(children.empty()) return;

					Widget* prev{nullptr};
					for(auto i(0u); i < children.size(); ++i)
					{
						auto& w(*children[i]);
						if(!w.isVisible()) continue;

						if(prev == nullptr)
						{
							prev = &w;
							prev->attach(alignFrom, *this, alignFrom, getAtVec(alignDir, getPadding()));
							continue;
						}

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
					wsShutter.setFillColor(wsShutter.isFocused() ? getStyle().colorBaseFocused : getStyle().colorBaseUnfocused);
				}

			public:
				Shutter(Context& mContext, std::string mLabel, const Vec2f& mSize) : Widget{mContext},
					wsBar(create<Strip>(At::Right, At::Left, At::Left)),
					wsShutter(create<Strip>(At::Top, At::Bottom, At::Bottom)),
					btnOpen(wsBar.create<Button>("v", getStyle().getBtnSquareSize())),
					lblLabel(wsBar.create<Label>(std::move(mLabel)))
				{
					setFillColor(sf::Color::Transparent);

					wsBar.setFillColor(sf::Color::Red);
					wsBar.setOutlineColor(getStyle().colorOutline);
					wsBar.setOutlineThickness(getStyle().outlineThickness);
					wsBar.setScaling(Scaling::Manual);
					wsBar.setSize(mSize);

					wsShutter.setOutlineColor(getStyle().colorOutline);
					wsShutter.setOutlineThickness(getStyle().outlineThickness);
					wsShutter.setExcludedRecursive(true);
					wsShutter.setContainer(true);
					wsShutter.setPadding(getStyle().padding * 2.f);
					wsShutter.setExternal(true);

					wsBar.attach(At::Right, *this, At::Right);
					wsShutter.attach(At::Top, wsBar, At::Bottom, Vec2f{0.f, getStyle().padding});

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
						btnUp(wsScroll.create<Button>("^", getStyle().getBtnSquareSize())),
						btnDown(wsScroll.create<Button>("v", getStyle().getBtnSquareSize())),
						lblCount(wsScroll.create<Label>(""))
				{
					btnUp.onLeftClick += [this]{ --idxOffset; refreshChoices(); };
					btnDown.onLeftClick += [this]{ ++idxOffset; refreshChoices(); };

					for(auto i(0u); i < choiceBtnsMax; ++i)
					{
						auto& btn(wsChoices.create<Button>("", getStyle().getBtnSizePerChar(7)));
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
				float green{0.f};
				Internal::TextCursor cursor;

				inline void update(FT mFT) override
				{
					cursor.update(mFT, editStr.size());

					if(editing)
					{
						green += mFT * 0.08f;
						auto effect(std::abs(std::sin(green)));

						cursor.refreshShape(ssvs::getGlobalLeft(lblText.getText()), lblText.getY(), getCursorSpacing());

						lblText.getText().setColor(sf::Color(255, effect * 255, 255, 255));
						lblText.setString(editStr);
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
							if(e.text.unicode == '\b' && !editStr.empty()) cursor.delStrBack(editStr);
							else if(e.text.unicode > 31 && e.text.unicode < 127) cursor.insertStr(editStr, char(e.text.unicode));
						}
						else if(e.type == sf::Event::KeyPressed)
						{
							if(e.key.code == ssvs::KKey::Return) finishEditing();
							else if(e.key.code == ssvs::KKey::Left) cursor.move(editStr, isKeyPressed(ssvs::KKey::LShift), isKeyPressed(ssvs::KKey::LControl), -1);
							else if(e.key.code == ssvs::KKey::Right) cursor.move(editStr, isKeyPressed(ssvs::KKey::LShift), isKeyPressed(ssvs::KKey::LControl), 1);
							else if(e.key.code == ssvs::KKey::Home) cursor.setBoth(0);
							else if(e.key.code == ssvs::KKey::End) cursor.setBoth(editStr.size());
							else if(e.key.code == ssvs::KKey::Delete) cursor.delStrFwd(editStr);
						}
					}
				}

				inline void finishEditing() { editing = false; str = editStr; onTextChanged(); }
				inline float getCursorSpacing() const noexcept { return getStyle().getGlyphWidth() + lblText.getText().getTracking(); }
				inline int getCursorPos() const noexcept { return (getMousePos().x - ssvs::getGlobalLeft(lblText.getText())) / getCursorSpacing(); }

			public:
				ssvu::Delegate<void()> onTextChanged;

				TextBox(Context& mContext, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					tBox(create<Widget>()), lblText(tBox.create<Label>("")),
					cursor{float(getStyle().getGlyphHeight())}
				{
					setFillColor(sf::Color::Transparent);

					lblText.onPostDraw += [this]{ if(editing) render(cursor.getShape()); };

					tBox.setOutlineColor(getStyle().colorOutline);
					tBox.setOutlineThickness(getStyle().outlineThickness);
					tBox.setFillColor(sf::Color::White);
					tBox.setScaling(Scaling::FitToNeighbor);

					lblText.setScaling(Scaling::FitToNeighbor);

					tBox.attach(At::Center, *this, At::Center);
					lblText.attach(At::Center, tBox, At::Center);

					onLeftClick += [this]
					{
						if(!editing) editStr = str;
						editing = true;

						if(isKeyPressed(ssvs::KKey::LShift)) cursor.setEnd(getCursorPos());
						else cursor.setBoth(getCursorPos());
					};
					onLeftClickDown += [this]{ cursor.setEnd(getCursorPos()); };
					onFocusChanged += [this](bool mFocus){ if(editing && !mFocus) finishEditing(); };
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
					btnClose(wsBtns.create<Button>("x", getStyle().getBtnSquareSize())),
					btnMinimize(wsBtns.create<Button>("_", getStyle().getBtnSquareSize())),
					btnCollapse(wsBtns.create<Button>("^", getStyle().getBtnSquareSize())),
					lblTitle(create<Label>(std::move(mTitle)))
				{
					external = true;
					setFillColor(sf::Color::Black);

					wsBtns.setWidgetOffset(getStyle().outlineThickness);

					wsBtns.attach(At::Right, *this, At::Right, Vec2f{-getStyle().padding, 0.f});
					lblTitle.attach(At::NW, *this, At::NW, Vec2f{0.f, getStyle().padding});
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
				float oldHeight;
				Scaling oldScalingX, oldScalingY;
				Vec2f dragOrigin;

				inline void update(FT) override
				{
					setFillColor(isFocused() ? getStyle().colorBaseFocused : getStyle().colorBaseUnfocused);

					if(action == Action::Move) setPosition(getMousePos() - dragOrigin);
					else if(action == Action::Resize)
					{
						auto oldNW(getVertexNW());
						setSize(getMousePos() - dragOrigin);
						setPosition(oldNW + getHalfSize());
					}
				}

			public:
				Form(Context& mContext, std::string mTitle, const Vec2f& mPosition, const Vec2f& mSize) : Widget{mContext, mPosition, mSize / 2.f},
					fbBar(create<FormBar>(std::move(mTitle))), fbResizer(create<Widget>(Vec2f{4.f, 4.f}))
				{
					setOutlineThickness(getStyle().outlineThickness); setOutlineColor(getStyle().colorOutline);

					fbBar.getBtnClose().onLeftClick += [this]{ hide(); };
					fbBar.getBtnMinimize().onLeftClick += [this]{ /* TODO */ };
					fbBar.getBtnCollapse().onLeftClick += [this]{ toggleCollapsed(); };
					fbBar.setScalingX(Scaling::FitToNeighbor);
					fbBar.setScalingY(Scaling::FitToChildren); fbBar.setPadding(getStyle().padding);

					fbResizer.setFillColor(sf::Color::Transparent);
					fbResizer.setOutlineThickness(getStyle().outlineThickness); fbResizer.setOutlineColor(getStyle().colorOutline);

					fbBar.attach(At::Bottom, *this, At::Top);
					fbResizer.attach(At::SE, *this, At::SE);

					fbBar.onLeftClickDown += [this]{ if(draggable) { action = Action::Move; dragOrigin = getMousePos() - getPosition(); } };
					fbBar.onLeftRelease += [this]{ if(action == Action::Move) action = Action::None; };
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
						resizeFromBottom(1.f);
					}
					else
					{
						setScalingX(oldScalingX); setScalingY(oldScalingY);
						fbBar.getBtnCollapse().getLabel().setString("^");
						resizeFromBottom(oldHeight);
					}

					collapsed = !collapsed;
					recurseChildrenIf<false>([this](Widget& mW){ return &mW != &fbBar; }, [this](Widget& mW){ mW.setCollapsedRecursive(collapsed); });
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
