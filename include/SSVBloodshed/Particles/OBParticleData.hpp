// Copyright (c) 2013-2015 Vittorio Romeo
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
    using ColorRng = ssvu::Tpl<RngI, RngI, RngI, RngI>;

    std::vector<ColorRng> colorRngs;
    RngF angleRng{0.f, 0.f}, velRng{0.f, 0.f}, sizeRng{0.f, 0.f},
    lifeRng{0.f, 0.f}, curveSpdRng{0.f, 0.f}, fuzzinessRng{0.f, 0.f},
    accelRng{0.f, 0.f}, distRng{0.f, 0.f};
    float alphaMult;
};

inline int getRndRngI(const OBParticleData::RngI& mRng) noexcept
{
    return ssvu::getRndI(std::get<0>(mRng), std::get<1>(mRng));
}
inline float getRndRngF(const OBParticleData::RngF& mRng) noexcept
{
    return ssvu::getRndR(std::get<0>(mRng), std::get<1>(mRng));
}
inline sf::Color getColorFromRng(const OBParticleData::ColorRng& mRng) noexcept
{
    return sf::Color(getRndRngI(std::get<0>(mRng)),
    getRndRngI(std::get<1>(mRng)), getRndRngI(std::get<2>(mRng)),
    getRndRngI(std::get<3>(mRng)));
}
}

SSVJ_CNV_ARR(ob::OBParticleData, angleRng, velRng, sizeRng, lifeRng,
curveSpdRng, fuzzinessRng, accelRng, distRng, alphaMult, colorRngs)

#endif
