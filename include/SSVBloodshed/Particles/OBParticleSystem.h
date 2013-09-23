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
			sf::VertexArray vertices{sf::PrimitiveType::Quads};
			std::vector<Particle> particles;

		public:
			inline ParticleSystem() { particles.reserve(maxParticles); }
			inline void emplace(const Vec2f& mPosition, const Vec2f& mVelocity, float mAcceleration, const sf::Color& mColor, float mSize, float mLife, float mAlphaMult = 1.f)
			{
				if(particles.size() >= maxParticles) return;
				particles.emplace_back(mPosition, mVelocity, mAcceleration, mColor, mSize, mLife, mAlphaMult);
			}
			inline void update(float mFrameTime)
			{
				ssvu::eraseRemoveIf(particles, [](const Particle& mParticle){ return mParticle.getLife() <= 0; });
				vertices.clear();
				for(auto& p : particles)
				{
					p.update(mFrameTime);
					vertices.append({{p.getPosition().x - p.getSize(), p.getPosition().y - p.getSize()}, p.getColor()});
					vertices.append({{p.getPosition().x + p.getSize(), p.getPosition().y - p.getSize()}, p.getColor()});
					vertices.append({{p.getPosition().x + p.getSize(), p.getPosition().y + p.getSize()}, p.getColor()});
					vertices.append({{p.getPosition().x - p.getSize(), p.getPosition().y + p.getSize()}, p.getColor()});
				}
			}
			inline void draw(sf::RenderTarget& mRenderTarget, sf::RenderStates mRenderStates) const override { mRenderTarget.draw(vertices, mRenderStates); }
			inline void clear() { particles.clear(); }
	};
}

#endif
