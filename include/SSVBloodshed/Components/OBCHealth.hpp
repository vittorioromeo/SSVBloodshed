// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_HEALTH
#define SSVOB_COMPONENTS_HEALTH

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"

namespace ob
{
	class OBCHealth : public Component
	{
		private:
			float health, maxHealth;
			Ticker tckCooldown{1.f};

		public:
			ssvu::Delegate<void(OBCActorND*)> onDamage;
			ssvu::Delegate<void()> onHeal;

			OBCHealth(Entity& mE, float mHealth) noexcept : Component{mE}, health{mHealth}, maxHealth{mHealth} { tckCooldown.setLoop(false); }

			inline void update(FT mFT) override { tckCooldown.update(mFT); }

			inline bool heal(float mAmount) noexcept
			{
				if(health >= maxHealth || isDead()) return false;
				health = ssvu::getClampedMax(health + mAmount, maxHealth);
				onHeal(); return true;
			}
			bool damage(sses::EntityStat mAttackerStat, OBCActorND* mAttacker, float mAmount) noexcept;
			inline void setHealth(float mHealth) noexcept	{ health = ssvu::getClamped(mHealth, 0.f, maxHealth); }
			inline void setMaxHealth(float mValue) noexcept	{ maxHealth = mValue; }
			inline void setCooldown(FT mValue) noexcept		{ tckCooldown.restart(mValue); }

			inline bool isDead() const noexcept			{ return health <= 0.f; }
			inline int getHealth() const noexcept		{ return health; }
			inline int getMaxHealth() const noexcept	{ return maxHealth; }
	};
}

#endif
