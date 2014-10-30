// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_USABLE
#define SSVOB_COMPONENTS_USABLE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCActorBase.hpp"

namespace ob
{
	class OBCPlayer;

	class OBCUsable : public OBCActorND
	{
		private:
			std::string msg;

		public:
			ssvu::Delegate<void(OBCPlayer&)> onUse;

			OBCUsable(Entity& mE, OBCPhys& mCPhys) : OBCActorND{mE, mCPhys} { getEntity().addGroups(OBGroup::GUsable); }

			inline void setMsg(std::string mMsg) { msg = std::move(mMsg); }
			inline const std::string& getMsg() const noexcept { return msg; }
	};
}

#endif

