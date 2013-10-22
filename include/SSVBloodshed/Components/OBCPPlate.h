// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PPLATE
#define SSVOB_COMPONENTS_PPLATE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBWeightable.h"
#include "SSVBloodshed/Components/OBCTrail.h"

namespace ob
{
	inline void activateIdReceivers(OBCPhys& mCaller, int mId, IdAction mIdAction, sses::Manager& mManager)
	{
		auto color(sf::Color::Yellow);
		if(mIdAction == IdAction::Open) color = sf::Color::Green;
		else if(mIdAction == IdAction::Close) color = sf::Color::Red;

		for(auto& e : mManager.getEntities(OBGroup::GIdReceiver))
		{
			auto& cIdReceiver(e->getComponent<OBCIdReceiver>());
			if(cIdReceiver.getId() != mId) continue;

			cIdReceiver.activate(mIdAction);

			auto& cPhys(e->getComponent<OBCPhys>());
			mCaller.getGame().getFactory().createTrail(mCaller.getPosI(), cPhys.getPosI(), color);
		}
	}

	class OBCPPlate : public OBCActorBase, public OBWeightable
	{
		private:
			int id;
			PPlateType type;
			IdAction idAction;
			bool triggered{false};

			inline void triggerNeighbors(bool mTrigger)
			{
				auto query(cPhys.getWorld().getQuery<ssvsc::QueryType::Distance>(cPhys.getPosI(), 1000));

				Body* body;
				while((body = query.next()) != nullptr)
				{
					if(body->hasGroup(OBGroup::GPPlate))
					{
						auto& cPPlate(getEntityFromBody(*body).getComponent<OBCPPlate>());
						if(cPPlate.cPhys.getPosI().x == cPhys.getPosI().x || cPPlate.cPhys.getPosI().y == cPhys.getPosI().y)
							if(cPPlate.id == id && cPPlate.type == type)
							{
								if(mTrigger) cPPlate.trigger(); else cPPlate.unTrigger();
							}
					}
				}
			}

			inline void trigger()	{ if(!triggered) { triggered = true; triggerNeighbors(true); } }
			inline void unTrigger()	{ if(triggered) { triggered = false; triggerNeighbors(false); } }

		public:
			OBCPPlate(OBCPhys& mCPhys, OBCDraw& mCDraw, int mId, PPlateType mType, IdAction mIdAction, bool mPlayerOnly) noexcept
				: OBCActorBase{mCPhys, mCDraw}, OBWeightable{mCPhys, mPlayerOnly}, id{mId}, type{mType}, idAction{mIdAction} { }

			inline void init()
			{
				if(type == PPlateType::Single) triggered = false;
				OBWeightable::init();
			}
			inline void update(float) override
			{
				if(hasBeenWeighted() && !triggered)
				{
					trigger(); activateIdReceivers(cPhys, id, idAction, manager);
				}
				else if(hasBeenUnweighted())
				{
					if(type == PPlateType::Multi) unTrigger();
					else if(type == PPlateType::OnOff) { unTrigger(); activateIdReceivers(cPhys, id, idAction, manager); }
				}

				OBWeightable::refresh();
			}
			inline void draw() override { cDraw[0].setColor(triggered ? sf::Color(100, 100, 100, 255) : sf::Color::White); }

			inline void setId(int mId) noexcept	{ id = mId; }
			inline int getId() const noexcept	{ return id; }
	};
}

#endif

