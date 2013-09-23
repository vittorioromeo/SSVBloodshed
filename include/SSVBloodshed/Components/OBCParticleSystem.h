// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PARTICLESYSTEM
#define SSVOB_COMPONENTS_PARTICLESYSTEM

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/Particles/OBParticleSystem.h"

namespace ob
{
	class OBCParticleSystem : public sses::Component
	{
		private:
			sf::RenderTexture& renderTexture;
			sf::RenderTarget& renderTarget;
			bool clearOnDraw{false};
			unsigned char alpha{255};
			ParticleSystem particleSystem;
			sf::Sprite sprite;

		public:
			OBCParticleSystem(sf::RenderTexture& mRenderTexture, sf::RenderTarget& mRenderTarget, bool mClearOnDraw = false, unsigned char mAlpha = 255)
				: renderTexture(mRenderTexture), renderTarget(mRenderTarget), clearOnDraw{mClearOnDraw}, alpha{mAlpha} { }

			inline void init() override
			{
				renderTexture.clear(sf::Color::Transparent);
				sprite.setTexture(renderTexture.getTexture());
				sprite.setColor({255, 255, 255, alpha});
			}
			inline void update(float mFrameTime) override { particleSystem.update(mFrameTime); }
			inline void draw() override
			{
				renderTexture.draw(particleSystem);
				renderTexture.display();
				renderTarget.draw(sprite);
				if(clearOnDraw) renderTexture.clear(sf::Color::Transparent);
			}

			inline ParticleSystem& getParticleSystem() noexcept				{ return particleSystem; }
			inline const ParticleSystem& getParticleSystem() const noexcept	{ return particleSystem; }
	};
}

#endif
