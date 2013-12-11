// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLEDATA
#define SSVOB_PARTICLES_PARTICLEDATA

#include "SSVBloodshed/OBCommon.hpp"

namespace ob
{
	struct OBParticleData
	{
		using RngI = std::pair<int, int>;
		using RngF = std::pair<float, float>;
		using ColorRange = std::tuple<RngI, RngI, RngI, RngI>;

		RngF angleRng{0.f, 0.f}, velRng{0.f, 0.f}, sizeRng{0.f, 0.f}, lifeRng{0.f, 0.f}, curveSpdRng{0.f, 0.f}, fuzzinessRng{0.f, 0.f}, accelRng{0.f, 0.f}, distRng{0.f, 0.f};
		float alphaMult;
		std::vector<ColorRange> colorRngs;
	};

	inline int getRndRngI(const OBParticleData::RngI& mRng) noexcept	{ return ssvu::getRnd(std::get<0>(mRng), std::get<1>(mRng)); }
	inline float getRndRngF(const OBParticleData::RngF& mRng) noexcept	{ return ssvu::getRndR(std::get<0>(mRng), std::get<1>(mRng)); }
	inline sf::Color getColorFromRng(const OBParticleData::ColorRange& mRng) noexcept
	{
		return sf::Color(getRndRngI(std::get<0>(mRng)), getRndRngI(std::get<1>(mRng)), getRndRngI(std::get<2>(mRng)), getRndRngI(std::get<3>(mRng)));
	}
}

namespace ssvuj
{
	template<> struct Converter<ob::OBParticleData>
	{
		using T = ob::OBParticleData;
		inline static void fromObj(T& mValue, const Obj& mObj)	{ extrArray(mObj, mValue.angleRng, mValue.velRng, mValue.sizeRng, mValue.lifeRng, mValue.curveSpdRng,
																			mValue.fuzzinessRng, mValue.accelRng, mValue.distRng, mValue.alphaMult, mValue.colorRngs); }
		inline static void toObj(Obj& mObj, const T& mValue)	{ archArray(mObj, mValue.angleRng, mValue.velRng, mValue.sizeRng, mValue.lifeRng, mValue.curveSpdRng,
																			mValue.fuzzinessRng, mValue.accelRng, mValue.distRng, mValue.alphaMult, mValue.colorRngs); }
	};
}

#endif
