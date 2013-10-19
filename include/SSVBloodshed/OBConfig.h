// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CONFIG
#define SSVOB_CONFIG

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class OBConfig
	{
		private:
			bool soundEnabled{true}, musicEnabled{true};
			ssvs::Input::Trigger tLeft, tRight, tUp, tDown;	// Movement triggers
			ssvs::Input::Trigger tShoot, tSwitch, tBomb;	// Action triggers

			inline static OBConfig& get() noexcept { static OBConfig instance; return instance; }

			inline OBConfig()
			{
				using k = ssvs::KKey;
				//using b = ssvs::MBtn;
				//using t = ssvs::Input::Trigger::Type;

				tLeft = {{k::Left}};
				tRight = {{k::Right}};
				tUp = {{k::Up}};
				tDown = {{k::Down}};

				tShoot = {{k::Z}};
				tSwitch = {{k::X}};
				tBomb = {{k::Space}};
			}

		public:
			inline static void setSoundEnabled(bool mValue) noexcept		{ get().soundEnabled = mValue; }
			inline static void setMusicEnabled(bool mValue) noexcept		{ get().musicEnabled = mValue; }

			inline static bool isSoundEnabled() noexcept					{ return get().soundEnabled; }
			inline static bool isMusicEnabled() noexcept					{ return get().musicEnabled; }

			inline static const ssvs::Input::Trigger& getTLeft() noexcept	{ return get().tLeft; }
			inline static const ssvs::Input::Trigger& getTRight() noexcept	{ return get().tRight; }
			inline static const ssvs::Input::Trigger& getTUp() noexcept		{ return get().tUp; }
			inline static const ssvs::Input::Trigger& getTDown() noexcept	{ return get().tDown; }

			inline static const ssvs::Input::Trigger& getTShoot() noexcept	{ return get().tShoot; }
			inline static const ssvs::Input::Trigger& getTSwitch() noexcept	{ return get().tSwitch; }
			inline static const ssvs::Input::Trigger& getTBomb() noexcept	{ return get().tBomb; }
	};
}

#endif
