// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_VMACHINE
#define SSVOB_COMPONENTS_VMACHINE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCUsable.hpp"
#include "SSVBloodshed/Components/OBCPlayer.hpp"

namespace ob
{
	class OBCVMachine : public OBCActorBase
	{
		private:
			OBCUsable& cUsable;
			float healAmount{1};
			int shardCost{10};

		public:
			OBCVMachine(OBCDraw& mCDraw, OBCUsable& mCUsable) noexcept : OBCActorBase{mCUsable.getCPhys(), mCDraw}, cUsable(mCUsable) { }

			inline void init()
			{
				body.setResolve(false);
				cUsable.setMsg("[" + ssvu::toStr(shardCost) + "] Heal <" + ssvu::toStr(healAmount) + "> hp");
				cUsable.onUse += [this](OBCPlayer& mPlayer){ mPlayer.useVM(*this); };
			}

			inline float getHealAmount() const noexcept	{ return healAmount; }
			inline int getShardCost() const noexcept	{ return shardCost; }
	};


	inline void OBCPlayer::useVM(OBCVMachine& mVMachine)
	{
		if(mVMachine.getShardCost() > shards + currentShards) return;

		if(cKillable.getCHealth().heal(mVMachine.getHealAmount()))
		{
			int toRemove{mVMachine.getShardCost()};
			int fromCurrent(currentShards - toRemove);

			if(fromCurrent > 0) currentShards -= toRemove;
			else
			{
				currentShards = 0;
				shards -= std::abs(toRemove);
			}

			game.createPHeal(15, cPhys.getPosPx());
		}
	}
}

#endif
