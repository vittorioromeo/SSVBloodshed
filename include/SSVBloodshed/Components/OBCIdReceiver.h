// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_IDRECEIVER
#define SSVOB_COMPONENTS_IDRECEIVER

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"

namespace ob
{
	class OBCIdReceiver : public sses::Component
	{
		private:
			int id;

		public:
			ssvu::Delegate<void(OBIdAction)> onActivate;

			inline OBCIdReceiver(int mId) : id{mId} { }
			inline void init() override { getEntity().addGroups(OBGroup::GIdReceiver); }

			inline void activate(OBIdAction mAction) { if(id != -1) onActivate(mAction); }

			inline void setId(int mId) noexcept	{ id = mId; }
			inline int getId() const noexcept	{ return id; }
	};
}

#endif

