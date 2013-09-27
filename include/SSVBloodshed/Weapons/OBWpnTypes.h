// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPNTYPES
#define SSVOB_WEAPONS_WPNTYPES

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Weapons/OBWpn.h"
#include "SSVBloodshed/Components/OBCProjectile.h"

namespace ob
{
	namespace OBWpnTypes
	{
		inline OBWpn createMachineGun()
		{
			return {4.5f, 1.f, 420.f, "Sounds/machineGun.wav",
			[](OBWpn& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBullet(mPos, mDeg));
			}};
		}
		inline OBWpn createPlasmaBolter()
		{
			return {9.5f, 2.f, 290.f, "Sounds/machineGun.wav",
			[](OBWpn& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBoltPlasma(mPos, mDeg));
			}};
		}
		inline OBWpn createEPlasmaBulletGun()
		{
			return {30.f, 0.5f, 320.f, "Sounds/machineGun.wav",
			[](OBWpn& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBulletPlasma(mPos, mDeg));
			}};
		}
		inline OBWpn createEPlasmaStarGun(int mFanCount)
		{
			return {75.f, 1.f, 260.f, "Sounds/machineGun.wav",
			[mFanCount](OBWpn& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				for(int i{-mFanCount}; i <= mFanCount; ++i)
				{
					auto& e(mGame.getFactory().createPJStarPlasma(mPos, mDeg + i * 15.f));
					mWpn.shotProjectile(e).setSpeed(260.f - std::abs(i) * 40.f);
				}
			}};
		}
	}
}

#endif
