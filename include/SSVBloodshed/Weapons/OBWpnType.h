// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_WEAPONS_WPNTYPE
#define SSVOB_WEAPONS_WPNTYPE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/Components/OBCProjectile.h"

namespace ob
{
	class OBGame;

	class OBWpnType
	{
		protected:
			float delay{1000.f}, pjDamage{0.f}, pjSpeed{0.f};
			std::string soundId{""};

		public:
			ssvu::Delegate<void(OBWpnType&, OBGame&, const Vec2i&, float)> onShoot;
			ssvu::Delegate<void(OBCProjectile&)> onShotProjectile;

			inline OBWpnType() = default;
			inline OBWpnType(float mDelay, float mDamage, float mPjSpeed, std::string mSoundId) noexcept : delay{mDelay}, pjDamage{mDamage}, pjSpeed{mPjSpeed}, soundId{std::move(mSoundId)} { }
			template<typename T> inline OBWpnType(float mDelay, float mDamage, float mPjSpeed, std::string mSoundId, T mOnShoot) noexcept : delay{mDelay}, pjDamage{mDamage}, pjSpeed{mPjSpeed}, soundId{std::move(mSoundId)}
			{
				onShoot += mOnShoot;
			}

			inline void shoot(OBGame& mGame, const Vec2i& mPos, float mDeg) { onShoot(*this, mGame, mPos, mDeg); }
			inline OBCProjectile& shotProjectile(Entity& mEntity)
			{
				auto& pj(mEntity.getComponent<OBCProjectile>());
				pj.setDamage(pjDamage); pj.setSpeed(pjSpeed);
				onShotProjectile(pj); return pj;
			}
			inline void playSound(OBGame& mGame) { mGame.getAssets().playSound(soundId); }

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
