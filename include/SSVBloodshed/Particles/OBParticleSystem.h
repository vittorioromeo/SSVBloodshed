// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_PARTICLES_PARTICLESYSTEM
#define SSVOB_PARTICLES_PARTICLESYSTEM

#include "SSVBloodshed/Particles/OBParticle.h"
#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	template<sf::PrimitiveType TPrimitive> struct VertexVector : public std::vector<sf::Vertex>
	{
		using std::vector<sf::Vertex>::vector;
		inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const
		{
			mRenderTarget.draw(&this->operator [](0), this->size(), TPrimitive, mRenderStates);
		}
	};

	class OBParticleSystem : public sf::Drawable
	{
		private:
			unsigned int maxParticles{10000};
			VertexVector<sf::PrimitiveType::Quads> vertices;
			std::vector<OBParticle> particles;

		public:
			inline OBParticleSystem() { vertices.reserve(maxParticles * 4); particles.reserve(maxParticles); }
			template<typename... TArgs> inline void emplace(TArgs&&... mArgs) { if(particles.size() < maxParticles) particles.emplace_back(std::forward<TArgs>(mArgs)...); }
			inline void update(float mFT)
			{
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
			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override { vertices.draw(mRenderTarget, mRenderStates); }
			inline void clear() { vertices.clear(); particles.clear(); }

	};
}

#endif
