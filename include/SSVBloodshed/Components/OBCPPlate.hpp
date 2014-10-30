// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PPLATE
#define SSVOB_COMPONENTS_PPLATE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBWeightable.hpp"
#include "SSVBloodshed/Components/OBCTrail.hpp"

namespace ob
{
	inline void activateIdReceivers(OBCPhys& mCaller, int mId, IdAction mIdAction, sses::Manager& mManager)
	{
		static sf::Color actionColors[]{sf::Color::Yellow, sf::Color::Green, sf::Color::Red};

		for(auto& e : mManager.getEntities(OBGroup::GIdReceiver))
		{
			auto& cIdReceiver(e->getComponent<OBCIdReceiver>());
			if(cIdReceiver.getId() != mId) continue;

			cIdReceiver.activate(mIdAction);
			mCaller.getFactory().createTrail(mCaller.getPosI(), e->getComponent<OBCPhys>().getPosI(), actionColors[int(mIdAction)]);
		}
	}

	class OBCPPlate : public OBCActor, public OBWeightable
	{
		private:
			int id;
			PPlateType type;
			IdAction idAction;
			bool triggered{false};

			inline std::vector<OBCPPlate*> getNeighbors()
			{
				std::vector<OBCPPlate*> result;
				auto query(cPhys.getWorld().getQuery<ssvsc::QueryType::Distance>(cPhys.getPosI(), 1000));

				Body* b;
				while((b = query.next()) != nullptr)
				{
					if(!b->hasGroup(OBGroup::GPPlate)) continue;

					auto& cPPlate(getEntityFromBody(*b).getComponent<OBCPPlate>());
					if(cPPlate.cPhys.getPosI().x == cPhys.getPosI().x || cPPlate.cPhys.getPosI().y == cPhys.getPosI().y)
						if(cPPlate.id == id && cPPlate.type == type) result.emplace_back(&cPPlate);
				}

				return result;
			}

			inline void triggerNeighbors(bool mTrigger)
			{
				for(const auto& n : getNeighbors()) if(mTrigger) n->trigger(); else n->unTrigger();
			}

			inline bool isAnyNeighborWeighted()
			{
				for(const auto& n : getNeighbors()) if(n->isWeighted()) return true;
				return false;
			}

			inline void trigger()	{ if(!triggered) { triggered = true; triggerNeighbors(true); cDraw[0].setColor(sf::Color(100, 100, 100, 255)); } }
			inline void unTrigger()	{ if(triggered) { triggered = false; triggerNeighbors(false); cDraw[0].setColor(sf::Color::White); } }

		public:
			OBCPPlate(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, int mId, PPlateType mType, IdAction mIdAction, bool mPlayerOnly) noexcept
				: OBCActor{mE, mCPhys, mCDraw}, OBWeightable{mCPhys, mPlayerOnly}, id{mId}, type{mType}, idAction{mIdAction}
			{
				body.addGroups(OBGroup::GPPlate);
			}

			inline void update(FT) override
			{
				if(hasBeenWeighted() && !triggered)
				{
					trigger(); activateIdReceivers(cPhys, id, idAction, manager);
				}
				else if(hasBeenUnweighted() && !isAnyNeighborWeighted())
				{
					if(type == PPlateType::Multi) unTrigger();
					else if(type == PPlateType::OnOff) { unTrigger(); activateIdReceivers(cPhys, id, idAction, manager); }
				}

				OBWeightable::refresh();
			}
			inline void draw() override {  }

			inline void setId(int mId) noexcept	{ id = mId; }
			inline int getId() const noexcept	{ return id; }
	};
}

#endif

