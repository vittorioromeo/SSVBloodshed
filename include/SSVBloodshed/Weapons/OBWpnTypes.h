// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPNTYPES
#define SSVOB_WEAPONS_WPNTYPES

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Weapons/OBWpn.h"
#include "SSVBloodshed/Components/OBCWpnController.h"

namespace ob
{
	namespace OBWpnTypes
	{
		inline OBWpn createMachineGun()
		{
			OBWpn result{4.5f, 1.f, 420.f, "Sounds/machineGun.wav"};
			result.onShoot += [](OBCWpnController& mCnt, const Vec2i& mPos, float mDeg)
			{
				mCnt.shotProjectile(mCnt.getFactory().createPJBullet(mPos, mDeg));
			};
			return result;
		}
		inline OBWpn createPlasmaBolter()
		{
			OBWpn result{9.5f, 2.f, 290.f, "Sounds/machineGun.wav"};
			result.onShoot += [](OBCWpnController& mCnt, const Vec2i& mPos, float mDeg)
			{
				mCnt.shotProjectile(mCnt.getFactory().createPJBoltPlasma(mPos, mDeg));
			};
			return result;
		}
		inline OBWpn createEPlasmaBulletGun()
		{
			OBWpn result{30.f, 0.5f, 320.f, "Sounds/machineGun.wav"};
			result.onShoot += [](OBCWpnController& mCnt, const Vec2i& mPos, float mDeg)
			{
				mCnt.shotProjectile(mCnt.getFactory().createPJBulletPlasma(mPos, mDeg));
			};
			return result;
		}
		inline OBWpn createEPlasmaStarGun(int mFanCount)
		{
			OBWpn result{75.f, 1.f, 260.f, "Sounds/machineGun.wav"};
			result.onShoot += [mFanCount](OBCWpnController& mCnt, const Vec2i& mPos, float mDeg)
			{
				for(int i{-mFanCount}; i <= mFanCount; ++i)
				{
					auto& e(mCnt.getFactory().createPJStarPlasma(mPos, mDeg + i * 15.f));
					mCnt.shotProjectile(e).setSpeed(260.f - std::abs(i) * 40.f);
				}
			};
			return result;
		}
	}
}

#endif
