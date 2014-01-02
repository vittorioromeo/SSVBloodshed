// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GUIOB_FORMIO
#define SSVOB_GUIOB_FORMIO

#include "SSVBloodshed/GUI/GUI.hpp"

namespace ob
{
	class FormIO : public GUI::Form
	{
		private:
			GUI::Strip& mainStrip;
			GUI::Label& lblCurrentPath;
			GUI::TextBox& tboxFilename;
			GUI::Strip& stripBtns;
			GUI::Button& btnSave;
			GUI::Button& btnLoad;

		public:
			ssvu::Delegate<void(const std::string&)> onSave, onLoad;

			FormIO(GUI::Context& mCtx) : GUI::Form{mCtx, "IO", Vec2f{300.f, 300.f}, Vec2f{100.f, 100.f}},
				mainStrip(create<GUI::Strip>(GUI::At::Top, GUI::At::Bottom, GUI::At::Bottom)),
				lblCurrentPath(mainStrip.create<GUI::Label>("CURRENT: null")),
				tboxFilename(mainStrip.create<GUI::TextBox>(getStyle().getBtnSizePerChar(14))),
				stripBtns(mainStrip.create<GUI::Strip>(GUI::At::Left, GUI::At::Right, GUI::At::Right)),
				btnSave(stripBtns.create<GUI::Button>("save", getStyle().getBtnSizePerChar(6))),
				btnLoad(stripBtns.create<GUI::Button>("load", getStyle().getBtnSizePerChar(6)))
			{
				setScaling(GUI::Scaling::FitToChildren);
				setResizable(false); setPadding(2.f);

				mainStrip.attach(GUI::At::Center, *this, GUI::At::Center);
				mainStrip.setPadding(2.f);

				tboxFilename.setString("./level.lvl");

				btnSave.onLeftClick += [this]{ if(!tboxFilename.getString().empty()) onSave(tboxFilename.getString()); };
				btnLoad.onLeftClick += [this]{ if(!tboxFilename.getString().empty()) onLoad(tboxFilename.getString()); };
			}

			inline GUI::Label& getLblCurrentPath() noexcept	{ return lblCurrentPath; }
			inline GUI::Button& getBtnSave() noexcept		{ return btnSave; }
	};
}

#endif
