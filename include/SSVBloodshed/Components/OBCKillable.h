// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_KILLABLE
#define SSVOB_COMPONENTS_KILLABLE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCKillable : public OBCActorNoDrawBase
	{
		public:
			enum class Type{Organic, Robotic};

		private:
			OBCHealth& cHealth;
			Type type{Type::Organic};
			int particleMult{1};

			inline void effectHit()
			{
				switch(type)
				{
					case Type::Organic:
						game.createPBlood(6 * particleMult, cPhys.getPosPixels());
						assets.playSound("Sounds/bulletHitFlesh.wav");
						break;
					case Type::Robotic:
						game.createPDebris(6 * particleMult, cPhys.getPosPixels());
						game.createPElectric(24 * particleMult, cPhys.getPosPixels());
						assets.playSound("Sounds/spark.wav");
						break;
				}
			}

			inline void effectDeath()
			{
				switch(type)
				{
					case Type::Organic:
						game.createPBlood(20 * particleMult * particleMult, cPhys.getPosPixels(), particleMult);
						game.createPGib(35 * particleMult * particleMult, cPhys.getPosPixels());
						assets.playSound("Sounds/squish.wav");
						break;
					case Type::Robotic:
						game.createPDebris(24 * particleMult, cPhys.getPosPixels());
						game.createPElectric(64 * particleMult, cPhys.getPosPixels());
						assets.playSound("Sounds/spark.wav");
						break;
				}
			}

		public:
			ssvu::Delegate<void()> onDeath;

			OBCKillable(OBCPhys& mCPhys, OBCHealth& mCHealth, Type mType) : OBCActorNoDrawBase{mCPhys}, cHealth(mCHealth), type{mType} { }

			inline void init() override
			{
				cHealth.onDamage += [this]
				{
					effectHit();
					if(cHealth.isDead()) { effectDeath(); onDeath(); getEntity().destroy(); }
				};
			}

			inline void setType(Type mType) noexcept		{ type = mType; }
			inline void setParticleMult(int mMult) noexcept { particleMult = mMult; }

			inline OBCHealth& getCHealth() const noexcept	{ return cHealth; }
			inline Type getType() const noexcept			{ return type; }
			inline int getParticleMult() const noexcept		{ return particleMult; }
	};
}

#endif
