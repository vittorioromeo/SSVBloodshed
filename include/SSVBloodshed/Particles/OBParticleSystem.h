// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLESYSTEM
#define SSVOB_PARTICLES_PARTICLESYSTEM

#include "SSVBloodshed/Particles/OBParticle.h"
#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class ParticleSystem : public sf::Drawable
	{
		private:
			unsigned int maxParticles{10000};
			std::vector<sf::Vertex> vertices; // TODO: wrap this in ssvs::VertexVector that supports emplacement
			std::vector<Particle> particles;

		public:
			inline ParticleSystem() { vertices.reserve(maxParticles * 4); particles.reserve(maxParticles); }
			template<typename... TArgs> inline void emplace(TArgs&&... mArgs)
			{
				if(particles.size() >= maxParticles) return;
				particles.emplace_back(std::forward<TArgs>(mArgs)...);
			}
			inline void update(float mFT)
			{
				ssvu::eraseRemoveIf(particles, [](const Particle& mParticle){ return mParticle.getLife() <= 0; });
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
			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override { mRenderTarget.draw(&vertices[0], vertices.size(), sf::PrimitiveType::Quads, mRenderStates); }
			inline void clear() { vertices.clear(); particles.clear(); }
	};
}

#endif
