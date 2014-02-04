// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_CESYSTEM_SYSTEMBASE
#define SSVOB_CESYSTEM_SYSTEMBASE

#include "SSVBloodshed/CESystem/Common.hpp"

namespace ssvces
{
	class Manager;
	class Entity;

	namespace Internal
	{
		class SystemBase : ssvu::NoCopy
		{
			friend bool matchesSystem(const TypeIdsBitset&, const SystemBase&) noexcept;
			friend class ssvces::Manager;

			private:
				TypeIdsBitset typeIdsReq, typeIdsNot;

			protected:
				inline SystemBase(TypeIdsBitset mTypeIdsReq) : typeIdsReq{std::move(mTypeIdsReq)} { }
				inline SystemBase(TypeIdsBitset mTypeIdsReq, TypeIdsBitset mTypeIdsNot) : typeIdsReq{std::move(mTypeIdsReq)}, typeIdsNot{std::move(mTypeIdsNot)} { }
				inline virtual ~SystemBase() noexcept { }

				virtual void registerEntity(Entity&) = 0;
				virtual void refresh() = 0;
		};
	}
}

#endif
