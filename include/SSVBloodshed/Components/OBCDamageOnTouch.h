// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DAMAGEONTOUCH
#define SSVOB_COMPONENTS_DAMAGEONTOUCH

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCDamageOnTouch : public OBCActorNoDrawBase
	{
		private:
			float damage;
			OBGroup targetGroup;

		public:
			OBCDamageOnTouch(OBCPhys& mCPhys, float mDamage, OBGroup mTargetGroup) noexcept : OBCActorNoDrawBase{mCPhys}, damage{mDamage}, targetGroup{mTargetGroup} { }

			inline void init() override
			{
				body.addGroupsToCheck(targetGroup);
				body.onDetection += [this](const DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(targetGroup)) getEntityFromBody(mDI.body).getComponent<OBCHealth>().damage(damage);
				};
			}

			inline void setDamage(float mValue) noexcept		{ damage = mValue; }
			inline void setTargetGroup(OBGroup mValue) noexcept	{ targetGroup = mValue; }

			inline float getDamage() const noexcept				{ return damage; }
			inline OBGroup getTargetGroup() const noexcept		{ return targetGroup; }
	};
}

#endif

