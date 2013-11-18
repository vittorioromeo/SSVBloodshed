// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_IDRECEIVER
#define SSVOB_COMPONENTS_IDRECEIVER

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"

namespace ob
{
	class OBCIdReceiver : public sses::Component
	{
		private:
			int id;

		public:
			ssvu::Delegate<void(IdAction)> onActivate;

			inline OBCIdReceiver(int mId) : id{mId} { }
			inline void init() { getEntity().addGroups(OBGroup::GIdReceiver); }

			inline void activate(IdAction mAction) { if(id != -1) onActivate(mAction); }

			inline void setId(int mId) noexcept	{ id = mId; }
			inline int getId() const noexcept	{ return id; }
	};

	inline void controlBoolByIdAction(OBCIdReceiver& mIdReceiver, bool& mValue) noexcept
	{
		mIdReceiver.onActivate += [&mValue](IdAction mIdAction)
		{
			switch(mIdAction)
			{
				case IdAction::Toggle:	mValue = !mValue;	return;
				case IdAction::Open:	mValue = true;		return;
				case IdAction::Close:	mValue = false;		return;
			}
		};
	}
}

#endif

