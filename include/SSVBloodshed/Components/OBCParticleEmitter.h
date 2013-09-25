// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PARTICLEEMITTER
#define SSVOB_COMPONENTS_PARTICLEEMITTER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCParticleSystem.h"

namespace ob
{
	class OBCParticleEmitter : public OBCActorNoDrawBase
	{
		public:
			enum class Type{Smoke, Plasma};

		private:
			Vec2f offset;
			Type type;

		public:
			OBCParticleEmitter(OBCPhys& mCPhys, Type mType) : OBCActorNoDrawBase{mCPhys}, type{mType} { }

			inline void update(float) override
			{
				switch(type)
				{
					case Type::Smoke: game.createPSmoke(1, cPhys.getPosPixels() + offset); break;
					case Type::Plasma: game.createPPlasma(1, cPhys.getPosPixels() + offset); break;
				}
			}

			inline void setOffset(const Vec2f& mOffset) noexcept { offset = mOffset; }
	};
}

#endif
