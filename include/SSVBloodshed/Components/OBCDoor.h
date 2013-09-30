// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DOOR
#define SSVOB_COMPONENTS_DOOR

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCActorBase.h"
#include "SSVBloodshed/Components/OBCIdReceiver.h"

namespace ob
{
	class OBCDoor : public OBCActorBase
	{
		private:
			OBCIdReceiver& cIdReceiver;
			bool open{false};

			inline void setOpen(bool mOpen) noexcept
			{
				open = mOpen;
				if(open)	cPhys.getBody().delGroups(OBGroup::GSolidGround, OBGroup::GSolidAir);
				else		cPhys.getBody().addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir);
			}

		public:
			OBCDoor(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, bool mOpen = false) : OBCActorBase{mCPhys, mCDraw}, cIdReceiver(mCIdReceiver), open{mOpen} { }

			inline void init() override
			{
				setOpen(open);
				cIdReceiver.onActivate += [this](OBIdAction mIdAction)
				{
					switch(mIdAction)
					{
						case OBIdAction::Toggle: setOpen(!open); break;
						case OBIdAction::Open: setOpen(true); break;
						case OBIdAction::Close: setOpen(false); break;
					}
				};
			}
			inline void draw() override { cDraw[0].setColor(sf::Color(255, 255, 255, open ? 100 : 255)); }
	};
}

#endif

