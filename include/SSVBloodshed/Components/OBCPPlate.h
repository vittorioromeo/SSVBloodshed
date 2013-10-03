// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_PPLATE
#define SSVOB_COMPONENTS_PPLATE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"

namespace ob
{
	class OBCPPlate : public OBCActorBase
	{
		private:
			int id;
			PPlateType type;
			OBIdAction idAction;
			bool triggered{false}, wasWeighted{false}, weighted{false};

			inline void activate()
			{
				for(auto& e : getManager().getEntities(OBGroup::GIdReceiver))
				{
					auto& c(e->getComponent<OBCIdReceiver>());
					if(c.getId() == id) c.onActivate(idAction);
				}
			}

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
			OBCPPlate(OBCPhys& mCPhys, OBCDraw& mCDraw, int mId, PPlateType mType, OBIdAction mIdAction) : OBCActorBase{mCPhys, mCDraw}, id{mId}, type{mType}, idAction{mIdAction} { }

			inline void init() override
			{
				if(type == PPlateType::Single) triggered = false;

				body.setResolve(false);
				body.addGroup(OBGroup::GPPlate);
				body.addGroupsToCheck(OBGroup::GFriendly, OBGroup::GEnemy);

				body.onPreUpdate += [this]{ weighted = false; };
				body.onDetection += [this](DetectionInfo& mDI)
				{
					if(mDI.body.hasGroup(OBGroup::GFriendly) || mDI.body.hasGroup(OBGroup::GEnemy)) weighted = true;
				};
			}
			inline void update(float) override
			{
				if(!wasWeighted && weighted && !triggered)
				{
					if(type == PPlateType::Single || type == PPlateType::Multi) { trigger(); activate(); }
				}

				if(wasWeighted && !weighted)
				{
					if(type == PPlateType::Multi) unTrigger();
				}

				wasWeighted = weighted;
			}
			inline void draw() override { cDraw[0].setColor(triggered ? sf::Color(100, 100, 100, 255) : sf::Color::White); }

			inline void setId(int mId) noexcept	{ id = mId; }
			inline int getId() const noexcept	{ return id; }
	};
}

#endif

