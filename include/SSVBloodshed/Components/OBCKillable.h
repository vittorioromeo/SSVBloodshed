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
			enum class Type{Organic, Robotic, Wall, ExplosiveCrate};

		private:
			OBCHealth& cHealth;
			Type type{Type::Organic};
			float particleMult{1.f};

			inline void effectHit()
			{
				switch(type)
				{
					case Type::Organic:
						game.createPBlood(5 * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/bulletHitFlesh.wav");
						break;
					case Type::Robotic:
						game.createPDebris(5 * particleMult, cPhys.getPosPx());
						game.createPElectric(22 * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/spark.wav");
						break;
					case Type::Wall:
					case Type::ExplosiveCrate:
						game.createPDebris(5 * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/spark.wav");
						break;
				}
			}

			inline void effectDeath()
			{
				switch(type)
				{
					case Type::Organic:
						game.createPBlood(20 * particleMult * particleMult, cPhys.getPosPx(), particleMult);
						game.createPGib(35 * particleMult * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/squish.wav");
						break;
					case Type::Robotic:
						game.createPDebris(24 * particleMult, cPhys.getPosPx());
						game.createPElectric(64 * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/spark.wav");
						break;
					case Type::Wall:
						game.createPDebrisFloor(4 * particleMult, cPhys.getPosPx());
						game.createPDebris(36 * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/spark.wav");
						break;
					case Type::ExplosiveCrate:
						game.createPDebrisFloor(3 * particleMult, cPhys.getPosPx());
						game.createPDebris(8 * particleMult, cPhys.getPosPx());
						game.createPExplosion(36 * particleMult, cPhys.getPosPx());
						assets.playSound("Sounds/spark.wav");
						break;
				}
			}

		public:
			ssvu::Delegate<void()> onDeath;

			OBCKillable(OBCPhys& mCPhys, OBCHealth& mCHealth, Type mType) noexcept : OBCActorNoDrawBase{mCPhys}, cHealth(mCHealth), type{mType} { }

			inline void init() { cHealth.onDamage += [this]{ effectHit(); if(cHealth.isDead()) kill(); }; }

			inline void kill() { effectDeath(); onDeath(); getEntity().destroy(); }

			inline void setType(Type mType) noexcept			{ type = mType; }
			inline void setParticleMult(float mMult) noexcept	{ particleMult = mMult; }

			inline OBCHealth& getCHealth() const noexcept	{ return cHealth; }
			inline Type getType() const noexcept			{ return type; }
			inline int getParticleMult() const noexcept		{ return particleMult; }
	};
}

#endif
