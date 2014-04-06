// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_TEXTBOX
#define SSVOB_GUI_CONTROLS_TEXTBOX

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"
#include "SSVBloodshed/GUI/Controls/Label.hpp"

namespace ob
{
	namespace GUI
	{
		class TextBox : public Widget
		{
			private:
				Widget& tBox;
				Label& lblText;
				bool editing{false};
				std::string editStr, str;
				float green{0.f};
				Internal::TextCursor cursor;
				Internal::TextCursorShape cursorShape;

				inline void update(FT mFT) override
				{
					if(editing)
					{
						green += mFT * 0.08f;
						auto effect(std::abs(std::sin(green)));

						cursorShape.refreshShape(cursor, ssvs::getGlobalLeft(lblText.getText()), lblText.getY(), getCursorSpacing());

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

						auto modLShift(isKeyPressed(ssvs::KKey::LShift));
						auto modLCtrl(isKeyPressed(ssvs::KKey::LControl));

						if(e.type == sf::Event::TextEntered)
						{
							if(e.text.unicode == '\b' && !editStr.empty()) cursor.delStrBack(modLCtrl);
							else if(e.text.unicode > 31 && e.text.unicode < 127) cursor.insertStr(char(e.text.unicode));
						}
						else if(e.type == sf::Event::KeyPressed)
						{
							if(e.key.code == ssvs::KKey::Return)		finishEditing();
							else if(e.key.code == ssvs::KKey::Escape)	finishEditing();
							else if(e.key.code == ssvs::KKey::Left)		cursor.move(modLShift, modLCtrl, -1);
							else if(e.key.code == ssvs::KKey::Right)	cursor.move(modLShift, modLCtrl, 1);
							else if(e.key.code == ssvs::KKey::Home)		cursor.setBoth(0);
							else if(e.key.code == ssvs::KKey::End)		cursor.setBoth(editStr.size());
							else if(e.key.code == ssvs::KKey::Delete)	cursor.delStrFwd(modLCtrl);
						}
					}

					if(editing && !isFocused()) finishEditing();
				}

				inline void finishEditing() { editing = false; str = editStr; onTextChanged(); }
				inline float getCursorSpacing() const noexcept { return getStyle().getGlyphWidth() + lblText.getText().getTracking(); }
				inline int getCursorPos() const noexcept { return (getMousePos().x - ssvs::getGlobalLeft(lblText.getText())) / getCursorSpacing(); }

			public:
				ssvu::Delegate<void()> onTextChanged;

				TextBox(Context& mContext, const Vec2f& mSize) : Widget{mContext, mSize / 2.f},
					tBox(create<Widget>()), lblText(tBox.create<Label>("")),
					cursor(editStr), cursorShape{float(getStyle().getGlyphHeight())}
				{
					setFillColor(sf::Color::Transparent);

					lblText.onPostDraw += [this]{ if(editing) render(cursorShape.getShape()); };

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
				}

				inline void setString(std::string mString) { str = std::move(mString); }
				inline const std::string& getString() const noexcept { return str; }
		};
	}
}

#endif
