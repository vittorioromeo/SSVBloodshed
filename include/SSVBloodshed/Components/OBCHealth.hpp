// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_HEALTH
#define SSVOB_COMPONENTS_HEALTH

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"

namespace ob
{
	class OBCHealth : public sses::Component
	{
		private:
			float health, maxHealth;
			ssvs::Ticker tckCooldown{1.f};

		public:
			ssvu::Delegate<void()> onDamage, onHeal;

			OBCHealth(float mHealth) noexcept : health{mHealth}, maxHealth{mHealth} { tckCooldown.setLoop(false); }

			inline void update(FT mFT) override { tckCooldown.update(mFT); }

			inline bool heal(float mAmount) noexcept
			{
				if(health >= maxHealth || isDead()) return false;
				health = ssvu::getClampedMax(health + mAmount, maxHealth);
				onHeal(); return true;
			}
			inline bool damage(float mAmount) noexcept
			{
				if(tckCooldown.isRunning() || isDead()) return false;
				health = ssvu::getClampedMin(health - mAmount, 0.f);
				tckCooldown.restart(); onDamage(); return true;
			}
			inline void setHealth(float mHealth) noexcept	{ health = ssvu::getClamped(mHealth, 0.f, maxHealth); }
			inline void setMaxHealth(float mValue) noexcept	{ maxHealth = mValue; }
			inline void setCooldown(FT mValue) noexcept	{ tckCooldown.restart(mValue); }

			inline bool isDead() const noexcept			{ return health <= 0.f; }
			inline int getHealth() const noexcept		{ return health; }
			inline int getMaxHealth() const noexcept	{ return maxHealth; }
	};
}

#endif
