// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_GAME_PARTICLES
#define SSVOB_GAME_PARTICLES

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBFactory.hpp"
#include "SSVBloodshed/Components/OBCParticleSystem.hpp"

namespace ob
{
	class OBGParticles
	{
		private:
			constexpr static unsigned int txWidth{levelWidthPx};
			constexpr static unsigned int txHeight{levelHeightPx};
			sf::RenderTexture txPSPerm, txPSTemp;
			OBParticleSystem* psPerm{nullptr};
			OBParticleSystem* psTemp{nullptr};
			OBParticleSystem* psTempAdd{nullptr};

		public:
			inline OBGParticles()
			{
				txPSPerm.create(txWidth, txHeight);
				txPSTemp.create(txWidth, txHeight);
			}

			inline void clear(OBFactory& mFactory)
			{
				psPerm =	&mFactory.createParticleSystem(txPSPerm, false, 175, OBLayer::LPSPerm, sf::BlendAlpha).getComponent<OBCParticleSystem>().getParticleSystem();
				psTemp =	&mFactory.createParticleSystem(txPSTemp, true, 255, OBLayer::LPSTemp, sf::BlendAlpha).getComponent<OBCParticleSystem>().getParticleSystem();
				psTempAdd =	&mFactory.createParticleSystem(txPSTemp, true, 255, OBLayer::LPSTemp, sf::BlendAdd).getComponent<OBCParticleSystem>().getParticleSystem();
			}

			inline OBParticleSystem& getPSPerm() noexcept		{ return *psPerm; }
			inline OBParticleSystem& getPSTemp() noexcept		{ return *psTemp; }
			inline OBParticleSystem& getPSTempAdd() noexcept	{ return *psTempAdd; }
	};
}

#endif
