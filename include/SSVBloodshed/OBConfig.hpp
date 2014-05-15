// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CONFIG
#define SSVOB_CONFIG

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
	class OBConfig
	{
		SSVUJ_CNV_FRIEND();

		private:
			// Input
			Trigger tLeft, tRight, tUp, tDown;	// Movement triggers
			Trigger tShoot, tSwitch, tBomb;	// Action triggers

			// GFX
			std::size_t particleMax{10000};
			float particleMult{1.f};

			// Gameplay
			float dmgMultGlobal{1.f};	// Multiplier of damage dealt
			float dmgMultPlayer{1.f};	// Multiplier of damage dealt by the player
			float dmgMultEnemy{1.f};	// Multiplier of damage dealt by the enemies

			// SFX
			bool soundEnabled{true}, musicEnabled{true};

			inline OBConfig()
			{
				using k = ssvs::KKey;
				//using b = ssvs::MBtn;
				//using t = ssvs::Input::Type;

				tLeft = {{k::Left}};
				tRight = {{k::Right}};
				tUp = {{k::Up}};
				tDown = {{k::Down}};

				tShoot = {{k::Z}};
				tSwitch = {{k::X}};
				tBomb = {{k::Space}};
			}

		public:
			inline static OBConfig& get() noexcept { static OBConfig instance; return instance; }

			// Input
			inline static const Trigger& getTLeft() noexcept	{ return get().tLeft; }
			inline static const Trigger& getTRight() noexcept	{ return get().tRight; }
			inline static const Trigger& getTUp() noexcept		{ return get().tUp; }
			inline static const Trigger& getTDown() noexcept	{ return get().tDown; }
			inline static const Trigger& getTShoot() noexcept	{ return get().tShoot; }
			inline static const Trigger& getTSwitch() noexcept	{ return get().tSwitch; }
			inline static const Trigger& getTBomb() noexcept	{ return get().tBomb; }

			// GFX
			inline static void setParticleMult(float mValue) noexcept		{ get().particleMult = mValue; }
			inline static void setParticleMax(std::size_t mValue) noexcept	{ get().particleMax = mValue; }

			inline static std::size_t getParticleMax() noexcept				{ return get().particleMax; }
			inline static float getParticleMult() noexcept					{ return get().particleMult; }

			// Gameplay
			inline static void setDmgMultGlobal(float mValue) noexcept		{ get().dmgMultGlobal = mValue; }
			inline static void setDmgMultPlayer(float mValue) noexcept		{ get().dmgMultPlayer = mValue; }
			inline static void setDmgMultEnemy(float mValue) noexcept		{ get().dmgMultEnemy = mValue; }

			inline static float getDmgMultGlobal() noexcept					{ return get().dmgMultGlobal; }
			inline static float getDmgMultPlayer() noexcept					{ return get().dmgMultPlayer * getDmgMultGlobal(); }
			inline static float getDmgMultEnemy() noexcept					{ return get().dmgMultEnemy * getDmgMultGlobal(); }

			// SFX
			inline static void setSoundEnabled(bool mValue) noexcept		{ get().soundEnabled = mValue; }
			inline static void setMusicEnabled(bool mValue) noexcept		{ get().musicEnabled = mValue; }

			inline static bool isSoundEnabled() noexcept					{ return get().soundEnabled; }
			inline static bool isMusicEnabled() noexcept					{ return get().musicEnabled; }
	};
}

namespace ssvuj
{
	template<> SSVUJ_CNV_SIMPLE(ob::OBConfig, mObj, mValue)
	{
		auto& gameplay(ssvuj::getObj(mObj, "gameplay"));
		auto& gfx(ssvuj::getObj(mObj, "gfx"));
		auto& sfx(ssvuj::getObj(mObj, "sfx"));
		auto& input(ssvuj::getObj(mObj, "input"));

		ssvuj::convertObj(gameplay,
				SSVUJ_CNV_OBJ_AUTO(mValue, dmgMultGlobal),
				SSVUJ_CNV_OBJ_AUTO(mValue, dmgMultPlayer),
				SSVUJ_CNV_OBJ_AUTO(mValue, dmgMultEnemy));

		ssvuj::convertObj(gfx,
				SSVUJ_CNV_OBJ_AUTO(mValue, particleMult),
				SSVUJ_CNV_OBJ_AUTO(mValue, particleMax));

		ssvuj::convertObj(sfx,
				SSVUJ_CNV_OBJ_AUTO(mValue, soundEnabled),
				SSVUJ_CNV_OBJ_AUTO(mValue, musicEnabled));

		ssvuj::convertObj(input,
				SSVUJ_CNV_OBJ_AUTO(mValue, tLeft),
				SSVUJ_CNV_OBJ_AUTO(mValue, tRight),
				SSVUJ_CNV_OBJ_AUTO(mValue, tUp),
				SSVUJ_CNV_OBJ_AUTO(mValue, tDown),
				SSVUJ_CNV_OBJ_AUTO(mValue, tShoot),
				SSVUJ_CNV_OBJ_AUTO(mValue, tSwitch),
				SSVUJ_CNV_OBJ_AUTO(mValue, tBomb));
	}
	SSVUJ_CNV_SIMPLE_END();
}

#endif
