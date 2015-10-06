// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLESYSTEM
#define SSVOB_PARTICLES_PARTICLESYSTEM

#include "SSVBloodshed/Particles/OBParticle.hpp"
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBConfig.hpp"

namespace ob
{
    // TODO: replace with multi-handlevector and benchmark
    class OBParticleSystem : public sf::Drawable
    {
    private:
        ssvs::VertexVector<sf::PrimitiveType::Quads> vertices;
        std::vector<OBParticle> particles;
        SizeT currentCount{0};

    public:
        inline OBParticleSystem()
        {
            vertices.resize(OBConfig::getParticleMax() * 4);
            particles.reserve(OBConfig::getParticleMax());
        }
        template <typename... TArgs>
        inline void emplace(TArgs&&... mArgs)
        {
            particles.emplace_back(FWD(mArgs)...);
        }
        inline void update(FT mFT)
        {
            // Remove excess particles
            if(particles.size() > OBConfig::getParticleMax())
                particles.erase(
                    std::begin(particles) + OBConfig::getParticleMax(),
                    std::end(particles));

            ssvu::eraseRemoveIf(particles, [](const OBParticle& mParticle)
                {
                    return mParticle.life <= 0;
                });
            currentCount = particles.size();

            for(auto i(0u); i < currentCount; ++i)
            {
                auto& p(particles[i]);
                p.update(mFT);
                const auto& vIdx(i * 4);

                auto& vNW(vertices[vIdx + 0]);
                auto& vNE(vertices[vIdx + 1]);
                auto& vSE(vertices[vIdx + 2]);
                auto& vSW(vertices[vIdx + 3]);

                vNW.position = p.nw;
                vNE.position = p.ne;
                vSE.position = p.se;
                vSW.position = p.sw;

                vNW.color = vNE.color = vSE.color = vSW.color = p.color;
            }
        }
        inline void draw(sf::RenderTarget& mRenderTarget,
            sf::RenderStates mRenderStates) const override
        {
            mRenderTarget.draw(&vertices[0], currentCount * 4,
                sf::PrimitiveType::Quads, mRenderStates);
        }
        inline void clear() noexcept
        {
            particles.clear();
            currentCount = 0;
        }
    };
}

#endif
