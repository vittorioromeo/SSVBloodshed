// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLETYPES
#define SSVOB_PARTICLES_PARTICLETYPES

#include "SSVBloodshed/Particles/OBParticleSystem.h"
#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	inline void createPBlood(OBParticleSystem& mPS, const Vec2f& mPosition, float mMult)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.f, 13.f * mMult)),
				0.9f,
				sf::Color{ssvu::getRnd<unsigned char>(185, 255), 0, 0, 255},
				1.f + ssvu::getRndR<float>(-0.3, 0.3),
				75 + ssvu::getRnd(-65, 65),
				0.42f);
	}
	inline void createPGib(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.1f, 13.f)),
				0.93f,
				sf::Color{ssvu::getRnd<unsigned char>(95, 170), 15, 15, 255},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				150 + ssvu::getRnd(-50, 50),
				1.5f);
	}
	inline void createPDebris(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 9.f)),
				0.9f,
				sf::Color::Black,
				1.f + ssvu::getRndR<float>(-0.3, 0.3),
				65 + ssvu::getRnd(-50, 50),
				0.8f);
	}
	inline void createPDebrisFloor(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.5f, 3.4f)),
				0.9f,
				sf::Color{179, 179, 179, 255},
				2.5f,
				65 + ssvu::getRnd(-50, 50),
				0.8f);
	}
	inline void createPMuzzle(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 4.5f)),
				0.9f,
				sf::Color{255, ssvu::getRnd<unsigned char>(95, 100), 15, 255},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				6 + ssvu::getRnd(-5, 5),
				1.5f);
	}
	inline void createPPlasma(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 1.5f)),
				0.95f,
				sf::Color{255, 15, ssvu::getRnd<unsigned char>(95, 100), 255},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				5 + ssvu::getRnd(-4, 4),
				1.5f);
	}
	inline void createPSmoke(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 1.5f)),
				0.96f,
				sf::Color{45, 45, 45, 225},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				5 + ssvu::getRnd(-4, 4),
				1.5f);
	}
	inline void createPElectric(OBParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDeg(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 7.4f)),
				0.94f,
				(ssvu::getRnd(0, 10) > 8) ? sf::Color::Cyan : sf::Color::Yellow,
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				65 + ssvu::getRnd(-25, 55),
				1.5f,
				ssvu::getRndR(-2.5f, 2.5f));
	}
	inline void createPCharge(OBParticleSystem& mPS, const Vec2f& mPosition, float mDist)
	{
		Vec2f pos{ssvs::getOrbitFromDeg(mPosition, ssvu::getRndR<float>(0.f, 360.f), mDist)};

		mPS.emplace(pos,
				ssvs::getNormalized(mPosition - pos) * (mDist / 15.f),
				0.97f,
				(ssvu::getRnd(0, 10) > 7) ? sf::Color::Yellow : sf::Color::Red,
				1.2f + ssvu::getRndR<float>(-0.3, 0.3),
				mDist + ssvu::getRnd(-10, 15),
				1.0f,
				ssvu::getRndR(-2.5f, 2.5f));
	}
}

#endif
