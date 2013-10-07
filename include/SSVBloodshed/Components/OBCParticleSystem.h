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
			bool clearOnDraw;
			unsigned char alpha;
			OBParticleSystem particleSystem;
			sf::Sprite sprite;
			sf::BlendMode blendMode;

		public:
			OBCParticleSystem(sf::RenderTexture& mRenderTexture, sf::RenderTarget& mRenderTarget, bool mClearOnDraw = false, unsigned char mAlpha = 255, sf::BlendMode mBlendMode = sf::BlendMode::BlendAlpha) noexcept
				: renderTexture(mRenderTexture), renderTarget(mRenderTarget), clearOnDraw{mClearOnDraw}, alpha{mAlpha}, blendMode{mBlendMode} { }

			inline void init() override
			{
				renderTexture.clear(sf::Color::Transparent);
				sprite.setTexture(renderTexture.getTexture());
				sprite.setColor({255, 255, 255, alpha});
			}
			inline void update(float mFT) override { particleSystem.update(mFT); }
			inline void draw() override
			{
				renderTexture.draw(particleSystem);
				renderTexture.display();
				renderTarget.draw(sprite, blendMode);
				if(clearOnDraw) renderTexture.clear(sf::Color::Transparent);
			}

			inline void setBlendMode(sf::BlendMode mMode) noexcept				{ blendMode = mMode; }

			inline sf::BlendMode getBlendMode() const noexcept					{ return blendMode; }
			inline OBParticleSystem& getParticleSystem() noexcept				{ return particleSystem; }
			inline const OBParticleSystem& getParticleSystem() const noexcept	{ return particleSystem; }
	};
}

#endif
