// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CONFIG
#define SSVOB_CONFIG

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
class OBConfig
{
    SSVJ_CNV_FRIEND();

private:
    // Input
    Trigger tLeft, tRight, tUp, tDown; // Movement triggers
    Trigger tShoot, tSwitch, tBomb;    // Action triggers

    // GFX
    SizeT particleMax{10000};
    float particleMult{1.f};

    // Gameplay
    float dmgMultGlobal{1.f}; // Multiplier of damage dealt
    float dmgMultPlayer{1.f}; // Multiplier of damage dealt by the player
    float dmgMultEnemy{1.f};  // Multiplier of damage dealt by the enemies

    // SFX
    bool soundEnabled{true}, musicEnabled{true};

    inline OBConfig()
    {
        using k = ssvs::KKey;
        // using b = ssvs::MBtn;
        // using t = ssvs::Input::Type;

        tLeft = {{k::Left}};
        tRight = {{k::Right}};
        tUp = {{k::Up}};
        tDown = {{k::Down}};

        tShoot = {{k::Z}};
        tSwitch = {{k::X}};
        tBomb = {{k::Space}};
    }

public:
    inline static OBConfig& get() noexcept
    {
        static OBConfig instance;
        return instance;
    }

    // Input
    inline static const Trigger& getTLeft() noexcept { return get().tLeft; }
    inline static const Trigger& getTRight() noexcept { return get().tRight; }
    inline static const Trigger& getTUp() noexcept { return get().tUp; }
    inline static const Trigger& getTDown() noexcept { return get().tDown; }
    inline static const Trigger& getTShoot() noexcept { return get().tShoot; }
    inline static const Trigger& getTSwitch() noexcept { return get().tSwitch; }
    inline static const Trigger& getTBomb() noexcept { return get().tBomb; }

    // GFX
    inline static void setParticleMult(float mX) noexcept
    {
        get().particleMult = mX;
    }
    inline static void setParticleMax(SizeT mX) noexcept
    {
        get().particleMax = mX;
    }

    inline static SizeT getParticleMax() noexcept { return get().particleMax; }
    inline static float getParticleMult() noexcept
    {
        return get().particleMult;
    }

    // Gameplay
    inline static void setDmgMultGlobal(float mX) noexcept
    {
        get().dmgMultGlobal = mX;
    }
    inline static void setDmgMultPlayer(float mX) noexcept
    {
        get().dmgMultPlayer = mX;
    }
    inline static void setDmgMultEnemy(float mX) noexcept
    {
        get().dmgMultEnemy = mX;
    }

    inline static float getDmgMultGlobal() noexcept
    {
        return get().dmgMultGlobal;
    }
    inline static float getDmgMultPlayer() noexcept
    {
        return get().dmgMultPlayer * getDmgMultGlobal();
    }
    inline static float getDmgMultEnemy() noexcept
    {
        return get().dmgMultEnemy * getDmgMultGlobal();
    }

    // SFX
    inline static void setSoundEnabled(bool mX) noexcept
    {
        get().soundEnabled = mX;
    }
    inline static void setMusicEnabled(bool mX) noexcept
    {
        get().musicEnabled = mX;
    }

    inline static bool isSoundEnabled() noexcept { return get().soundEnabled; }
    inline static bool isMusicEnabled() noexcept { return get().musicEnabled; }
};
}

// TODO: even more abstract macro?
SSVJ_CNV_NAMESPACE()
{
    template <>
    SSVJ_CNV(ob::OBConfig, mV, mX)
    {
        auto& gameplay(mV["gameplay"]);
        auto& gfx(mV["gfx"]);
        auto& sfx(mV["sfx"]);
        auto& input(mV["input"]);

        SSVJ_SRLZ_OBJ_AUTO(
        gameplay, mX, dmgMultGlobal, dmgMultPlayer, dmgMultEnemy);

        SSVJ_SRLZ_OBJ_AUTO(gfx, mX, particleMult, particleMax);

        SSVJ_SRLZ_OBJ_AUTO(sfx, mX, soundEnabled, musicEnabled);

        SSVJ_SRLZ_OBJ_AUTO(
        input, mX, tLeft, tRight, tUp, tDown, tShoot, tSwitch, tBomb);
    }
    SSVJ_CNV_END()
}
SSVJ_CNV_NAMESPACE_END()

#endif
