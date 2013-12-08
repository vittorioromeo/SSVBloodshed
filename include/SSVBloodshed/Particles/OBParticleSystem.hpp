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
			inline OBParticleSystem() { vertices.reserve(OBConfig::getParticleMax() * 4); particles.reserve(OBConfig::getParticleMax()); }
			template<typename... TArgs> inline void emplace(TArgs&&... mArgs) { particles.emplace_back(std::forward<TArgs>(mArgs)...); }
			inline void update(FT mFT)
			{
				// Remove excess particles
				if(particles.size() > OBConfig::getParticleMax()) particles.erase(std::begin(particles) + OBConfig::getParticleMax(), std::end(particles));

				ssvu::eraseRemoveIf(particles, [](const OBParticle& mParticle){ return mParticle.getLife() <= 0; });
				vertices.clear();
				for(auto& p : particles)
				{
					p.update(mFT);
					vertices.emplace_back(Vec2f{p.getPosition().x - p.getSize(), p.getPosition().y - p.getSize()}, p.getColor());
					vertices.emplace_back(Vec2f{p.getPosition().x + p.getSize(), p.getPosition().y - p.getSize()}, p.getColor());
					vertices.emplace_back(Vec2f{p.getPosition().x + p.getSize(), p.getPosition().y + p.getSize()}, p.getColor());
					vertices.emplace_back(Vec2f{p.getPosition().x - p.getSize(), p.getPosition().y + p.getSize()}, p.getColor());
				}
			}
			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override { mRenderTarget.draw(vertices, mRenderStates); }
			inline void clear() { vertices.clear(); particles.clear(); }

	};
}

#endif
