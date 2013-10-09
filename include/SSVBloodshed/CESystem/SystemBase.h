// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_SYSTEMBASE
#define SSVOB_CESYSTEM_SYSTEMBASE

#include "SSVBloodshed/CESystem/Common.h"

namespace ssvces
{
	class Entity;

	class SystemBase
	{
		friend class Manager;

		private:
			TypeIdsBitset typeIdsBitset;

		protected:
			inline SystemBase(TypeIdsBitset mTypeIdBitset) : typeIdsBitset{std::move(mTypeIdBitset)} { }

		public:
			virtual void registerEntity(Entity&) = 0;
			virtual void refresh() = 0;
	};
}

#endif
