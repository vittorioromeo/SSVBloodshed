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
			unsigned int count;

		public:
			OBCParticleEmitter(OBCPhys& mCPhys, Type mType, unsigned int mCount = 1) : OBCActorNoDrawBase{mCPhys}, type{mType}, count{mCount} { }

			inline void update(float) override
			{
				switch(type)
				{
					case Type::Smoke: game.createPSmoke(count, cPhys.getPosPixels() + offset); break;
					case Type::Plasma: game.createPPlasma(count, cPhys.getPosPixels() + offset); break;
				}
			}

			inline void setOffset(const Vec2f& mOffset) noexcept { offset = mOffset; }
	};
}

#endif
