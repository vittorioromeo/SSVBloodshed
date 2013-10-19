// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_TRAPDOOR
#define SSVOB_COMPONENTS_TRAPDOOR

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBWeightable.h"

namespace ob
{
	class OBCTrapdoor : public OBCActorBase, public OBWeightable
	{
		private:
			static constexpr float fallTimeMax{70.f};
			float fallTime{fallTimeMax};
			bool falling{false};

		public:
			OBCTrapdoor(OBCPhys& mCPhys, OBCDraw& mCDraw, bool mPlayerOnly) noexcept : OBCActorBase{mCPhys, mCDraw}, OBWeightable{mCPhys, mPlayerOnly} { }

			inline void init() override
			{
				getEntity().addGroups(OBGroup::GTrapdoor);
				body.addGroups(OBGroup::GTrapdoor);
				OBWeightable::init();
			}
			inline void update(float mFT) override
			{
				if(falling)
				{
					fallTime -= mFT * 2.5f;
					if(fallTime <= 0) getEntity().destroy();
					cDraw.setGlobalScale(fallTime / fallTimeMax);
					return;
				}

				if(hasBeenUnweighted())
				{
					assets.playSound("Sounds/spark.wav");
					factory.createPit(cPhys.getPosI());
					falling = true;
				}

				OBWeightable::refresh();
			}
	};
}

#endif

