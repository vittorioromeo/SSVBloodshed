// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLESYSTEM
#define SSVOB_PARTICLES_PARTICLESYSTEM

#include "SSVBloodshed/Particles/OBParticle.hpp"
#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBConfig.hpp"

namespace ob
{
	class OBParticleSystem : public sf::Drawable
	{
		private:
			ssvs::VertexVector<sf::PrimitiveType::Quads> vertices;
			std::vector<OBParticle> particles;

		public:
			inline OBParticleSystem() { vertices.resize(OBConfig::getParticleMax() * 4); particles.reserve(OBConfig::getParticleMax()); }
			template<typename... TArgs> inline void emplace(TArgs&&... mArgs) { particles.emplace_back(std::forward<TArgs>(mArgs)...); }
			inline void update(FT mFT)
			{
				// Remove excess particles
				if(particles.size() > OBConfig::getParticleMax()) particles.erase(std::begin(particles) + OBConfig::getParticleMax(), std::end(particles));

				ssvu::eraseRemoveIf(particles, [](const OBParticle& mParticle){ return mParticle.getLife() <= 0; });
				for(auto i(0u); i < particles.size(); ++i)
				{
					auto& p(particles[i]); p.update(mFT);
					const auto& vIdx(i * 4);

					constexpr float fuzzyness{1.f};

					// TODO: add fuzzyness to particle type
					// TODO: store l/r/t/b in particle
					vertices[vIdx + 0].position = Vec2f{p.getPosition().x - p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness), p.getPosition().y - p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness)};
					vertices[vIdx + 1].position = Vec2f{p.getPosition().x - p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness), p.getPosition().y + p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness)};
					vertices[vIdx + 2].position = Vec2f{p.getPosition().x + p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness), p.getPosition().y - p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness)};
					vertices[vIdx + 3].position = Vec2f{p.getPosition().x + p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness), p.getPosition().y + p.getSize() + ssvu::getRndR(-fuzzyness, fuzzyness)};
					vertices[vIdx + 0].color = p.getColor();
					vertices[vIdx + 1].color = p.getColor();
					vertices[vIdx + 2].color = p.getColor();
					vertices[vIdx + 3].color = p.getColor();
				}
			}
			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override { mRenderTarget.draw(&vertices[0], particles.size() * 4, sf::PrimitiveType::Quads, mRenderStates); }
			inline void clear() { particles.clear(); }
	};
}

#endif
