// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef SSVOB_COMPONENTS_ACTORBASE
#define SSVOB_COMPONENTS_ACTORBASE

#include "SSVBloodshed/OBCommon.h"
#include "SSVBloodshed/OBGame.h"
#include "SSVBloodshed/Components/OBCPhys.h"
#include "SSVBloodshed/Components/OBCDraw.h"

namespace ob
{
	class OBCActorNoDrawBase : public sses::Component
	{
		protected:
			OBGame& game;
			OBCPhys& cPhys;
			OBAssets& assets;
			ssvsc::Body& body;

		public:
			inline OBCActorNoDrawBase(OBCPhys& mCPhys) : game(mCPhys.getGame()), cPhys(mCPhys), assets(game.getAssets()), body(cPhys.getBody()) { }
			inline OBGame& getGame() const noexcept			{ return game; }
			inline OBFactory& getFactory() const noexcept	{ return game.getFactory(); }
			inline OBCPhys& getCPhys() const noexcept		{ return cPhys; }
	};

	class OBCActorBase : public OBCActorNoDrawBase
	{
		protected:
			OBCDraw& cDraw;

		public:
			inline OBCActorBase(OBCPhys& mCPhys, OBCDraw& mCDraw) : OBCActorNoDrawBase{mCPhys}, cDraw(mCDraw) { }
			inline OBCDraw& getCDraw() const noexcept { return cDraw; }
	};
}

#endif
