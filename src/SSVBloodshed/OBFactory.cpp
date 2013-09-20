// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "SSVBloodshed/OBFactory.h"

#include "SSVBloodshed/Components/OBCPhysics.h"
#include "SSVBloodshed/Components/OBCRender.h"

using namespace std;
using namespace sf;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvs;
using namespace ssvsc;
using namespace ssvsc::Utils;
using namespace sses;

namespace ob
{
	OBFactory::OBFactory(OBAssets& mAssets, OBGame& mGame, Manager& mManager, World& mWorld) : assets(mAssets), game(mGame), manager(mManager), world(mWorld) { }

	Entity& OBFactory::createFloor(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<OBCPhysics>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		Sprite s{assets.get<Texture>("tileset.png")};
		s.setTextureRect(assets.tileset[{0, 0}]);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createWall(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		auto& cPhysics(result.createComponent<OBCPhysics>(world, true, mPos, Vec2i{1000, 1000}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroupToCheck(OBGroup::Solid);
		body.setVelTransferMultX(1.f);
		body.setVelTransferMultY(1.f);

		Sprite s{assets.get<Texture>("tileset.png")};
		s.setTextureRect(assets.tileset[{1, 0}]);

		body.setStressMult(0.f);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
	Entity& OBFactory::createPlayer(const Vec2i& mPos)
	{
		auto& result(manager.createEntity());
		result.setDrawPriority(-1000);
		auto& cPhysics(result.createComponent<OBCPhysics>(world, false, mPos, Vec2i{700, 700}));
		auto& cRender(result.createComponent<OBCRender>(game, cPhysics.getBody()));

		Body& body(cPhysics.getBody());
		body.addGroup(OBGroup::Solid);
		body.addGroupToCheck(OBGroup::Solid);
		body.setVelTransferMultX(1.f);
		body.setVelTransferMultY(1.f);

		Sprite s{assets.get<Texture>("tilesetPlayer.png")};
		s.setTextureRect(assets.tilesetPlayer[{0, 0}]);

		body.setStressMult(0.f);

		cRender.addSprite(s);
		cRender.setScaleWithBody(false);

		return result;
	}
}

