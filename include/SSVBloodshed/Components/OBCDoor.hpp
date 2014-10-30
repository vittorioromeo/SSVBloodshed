// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_DOOR
#define SSVOB_COMPONENTS_DOOR

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"
#include "SSVBloodshed/Components/OBCIdReceiver.hpp"

namespace ob
{
	class OBCDoorBase : public OBCActor
	{
		protected:
			bool openStatus{false};

			inline void setOpen(bool mOpen) noexcept
			{
				static sf::Color colors[]
				{
					sf::Color::White,
					sf::Color{255, 255, 255, 100}
				};

				openStatus = mOpen;
				cDraw[0].setColor(colors[openStatus]);
				body.setGroups(!openStatus, OBGroup::GSolidGround, OBGroup::GSolidAir);
			}

		public:
			OBCDoorBase(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, bool mOpen = false) noexcept : OBCActor{mE, mCPhys, mCDraw}, openStatus{mOpen} { setOpen(openStatus); }

			inline void toggle() noexcept	{ setOpen(!openStatus); }
			inline void open() noexcept		{ setOpen(true); }
			inline void close() noexcept	{ setOpen(false); }
	};

	class OBCDoor : public OBCDoorBase
	{
		private:
			OBCIdReceiver& cIdReceiver;

		public:
			OBCDoor(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, OBCIdReceiver& mCIdReceiver, bool mOpen = false) noexcept : OBCDoorBase{mE, mCPhys, mCDraw, mOpen}, cIdReceiver(mCIdReceiver)
			{
				cIdReceiver.onActivate += [this](IdAction mIdAction)
				{
					if(mIdAction == IdAction::Toggle) toggle();
					else if(mIdAction == IdAction::Open) open();
					else if(mIdAction == IdAction::Close) close();
				};
			}
	};

	class OBCDoorG : public OBCDoorBase
	{
		private:
			bool triggered{false};

		public:
			OBCDoorG(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, bool mOpen = false) noexcept : OBCDoorBase{mE, mCPhys, mCDraw, mOpen} { }
			inline void update(FT) override { if(!triggered && game.isLevelClear()) { toggle(); triggered = true; } }
	};

	class OBCDoorR : public OBCDoorBase
	{
		private:
			bool triggered{false};

		public:
			OBCDoorR(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw, bool mOpen = false) noexcept : OBCDoorBase{mE, mCPhys, mCDraw, mOpen} { }
			inline void update(FT) override { if(!triggered && manager.getEntityCount(OBGroup::GTrapdoor) <= 0) { toggle(); triggered = true; } }
	};
}

#endif

