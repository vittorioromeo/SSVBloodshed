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
	class OBCDoorBase : public OBCActorBase
	{
		protected:
			bool openStatus{false};

			inline void setOpen(bool mOpen) noexcept
			{
				openStatus = mOpen;
				if(openStatus)	body.delGroups(OBGroup::GSolidGround, OBGroup::GSolidAir);
				else			body.addGroups(OBGroup::GSolidGround, OBGroup::GSolidAir);
			}

		public:
			OBCDoorBase(OBCPhys& mCPhys, OBCDraw& mCDraw, bool mOpen = false) noexcept : OBCActorBase{mCPhys, mCDraw}, openStatus{mOpen} { }

			inline void init() override { setOpen(openStatus); }
			inline void draw() override { cDraw[0].setColor(sf::Color(255, 255, 255, openStatus ? 100 : 255)); }

			inline void toggle() noexcept	{ setOpen(!openStatus); }
			inline void open() noexcept		{ setOpen(true); }
			inline void close() noexcept	{ setOpen(false); }
	};

	class OBCDoor : public OBCDoorBase
	{
		private:
			OBCIdReceiver& cIdReceiver;

		public:
			OBCDoor(OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, bool mOpen = false) noexcept : OBCDoorBase{mCPhys, mCDraw, mOpen}, cIdReceiver(mCIdReceiver)
			{
				cIdReceiver.onActivate += [this](OBIdAction mIdAction)
				{
					switch(mIdAction)
					{
						case OBIdAction::Toggle: toggle(); break;
						case OBIdAction::Open: open(); break;
						case OBIdAction::Close: close(); break;
					}
				};
			}
	};

	class OBCDoorG : public OBCDoorBase
	{
		private:
			bool triggered{false};

		public:
			OBCDoorG(OBCPhys& mCPhys, OBCDraw& mCDraw, bool mOpen = false) noexcept : OBCDoorBase{mCPhys, mCDraw, mOpen} { }
			inline void update(float) override { if(!triggered && game.isLevelClear()) { toggle(); triggered = true; } }
	};

	class OBCDoorR : public OBCDoorBase
	{
		private:
			bool triggered{false};

		public:
			OBCDoorR(OBCPhys& mCPhys, OBCDraw& mCDraw, bool mOpen = false) noexcept : OBCDoorBase{mCPhys, mCDraw, mOpen} { }
			inline void update(float) override { if(!triggered && getManager().getEntityCount(OBGroup::GTrapdoor) <= 0) { toggle(); triggered = true; } }
	};
}

#endif

