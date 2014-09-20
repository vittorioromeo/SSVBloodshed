// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PARTICLESYSTEM
#define SSVOB_COMPONENTS_PARTICLESYSTEM

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/Particles/OBParticleSystem.hpp"

namespace ob
{
	class OBCParticleSystem : public sses::Component
	{
		private:
			OBParticleSystem particleSystem;
			sf::RenderTexture& renderTexture;
			sf::RenderTarget& renderTarget;
			sf::Sprite sprite;
			sf::BlendMode blendMode;
			unsigned char alpha;
			bool clearOnDraw;

		public:
			OBCParticleSystem(sf::RenderTexture& mRenderTexture, sf::RenderTarget& mRenderTarget, bool mClearOnDraw, unsigned char mAlpha, sf::BlendMode mBlendMode) noexcept
				: renderTexture(mRenderTexture), renderTarget(mRenderTarget), blendMode{mBlendMode}, alpha{mAlpha}, clearOnDraw{mClearOnDraw} { }

			inline void init()
			{
				renderTexture.clear(sf::Color::Transparent);
				sprite.setTexture(renderTexture.getTexture());
				sprite.setColor({255, 255, 255, alpha});
			}
			inline void update(FT mFT) override { particleSystem.update(mFT); }
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
