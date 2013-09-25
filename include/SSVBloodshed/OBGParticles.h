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
			constexpr static unsigned int txWidth{320};
			constexpr static unsigned int txHeight{320};
			sf::RenderTexture txPSPerm, txPSTemp;
			ParticleSystem* psPerm{nullptr};
			ParticleSystem* psTemp{nullptr};

		public:
			inline OBGParticles()
			{
				txPSPerm.create(txWidth, txHeight);
				txPSTemp.create(txWidth, txHeight);
			}

			inline void clear(OBFactory& mFactory)
			{
				psPerm = &mFactory.createParticleSystem(txPSPerm, false, 175, OBLayer::LPSPerm).getComponent<OBCParticleSystem>().getParticleSystem();
				psTemp = &mFactory.createParticleSystem(txPSTemp, true, 255, OBLayer::LPSTemp).getComponent<OBCParticleSystem>().getParticleSystem();
			}

			inline ParticleSystem& getPSPerm() noexcept	{ return *psPerm; }
			inline ParticleSystem& getPSTemp() noexcept	{ return *psTemp; }
	};
}

#endif
