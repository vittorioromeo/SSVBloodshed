// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLETYPES
#define SSVOB_PARTICLES_PARTICLETYPES

#include "SSVBloodshed/Particles/OBParticleSystem.hpp"
#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
	inline void createPBlood(OBParticleSystem& mPS, const Vec2f& mPos, float mMult)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.f, 13.f * mMult)),
				0.9f,
				sf::Color{ssvu::getRnd<unsigned char>(185, 255), 0, 0, 255},
				0.5f + ssvu::getRndR<float>(-0.3f, 0.3f),
				75 + ssvu::getRnd(-65, 65),
				0.42f);
	}
	inline void createPGib(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.1f, 13.f)),
				0.93f,
				sf::Color{ssvu::getRnd<unsigned char>(95, 170), 15, 15, 255},
				0.6f + ssvu::getRndR<float>(-0.3f, 0.3f),
				150 + ssvu::getRnd(-50, 50),
				1.5f);
	}
	inline void createPDebris(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 9.f)),
				0.9f,
				sf::Color::Black,
				0.5f + ssvu::getRndR<float>(-0.3f, 0.3f),
				65 + ssvu::getRnd(-50, 50),
				0.8f);
	}
	inline void createPDebrisFloor(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.5f, 3.4f)),
				0.9f,
				sf::Color{179, 179, 179, 255},
				2.5f,
				65 + ssvu::getRnd(-50, 50),
				0.8f);
	}
	inline void createPMuzzleBullet(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 4.5f)),
				0.9f,
				sf::Color{255, ssvu::getRnd<unsigned char>(95, 100), 15, 255},
				0.6f + ssvu::getRndR<float>(-0.3f, 0.3f),
				6 + ssvu::getRnd(-5, 7),
				1.5f);
	}
	inline void createPMuzzlePlasma(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 4.5f)),
				0.89f,
				sf::Color{225, 55, ssvu::getRnd<unsigned char>(95, 100), 255},
				0.65f + ssvu::getRndR<float>(-0.3f, 0.3f),
				9 + ssvu::getRnd(-4, 7),
				1.5f,
				ssvu::getRndR(-1.5f, 1.5f));
	}
	inline void createPMuzzleRocket(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 4.5f)),
				0.9f,
				sf::Color{255, ssvu::getRnd<unsigned char>(95, 100), 15, 255},
				0.85f + ssvu::getRndR<float>(-0.3f, 0.3f),
				6 + ssvu::getRnd(-3, 12),
				1.5f);
	}
	inline void createPPlasma(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 1.5f)),
				0.95f,
				sf::Color{255, 15, ssvu::getRnd<unsigned char>(95, 100), 255},
				0.62f + ssvu::getRndR<float>(-0.3f, 0.3f),
				5 + ssvu::getRnd(-4, 4),
				1.5f);
	}
	inline void createPSmoke(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 1.5f)),
				0.96f,
				sf::Color{45, 45, 45, 225},
				0.6f + ssvu::getRndR<float>(-0.3f, 0.3f),
				5 + ssvu::getRnd(-4, 4),
				1.5f);
	}
	inline void createPElectric(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 7.4f)),
				0.94f,
				(ssvu::getRnd(0, 10) > 8) ? sf::Color::Cyan : sf::Color::Yellow,
				0.6f + ssvu::getRndR<float>(-0.3f, 0.3f),
				65 + ssvu::getRnd(-25, 55),
				1.5f,
				ssvu::getRndR(-2.5f, 2.5f));
	}
	inline void createPCharge(OBParticleSystem& mPS, const Vec2f& mPos, float mDist)
	{
		Vec2f pos{ssvs::getOrbitDeg(mPos, ssvu::getRndR<float>(0.f, 360.f), mDist)};

		mPS.emplace(pos,
				ssvs::getNormalized(mPos - pos) * (mDist / 15.f),
				0.97f,
				(ssvu::getRnd(0, 10) > 7) ? sf::Color::Yellow : sf::Color::Red,
				0.7f + ssvu::getRndR<float>(-0.3f, 0.3f),
				mDist + ssvu::getRnd(-10, 15),
				1.f,
				ssvu::getRndR(-2.5f, 2.5f));
	}
	inline void createPShard(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 5.4f)),
				0.97f,
				(ssvu::getRnd(0, 10) > 7) ? sf::Color::Yellow : sf::Color::Red,
				0.7f + ssvu::getRndR<float>(-0.3f, 0.3f),
				25 + ssvu::getRnd(-10, 15),
				1.f,
				ssvu::getRndR(-2.5f, 2.5f));
	}
	inline void createPExplosion(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 6.7f)),
				0.965f,
				(ssvu::getRnd(0, 10) > 7) ? sf::Color::Yellow : sf::Color::Red,
				0.7f + ssvu::getRndR<float>(-1.2f, 1.8f),
				20 + ssvu::getRnd(-15, 25),
				1.f,
				ssvu::getRndR(-2.5f, 2.5f));
	}
	inline void createPHeal(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 5.4f)),
				0.97f,
				(ssvu::getRnd(0, 10) > 7) ? sf::Color::White : sf::Color::Green,
				0.7f + ssvu::getRndR<float>(-0.3f, 0.3f),
				45 + ssvu::getRnd(-15, 20),
				1.f,
				ssvu::getRndR(-4.5f, 4.5f));
	}
	inline void createPCaseBullet(OBParticleSystem& mPS, const Vec2f& mPos, float mDeg)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(mDeg + 90.f) * ssvu::getRndR<float>(1.f, 3.f),
				0.96f,
				sf::Color::Yellow,
				0.5f,
				150 + ssvu::getRnd(-15, 20),
				7.f,
				ssvu::getRndR(-1.5f, 1.5f));
	}
	inline void createPCaseRocket(OBParticleSystem& mPS, const Vec2f& mPos, float mDeg)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(mDeg + 90.f) * ssvu::getRndR<float>(1.f, 3.f),
				0.96f,
				sf::Color{55, 25, 85, 255},
				0.9f,
				150 + ssvu::getRnd(-15, 20),
				7.f,
				ssvu::getRndR(-1.5f, 1.5f));
	}
	inline void createPForceField(OBParticleSystem& mPS, const Vec2f& mPos)
	{
		mPS.emplace(mPos,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 5.4f)),
				0.91f,
				(ssvu::getRnd(0, 9) > 4) ? sf::Color::Red : sf::Color::Blue,
				0.5f + ssvu::getRndR<float>(-0.3f, 0.3f),
				35 + ssvu::getRnd(-25, 25),
				1.5f,
				ssvu::getRndR(-3.5f, 3.5f));
	}
}

#endif
