// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPNTYPES
#define SSVOB_WEAPONS_WPNTYPES

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Weapons/OBWpnType.h"
#include "SSVBloodshed/Components/OBCProjectile.h"

namespace ob
{
	namespace OBWpnTypes
	{
		using FactoryMemPtr = Entity&(OBFactory::*)(const Vec2i& mPos, float mDeg);
		//template<typename... TArgs> using FactoryMemPtr = Entity&(OBFactory::*)(TArgs...);

		template<typename T> inline void fanShoot(T mFactoryAction, OBWpnType& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg, int mCount, float mStep, float mSpeedChange)
		{
			for(int i{-mCount}; i <= mCount; ++i)
			{
				auto& e((mGame.getFactory().*mFactoryAction)(mPos, mDeg + i * mStep));
				auto& cProjectile(mWpn.shotProjectile(e));
				cProjectile.setSpeed(cProjectile.getSpeed() - std::abs(i) * mSpeedChange);
			}
		}

		inline OBWpnType createMachineGun()
		{
			return {4.5f, 1.f, 420.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBullet(mPos, mDeg));
			}};
		}
		inline OBWpnType createPlasmaBolter()
		{
			return {9.5f, 2.f, 290.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBoltPlasma(mPos, mDeg));
			}};
		}
		inline OBWpnType createEPlasmaBulletGun(int mFanCount = 0, float mStep = 12.f)
		{
			return {45.f, 0.5f, 320.f, "Sounds/machineGun.wav",
			[mFanCount, mStep](OBWpnType& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				fanShoot(&OBFactory::createPJBulletPlasma, mWpn, mGame, mPos, mDeg, mFanCount, mStep, 40.f);
			}};
		}
		inline OBWpnType createEPlasmaStarGun(int mFanCount = 0, float mStep = 12.f)
		{
			return {75.f, 1.f, 260.f, "Sounds/machineGun.wav",
			[mFanCount, mStep](OBWpnType& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				fanShoot(&OBFactory::createPJStarPlasma, mWpn, mGame, mPos, mDeg, mFanCount, mStep, 40.f);
			}};
		}
		inline OBWpnType createPlasmaCannon()
		{
			return {120.f, 5.f, 180.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, const Vec2i& mPos, float mDeg)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJCannonPlasma(mPos, mDeg));
			}};
		}
	}
}

#endif
