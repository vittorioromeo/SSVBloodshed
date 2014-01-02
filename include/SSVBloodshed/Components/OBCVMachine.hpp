// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_VMACHINE
#define SSVOB_COMPONENTS_VMACHINE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCPlayer.hpp"

namespace ob
{
	class OBCVMachine : public OBCActorBase
	{
		private:
			float healAmount{1};
			int shardCost{10};
			std::string msg{"[" + ssvu::toStr(shardCost) + "] Heal <" + ssvu::toStr(healAmount) + "> hp"};

		public:
			OBCVMachine(OBCPhys& mCPhys, OBCDraw& mCDraw) noexcept : OBCActorBase{mCPhys, mCDraw} { }

			inline void init() { body.setResolve(false); }

			inline void update(FT) override
			{
				for(auto& e : manager.getEntities(OBGroup::GPlayer))
				{
					auto& cPhys(e->getComponent<OBCPhys>());
					auto& cPlayer(e->getComponent<OBCPlayer>());

					if(ssvs::getDistEuclidean(cPhys.getPosI(), body.getPosition()) < 1300) cPlayer.setCurrentVM(this);
					else if(cPlayer.getCurrentVM() == this) cPlayer.setCurrentVM(nullptr);
				}
			}

			inline float getHealAmount() const noexcept			{ return healAmount; }
			inline int getShardCost() const noexcept			{ return shardCost; }
			inline const std::string& getMsg() const noexcept	{ return msg; }
	};

	inline void OBCPlayer::updateHUD()
	{
		// TODO:
		auto& cHealth(cKillable.getCHealth());
		game.testhp.setValue(cHealth.getHealth());
		game.testhp.setMaxValue(cHealth.getMaxHealth());
		game.txtShards.setString(ssvu::toStr(shards + currentShards));
		game.txtVM.setString(currentVM == nullptr ? weapons[currentWpn].name : currentVM->getMsg());
	}

	inline void OBCPlayer::setCurrentVM(OBCVMachine* mVMachine) { currentVM = mVMachine; }
	inline void OBCPlayer::useVM()
	{
		if(currentVM->getShardCost() > shards + currentShards) return;

		if(cKillable.getCHealth().heal(currentVM->getHealAmount()))
		{
			int toRemove{currentVM->getShardCost()};
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

