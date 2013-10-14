// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_VMACHINE
#define SSVOB_COMPONENTS_VMACHINE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCPlayer.h"

namespace ob
{
	class OBCVMachine : public OBCActorBase
	{
		private:
			float healAmount{1};
			int shardCost{10};

		public:
			OBCVMachine(OBCPhys& mCPhys, OBCDraw& mCDraw) noexcept : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() override
			{
				body.setResolve(false);
			}

			inline void update(float) override
			{
				for(auto& e : getManager().getEntities(OBGroup::GPlayer))
				{
					if(!e->hasComponent<OBCPhys>()) throw;
					if(!e->hasComponent<OBCPlayer>()) throw;

					auto& cPhys(e->getComponent<OBCPhys>());
					auto& cPlayer(e->getComponent<OBCPlayer>());
					if(ssvs::getDistEuclidean(cPhys.getPosPx(), toPixels(body.getPosition())) < 13)
					{
						cPlayer.setCurrentVM(this);
					}
					else if(cPlayer.getCurrentVM() == this)
					{
						cPlayer.setCurrentVM(nullptr);
					}
				}
			}

			inline float getHealAmount() const noexcept		{ return healAmount; }
			inline int getShardCost() const noexcept		{ return shardCost; }
			inline std::string getMsg() const noexcept		{ return "[" + ssvu::toStr(shardCost) + "] Heal <" + ssvu::toStr(healAmount) + "> hp"; }
	};

	inline void OBCPlayer::updateVM()
	{
		if(currentVM == nullptr) { game.txtVM.setString(""); return; }
		game.txtVM.setString(currentVM->getMsg());
	}

	inline void OBCPlayer::useVM()
	{
		if(currentVM->getShardCost() > shards + currentShards) return;

		if(cKillable.getCHealth().heal(currentVM->getHealAmount()))
		{
			int toRemove{currentVM->getShardCost()};
			int fromCurrent(currentShards - toRemove);
			if(fromCurrent > 0)
			{
				currentShards -= toRemove;
			}
			else
			{
				currentShards = 0;
				shards -= std::abs(toRemove);
			}
		}
	}
}

#endif

