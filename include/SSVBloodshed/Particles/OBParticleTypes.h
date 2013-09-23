// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLETYPES
#define SSVOB_PARTICLES_PARTICLETYPES

#include "SSVBloodshed/Particles/OBParticleSystem.h"
#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	inline void createPBlood(ParticleSystem& mPS, const Vec2f& mPosition, float mMult)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.f, 13.f * mMult)),
				0.9f,
				{ssvu::getRnd<unsigned char>(185, 255), 0, 0, 255},
				1.f + ssvu::getRndR<float>(-0.3, 0.3),
				75 + ssvu::getRnd(-65, 65),
				0.42f);
	}
	inline void createPGib(ParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.1f, 13.f)),
				0.93f,
				{ssvu::getRnd<unsigned char>(95, 170), 15, 15, 255},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				150 + ssvu::getRnd(-50, 50),
				1.5f);
	}
	inline void createPDebris(ParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 9.f)),
				0.9f,
				sf::Color::Black,
				1.f + ssvu::getRndR<float>(-0.3, 0.3),
				65 + ssvu::getRnd(-50, 50),
				0.8f);
	}
	inline void createPDebrisFloor(ParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.5f, 3.4f)),
				0.9f,
				{179, 179, 179, 255},
				2.5f,
				65 + ssvu::getRnd(-50, 50),
				0.8f);
	}
	inline void createPMuzzle(ParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(1.f, 4.5f)),
				0.9f,
				{255, ssvu::getRnd<unsigned char>(95, 100), 15, 255},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				6 + ssvu::getRnd(-5, 5),
				1.5f);
	}
	inline void createPPlasma(ParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 1.5f)),
				0.95f,
				{255, 15, ssvu::getRnd<unsigned char>(95, 100), 255},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				5 + ssvu::getRnd(-4, 4),
				1.5f);
	}
	inline void createPSmoke(ParticleSystem& mPS, const Vec2f& mPosition)
	{
		mPS.emplace(mPosition,
				ssvs::getVecFromDegrees(ssvu::getRndR<float>(0.f, 360.f), ssvu::getRndR<float>(0.3f, 1.5f)),
				0.96f,
				{45, 45, 45, 225},
				1.1f + ssvu::getRndR<float>(-0.3, 0.3),
				5 + ssvu::getRnd(-4, 4),
				1.5f);
	}
}

#endif
