// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_ENTITYHANDLE_INL
#define SSVOB_CESYSTEM_ENTITYHANDLE_INL

namespace ssvces
{
	inline bool EntityHandle::isAlive() const noexcept { return manager.entityIdPool.isAlive(stat); }
}

#endif
