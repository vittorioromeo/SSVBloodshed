// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PARTICLEEMITTER
#define SSVOB_COMPONENTS_PARTICLEEMITTER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"

namespace ob
{
	class OBCParticleEmitter : public sses::Component
	{
		public:
			enum class Type{Smoke, Plasma};

		private:
			OBGame& game;
			OBCPhys& cPhys;
			OBAssets& assets;
			ssvsc::Body& body;
			Vec2f offset;
			Type type;

		public:
			OBCParticleEmitter(OBGame& mGame, OBCPhys& mCPhys, Type mType) : game(mGame), cPhys(mCPhys), assets(game.getAssets()), body(cPhys.getBody()), type{mType} { }

			inline void update(float) override
			{
				switch(type)
				{
					case Type::Smoke: game.createPSmoke(1, toPixels(body.getPosition()) + offset); break;
					case Type::Plasma: game.createPPlasma(1, toPixels(body.getPosition()) + offset); break;
				}
			}

			inline void setOffset(const Vec2f& mOffset) noexcept { offset = mOffset; }
	};
}

#endif
