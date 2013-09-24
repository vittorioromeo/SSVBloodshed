// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_HEALTH
#define SSVOB_COMPONENTS_HEALTH

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCHealth : public sses::Component
	{
		private:
			int health, maxHealth;
			ssvs::Ticker cooldown{1.f};

		public:
			ssvu::Delegate<void()> onDamage, onHeal;

			OBCHealth(int mHealth) : health{mHealth}, maxHealth{mHealth} { }

			inline void update(float mFrameTime) override { if(cooldown.update(mFrameTime)) cooldown.stop(); }

			inline void heal(int mHealth) noexcept { health += mHealth; ssvu::clampMax(health, maxHealth); onHeal(); }
			inline bool damage(int mDamage) noexcept
			{
				if(cooldown.isRunning()) return false;

				cooldown.restart();
				health = ssvu::getClampedMin(health - mDamage, 0);
				onDamage();
				return true;
			}
			inline void setHealth(int mHealth) noexcept		{ health = ssvu::getClamped(mHealth, 0, maxHealth); }
			inline void setMaxHealth(int mValue) noexcept	{ maxHealth = mValue; }
			inline void setCooldown(float mValue) noexcept	{ cooldown.restart(mValue); }

			inline bool isDead() const noexcept			{ return health <= 0; }
			inline int getHealth() const noexcept		{ return health; }
			inline int getMaxHealth() const noexcept	{ return maxHealth; }
			inline operator int() const noexcept		{ return health; }
	};
}

#endif
