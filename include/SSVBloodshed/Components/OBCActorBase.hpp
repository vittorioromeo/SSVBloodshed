// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ACTORBASE
#define SSVOB_COMPONENTS_ACTORBASE

#include "SSVBloodshed/OBCommon.hpp"
#include "SSVBloodshed/OBGame.hpp"
#include "SSVBloodshed/Components/OBCPhys.hpp"
#include "SSVBloodshed/Components/OBCDraw.hpp"

namespace ob
{
	class OBCActorND : public Component
	{
		protected:
			OBGame& game;
			OBCPhys& cPhys;
			OBAssets& assets;
			OBFactory& factory;
			sses::Manager& manager;
			Body& body;

		public:
			inline OBCActorND(Entity& mE, OBCPhys& mCPhys) noexcept : Component{mE}, game(mCPhys.getGame()), cPhys(mCPhys), assets(game.getAssets()),
				factory(game.getFactory()), manager(game.getManager()), body(cPhys.getBody()) { }

			inline OBGame& getGame() const noexcept		{ return game; }
			inline OBCPhys& getCPhys() const noexcept	{ return cPhys; }
	};

	class OBCActor : public OBCActorND
	{
		protected:
			OBCDraw& cDraw;

		public:
			inline OBCActor(Entity& mE, OBCPhys& mCPhys, OBCDraw& mCDraw) noexcept : OBCActorND{mE, mCPhys}, cDraw(mCDraw) { }
			inline OBCDraw& getCDraw() const noexcept { return cDraw; }
	};
}

#endif
