// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUI_CONTROLS_STRIP
#define SSVOB_GUI_CONTROLS_STRIP

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/GUI/Widget.hpp"

namespace ob
{
	namespace GUI
	{
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
	}
}

#endif
