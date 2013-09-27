// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPN
#define SSVOB_WEAPONS_WPN

#include "SSVBloodshed/OBCommon.h"

namespace ob
{
	class OBCWpnController;

	class OBWpn
	{
		protected:
			float delay{1000.f}, pjDamage{0.f}, pjSpeed{0.f};
			std::string soundId{""};

		public:
			ssvu::Delegate<void(OBCWpnController&, const Vec2i&, float)> onShoot;

			inline OBWpn() = default;

			inline OBWpn(float mDelay, float mDamage, float mPjSpeed, std::string mSoundId)
				: delay{mDelay}, pjDamage{mDamage}, pjSpeed{mPjSpeed}, soundId{std::move(mSoundId)} { }

			inline void setDelay(float mValue) noexcept				{ delay = mValue; }
			inline void setPjDamage(float mValue) noexcept			{ pjDamage = mValue; }
			inline void setPjSpeed(float mValue) noexcept			{ pjSpeed = mValue; }

			inline float getDelay() const noexcept					{ return delay; }
			inline float getPjDamage() const noexcept				{ return pjDamage; }
			inline float getPjSpeed() const noexcept				{ return pjSpeed; }
			inline const std::string& getSoundId() const noexcept	{ return soundId; }
	};
}

#endif
