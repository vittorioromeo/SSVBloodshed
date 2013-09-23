// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME_PARTICLES
#define SSVOB_GAME_PARTICLES

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBFactory.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"

namespace ob
{
	class OBGParticles
	{
		private:
			sf::RenderTexture psPermTexture, psTempTexture;
			ParticleSystem* psPerm{nullptr};
			ParticleSystem* psTemp{nullptr};

		public:
			inline OBGParticles()
			{
				psPermTexture.create(320, 240);
				psTempTexture.create(320, 240);
			}

			inline void clear(OBFactory& mFactory)
			{
				psPerm = &mFactory.createParticleSystem(psPermTexture, false, 175, OBLayer::PParticlePerm).getComponent<OBCParticleSystem>().getParticleSystem();
				psTemp = &mFactory.createParticleSystem(psTempTexture, true, 255, OBLayer::PParticleTemp).getComponent<OBCParticleSystem>().getParticleSystem();
			}

			inline ParticleSystem& getPSPerm() noexcept { return *psPerm; }
			inline ParticleSystem& getPSTemp() noexcept { return *psTemp; }
	};
}

#endif
