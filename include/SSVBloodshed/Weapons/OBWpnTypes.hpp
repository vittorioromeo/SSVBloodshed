// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPNTYPES
#define SSVOB_WEAPONS_WPNTYPES

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Weapons/OBWpnType.hpp"
#include "SSVBloodshed/Components/OBCProjectile.hpp"

namespace ob
{
	namespace OBWpnTypes
	{
		//using FactoryMemPtr = Entity&(OBFactory::*)(const Vec2i& mPos, float mDeg);
		//template<typename... TArgs> using FactoryMemPtr = Entity&(OBFactory::*)(TArgs...);

		template<typename T> inline void fanShoot(T mFactoryAction, OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, int mCount, float mStep, float mSpeedChange)
		{
			for(int i{-mCount}; i <= mCount; ++i)
			{
				auto& e((mGame.getFactory().*mFactoryAction)(mShooter, mPos, mDeg + i * mStep));
				auto& cProjectile(mWpn.shotProjectile(e));
				cProjectile.setSpeed(cProjectile.getSpeed() - std::abs(i) * mSpeedChange);
			}
		}

		inline OBWpnType createMachineGun()
		{
			return {4.5f, 1.f, 420.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBullet(mShooter, mPos, mDeg));
				mGame.createPCaseBullet(1, mMuzzlePxPos, mDeg);
				mGame.createPMuzzleBullet(16, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createPlasmaBolter()
		{
			return {9.5f, 2.f, 290.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJBoltPlasma(mShooter, mPos, mDeg));
				mGame.createPMuzzlePlasma(20, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createEPlasmaBulletGun(int mFanCount = 0, float mStep = 12.f)
		{
			return {45.f, 0.5f, 320.f, "Sounds/machineGun.wav",
			[mFanCount, mStep](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				fanShoot(&OBFactory::createPJBulletPlasma, mWpn, mGame, mShooter, mPos, mDeg, mFanCount, mStep, 40.f);
				mGame.createPMuzzlePlasma(16, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createEPlasmaStarGun(int mFanCount = 0, float mStep = 12.f)
		{
			return {75.f, 1.f, 260.f, "Sounds/machineGun.wav",
			[mFanCount, mStep](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				fanShoot(&OBFactory::createPJStarPlasma, mWpn, mGame, mShooter, mPos, mDeg, mFanCount, mStep, 40.f);
				mGame.createPMuzzlePlasma(16, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createPlasmaCannon()
		{
			return {120.f, 5.f, 180.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJCannonPlasma(mShooter, mPos, mDeg));
				mGame.createPMuzzlePlasma(30, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createRocketLauncher()
		{
			return {80.f, 5.f, 25.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJRocket(mShooter, mPos, mDeg));
				mGame.createPCaseRocket(1, mMuzzlePxPos, mDeg);
				mGame.createPMuzzleRocket(14, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createGrenadeLauncher()
		{
			return {45.f, 5.f, 180.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJGrenade(mShooter, mPos, mDeg));
				mGame.createPCaseRocket(1, mMuzzlePxPos, mDeg);
				mGame.createPMuzzleRocket(8, mMuzzlePxPos);
			}};
		}
		inline OBWpnType createShockwaveGun()
		{
			return {19.5f, 1.7f, 220.f, "Sounds/machineGun.wav",
			[](OBWpnType& mWpn, OBGame& mGame, OBCActorND* mShooter, const Vec2i& mPos, float mDeg, const Vec2f& mMuzzlePxPos)
			{
				mWpn.shotProjectile(mGame.getFactory().createPJShockwave(mShooter, mPos, mDeg, 3));
				mGame.createPMuzzleShockwave(20, mMuzzlePxPos);
			}};
		}
	}
}

#endif
