// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_TRAPDOOR
#define SSVOB_COMPONENTS_TRAPDOOR

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCTrapdoor : public OBCActorBase
	{
		private:
			bool wasWeighted{false}, weighted{false}, falling{false};
			float fallTime{100.f};

		public:
			OBCTrapdoor(OBCPhys& mCPhys, OBCDraw& mCDraw) noexcept : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() override
			{
				body.setResolve(false);
				getEntity().addGroup(OBGroup::GTrapdoor);
				body.addGroup(OBGroup::GTrapdoor);
				body.addGroupsToCheck(OBGroup::GFriendly, OBGroup::GEnemy);

				body.onPreUpdate += [this]{ weighted = false; };
				body.onDetection += [this](DetectionInfo& mDI){ if(mDI.body.hasAnyGroup(OBGroup::GFriendly | OBGroup::GEnemy)) weighted = true; };
			}
			inline void update(float mFT) override
			{
				if(falling)
				{
					fallTime -= mFT * 2.5f;
					if(fallTime <= 0) getEntity().destroy();
					cDraw.setGlobalScale(Vec2f(fallTime / 100.f, fallTime / 100.f));
					return;
				}

				if(wasWeighted && !weighted)
				{
					assets.playSound("Sounds/spark.wav");
					getFactory().createPit(cPhys.getPosI());
					falling = true;
				}

				wasWeighted = weighted;
			}
	};
}

#endif

